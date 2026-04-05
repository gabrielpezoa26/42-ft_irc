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

irc_cmd() {
    local cmds="$1"
    (echo -e "$cmds"; sleep 1) | timeout 2 nc -C "$HOST" "$PORT" 2>/dev/null
}

# ---------- TOPIC 1: NICKNAME EDGE CASES ----------
section "TOPIC 1: NICKNAME VALIDATION (RFC 2812)"

# 1.1 Illegal characters (starting with a digit)
out=$(irc_cmd "PASS $PASS\r\nNICK 123nick")
if echo "$out" | grep -q "432"; then
    pass "Nick começando com número → erro 432 (Erroneous Nickname)"
else
    fail "Nick começando com número → deveria retornar 432" "$out"
fi

# 1.2 Forbidden characters (special chars)
out=$(irc_cmd "PASS $PASS\r\nNICK nick!name")
if echo "$out" | grep -q "432"; then
    pass "Nick com caracteres proibidos (!) → erro 432"
else
    fail "Nick com caracteres proibidos → deveria retornar 432" "$out"
fi

# 1.3 Same Nick Swap (changing to the current nick)
# A user shouldn't get an error, but the server should handle it silently or confirm.
out=$(irc_cmd "PASS $PASS\r\nNICK gabs\r\nUSER g 0 * :g\r\nNICK gabs")
if echo "$out" | grep -q "433"; then
    fail "Trocar para o mesmo nick → erro 433 (Não deveria dar erro de 'em uso')" "$out"
else
    pass "Trocar para o mesmo nick → lidado corretamente"
fi


# ---------- TOPIC 2: STATE MACHINE VIOLATIONS ----------
section "TOPIC 2: REGISTRATION STATE MACHINE"

# 2.1 Double PASS (Already Registered)
out=$(irc_cmd "PASS $PASS\r\nNICK user1\r\nUSER u 0 * :u\r\nPASS $PASS")
if echo "$out" | grep -q "462"; then
    pass "Segundo PASS após registro → erro 462 (Already Registered)"
else
    fail "Segundo PASS após registro → deveria retornar 462" "$out"
fi

# 2.2 Double USER (Attempting to change username)
out=$(irc_cmd "PASS $PASS\r\nNICK user2\r\nUSER u2 0 * :u2\r\nUSER hacker 0 * :hacker")
if echo "$out" | grep -q "462"; then
    pass "Segundo USER após registro → erro 462"
else
    fail "Segundo USER após registro → deveria retornar 462" "$out"
fi

# 2.3 Incomplete USER command (Parameters missing)
out=$(irc_cmd "PASS $PASS\r\nNICK user3\r\nUSER guest 0 *")
if echo "$out" | grep -q "461"; then
    pass "USER com parâmetros insuficientes → erro 461 (Need more params)"
else
    fail "USER incompleto → deveria retornar 461" "$out"
fi

# ---------- RESULTADO ----------
echo -e "\n${CYAN}================================${NC}"
echo -e "${GREEN}PASSOU: $PASS_COUNT${NC}"
echo -e "${RED}FALHOU: $FAIL_COUNT${NC}"
echo -e "${CYAN}TOTAL:  $((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "${CYAN}================================${NC}"