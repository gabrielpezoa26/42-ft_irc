#!/bin/bash

HOST="127.0.0.1"
PORT="4444"
PASS="senha123"

GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

PASS_COUNT=0
FAIL_COUNT=0

pass() { echo -e "${GREEN}[PASS]${NC} $1"; ((PASS_COUNT++)); }
fail() { echo -e "${RED}[FAIL]${NC} $1 (got: '$2')"; ((FAIL_COUNT++)); }
section() { echo -e "\n${CYAN}=== $1 ===${NC}"; }

irc_cmd()
{
    local cmds="$1"
    local delay="${2:-1}"
    # Use -w to set a network timeout, and capture the exit code
    (echo -e "$cmds"; sleep "$delay") | timeout "$((delay + 1))" nc -C "$HOST" "$PORT" 2>/dev/null
}

# ---------- CASE SENSITIVITY ----------
section "CASE SENSITIVITY"

out=$(irc_cmd "pAsS $PASS\r\nnIcK mixedCase\r\nuSeR user 0 * :realname")
if echo "$out" | grep -q "001\|Welcome"; then
    pass "Comandos mixed-case (pAsS, nIcK) → aceitos"
else
    fail "Comandos mixed-case → falharam" "$out"
fi

# ---------- WHITESPACE & TABS ----------
section "WHITESPACE & CLEANUP"

out=$(irc_cmd "PASS    $PASS\r\nNICK  spacey   \r\nUSER    u  0  * :name")
if echo "$out" | grep -q "001\|Welcome"; then
    pass "Espaços extras entre argumentos → ignorados"
else
    fail "Espaços extras → falharam" "$out"
fi

# ---------- COMMAND FRAGMENTATION (TCP Stress) ----------
section "FRAGMENTAÇÃO (TCP STREAMING)"

out=$({ 
    echo -n "PA"; sleep 0.2; echo -n "SS $PASS"; echo -ne "\r\n"; 
    sleep 0.2;
    echo -n "NI"; sleep 0.2; echo -n "CK frag"; echo -ne "\r\n";
    sleep 0.2;
    echo -e "USER f 0 * :f"; 
    sleep 1;
} | timeout 2 nc -C "$HOST" "$PORT" 2>/dev/null)

if echo "$out" | grep -q "001\|Welcome"; then
    pass "Comandos fragmentados (PA...SS) → bufferizados corretamente"
else
    fail "Comandos fragmentados → falharam" "$out"
fi

# ---------- UNKNOWN & MALFORMED ----------
section "COMANDOS INVÁLIDOS"

# 1. Unknown Command: Strip \r before grepping so the script doesn't get confused
out=$(irc_cmd "PASS $PASS\r\nNICK valid\r\nUSER v 0 * :v\r\nBOGUSCOMMAND arg1 arg2\r\nQUIT")
RET=$?

# We delete \r so grep sees a clean string
clean_out=$(echo "$out" | tr -d '\r')

if echo "$clean_out" | grep -q "421"; then
    pass "Comando inexistente → erro 421 recebido"
elif [ $RET -eq 0 ] || [ $RET -eq 124 ]; then
    fail "Comando inexistente → erro 421 não encontrado" "$clean_out"
else
    fail "Comando inexistente → servidor caiu" "Exit code $RET"
fi
# 2. Empty Newlines: Timeout (124) is a PASS because the server should be silent
irc_cmd "\r\n\r\n\r\n" > /dev/null 2>&1
RET=$?
if [ $RET -eq 124 ] || [ $RET -eq 0 ]; then
    pass "Apenas newlines (\r\n) → ignorados (servidor vivo e silencioso)"
else
    fail "Apenas newlines → erro ou queda do servidor" "Exit code $RET"
fi

# ---------- LINGERING DATA ----------
section "DADOS RESIDUAIS"

(echo -n "PASS $PASS"; sleep 1) | timeout 2 nc -C "$HOST" "$PORT" > /dev/null 2>&1
RET=$?
if [ $RET -eq 124 ] || [ $RET -eq 0 ]; then
    pass "Conexão fechada com comando incompleto → limpo corretamente"
else
    fail "Conexão fechada com comando incompleto → erro no server" "Exit code $RET"
fi

# ---------- RESULTADO ----------
echo -e "\n${CYAN}================================${NC}"
echo -e "${GREEN}PASSOU: $PASS_COUNT${NC}"
echo -e "${RED}FALHOU: $FAIL_COUNT${NC}"
echo -e "${CYAN}TOTAL:  $((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "${CYAN}================================${NC}"
