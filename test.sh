#!/bin/bash

HOST="127.0.0.1"
PORT="4444"
PASS="senha123"
PASS_WRONG="senhaerrada"

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

# envia comandos IRC e captura output, fecha após 1s sem atividade
irc_cmd()
{
	local cmds="$1"
	local delay="${2:-1}"
	(echo -e "$cmds"; sleep "$delay") | timeout "$((delay + 1))" nc -C "$HOST" "$PORT" 2>/dev/null
}

# ---------- AUTENTICAÇÃO ----------
section "AUTENTICAÇÃO"

out=$(irc_cmd "PASS $PASS_WRONG\r\nNICK test1\r\nUSER test1 0 * test1")
if echo "$out" | grep -qi "464\|incorrect\|password\|ERR\|error"; then
	pass "Senha errada → servidor rejeita"
else
	fail "Senha errada → servidor deveria rejeitar" "$out"
fi

out=$(irc_cmd "NICK test2\r\nUSER test2 0 * test2")
if echo "$out" | grep -qi "464\|451\|password\|ERR\|error\|register"; then
	pass "Sem PASS → servidor rejeita"
else
	fail "Sem PASS → servidor deveria rejeitar" "$out"
fi

out=$(irc_cmd "PASS $PASS\r\nNICK test3\r\nUSER test3 0 * test3")
if echo "$out" | grep -q "001\|Welcome"; then
	pass "Login correto → recebe 001 Welcome"
else
	fail "Login correto → deveria receber 001 Welcome" "$out"
fi

# ---------- NICK ----------
section "NICK"

(echo -e "PASS $PASS\r\nNICK dupnick\r\nUSER dup1 0 * dup1"; sleep 4) | timeout 5 nc -C "$HOST" "$PORT" > /dev/null 2>&1 &
BG_PID=$!
sleep 0.8

out=$(irc_cmd "PASS $PASS\r\nNICK dupnick\r\nUSER dup2 0 * dup2")
if echo "$out" | grep -q "433\|already\|use"; then
	pass "Nick duplicado → erro 433"
else
	fail "Nick duplicado → deveria receber 433" "$out"
fi
kill $BG_PID 2>/dev/null; wait $BG_PID 2>/dev/null

out=$(irc_cmd "PASS $PASS\r\nNICK testnick\r\nUSER testnick 0 * testnick\r\nNICK")
if echo "$out" | grep -q "431\|No nickname"; then
	pass "NICK sem argumento → erro 431"
else
	fail "NICK sem argumento → deveria receber 431" "$out"
fi

out=$(irc_cmd "PASS $PASS\r\nNICK oldnick\r\nUSER oldnick 0 * oldnick\r\nNICK newnick")
if echo "$out" | grep -q "NICK\|newnick"; then
	pass "Trocar nick após registro → confirmação recebida"
else
	fail "Trocar nick após registro → deveria confirmar" "$out"
fi

# ---------- PRIVMSG ----------
section "PRIVMSG"

out=$(irc_cmd "PASS $PASS\r\nNICK sender1\r\nUSER sender1 0 * sender1\r\nPRIVMSG")
if echo "$out" | grep -q "411\|No recipient"; then
	pass "PRIVMSG sem destinatário → erro 411"
else
	fail "PRIVMSG sem destinatário → deveria receber 411" "$out"
fi

out=$(irc_cmd "PASS $PASS\r\nNICK sender2\r\nUSER sender2 0 * sender2\r\nPRIVMSG alguem")
if echo "$out" | grep -q "412\|No text"; then
	pass "PRIVMSG sem texto → erro 412"
else
	fail "PRIVMSG sem texto → deveria receber 412" "$out"
fi

out=$(irc_cmd "PASS $PASS\r\nNICK sender3\r\nUSER sender3 0 * sender3\r\nPRIVMSG ninguem :oi")
if echo "$out" | grep -q "401\|No such"; then
	pass "PRIVMSG para nick inexistente → erro 401"
else
	fail "PRIVMSG para nick inexistente → deveria receber 401" "$out"
fi

(echo -e "PASS $PASS\r\nNICK recvnick\r\nUSER recv 0 * recv"; sleep 4) | timeout 5 nc -C "$HOST" "$PORT" > /tmp/irc_recv_output 2>&1 &
RECV_PID=$!
sleep 0.8

irc_cmd "PASS $PASS\r\nNICK sendnick\r\nUSER send 0 * send\r\nPRIVMSG recvnick :mensagem de teste" > /dev/null
sleep 0.5

kill $RECV_PID 2>/dev/null; wait $RECV_PID 2>/dev/null

if grep -q "mensagem de teste" /tmp/irc_recv_output 2>/dev/null; then
	pass "PRIVMSG entre clientes → mensagem entregue"
else
	fail "PRIVMSG entre clientes → mensagem não entregue" "$(cat /tmp/irc_recv_output)"
fi
rm -f /tmp/irc_recv_output

# ---------- QUIT ----------
section "QUIT"

out=$(irc_cmd "PASS $PASS\r\nNICK quitnick\r\nUSER quit 0 * quit\r\nQUIT :tchau")
if echo "$out" | grep -q "Closing Link\|ERROR\|tchau"; then
	pass "QUIT com motivo → recebe ERROR Closing Link"
else
	fail "QUIT com motivo → deveria receber ERROR Closing Link" "$out"
fi

out=$(irc_cmd "PASS $PASS\r\nNICK quitnick2\r\nUSER quit2 0 * quit2\r\nQUIT")
if echo "$out" | grep -q "Closing Link\|ERROR\|Leaving"; then
	pass "QUIT sem motivo → recebe ERROR com motivo padrão"
else
	fail "QUIT sem motivo → deveria receber ERROR com Leaving" "$out"
fi

# ---------- MÚLTIPLOS CLIENTES ----------
section "MÚLTIPLOS CLIENTES"

PIDS=()
for i in 1 2 3; do
	(echo -e "PASS $PASS\r\nNICK multiclient$i\r\nUSER multi$i 0 * multi$i"; sleep 3) | timeout 4 nc -C "$HOST" "$PORT" > /tmp/irc_multi_$i 2>&1 &
	PIDS+=($!)
done
sleep 1.5

ALL_OK=true
for i in 1 2 3; do
	if ! grep -q "001\|Welcome" /tmp/irc_multi_$i 2>/dev/null; then
		ALL_OK=false
	fi
done

if $ALL_OK; then
	pass "3 clientes simultâneos → todos recebem Welcome"
else
	fail "3 clientes simultâneos → nem todos receberam Welcome" ""
fi

for pid in "${PIDS[@]}"; do
	kill $pid 2>/dev/null; wait $pid 2>/dev/null
done
rm -f /tmp/irc_multi_*

# ---------- PING/PONG ----------
section "PING/PONG"

out=$(irc_cmd "PASS $PASS\r\nNICK pingnick\r\nUSER ping 0 * ping\r\nPING :token123")
if echo "$out" | grep -q "PONG\|token123"; then
	pass "PING → recebe PONG com token"
else
	fail "PING → deveria receber PONG" "$out"
fi

# ---------- RESULTADO ----------
echo -e "\n${CYAN}================================${NC}"
echo -e "${GREEN}PASSOU: $PASS_COUNT${NC}"
echo -e "${RED}FALHOU: $FAIL_COUNT${NC}"
echo -e "${CYAN}TOTAL:  $((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "${CYAN}================================${NC}"
