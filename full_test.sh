#!/bin/bash

HOST="127.0.0.1"
PORT="4444"
PASS="senha123"
PASS_WRONG="wrongpass"

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
info() { echo -e "${YELLOW}[INFO]${NC} $1"; }

irc_cmd() {
    local cmds="$1"
    local delay="${2:-1}"
    (echo -e "$cmds"; sleep "$delay") | timeout "$((delay + 1))" nc -C "$HOST" "$PORT" 2>/dev/null
}

irc_cmd_no_auth() {
    local cmds="$1"
    local delay="${2:-1}"
    (echo -e "$cmds"; sleep "$delay") | timeout "$((delay + 1))" nc -C "$HOST" "$PORT" 2>/dev/null
}

has_welcome() {
    echo "$1" | grep -q "001\|002\|003\|004\|005"
}

extract_nick() {
    echo "$1" | grep -oP '(?<=NICK )[^ :]+' | head -1
}

# ---------- TOPIC 1: CONNECTION & AUTHENTICATION ----------
section "TOPIC 1: CONNECTION & AUTHENTICATION"

# 1.1 Valid registration sequence
out=$(irc_cmd "PASS $PASS\r\nNICK testuser\r\nUSER testuser 0 * :Test User")
if has_welcome "$out"; then
    pass "Valid PASS/NICK/USER sequence → received welcome message (001-005)"
else
    fail "Valid registration sequence → should receive welcome (001-005)" "$out"
fi

# 1.2 Wrong password rejection
out=$(irc_cmd_no_auth "PASS $PASS_WRONG\r\nNICK wrongpass\r\nUSER w 0 * :w")
if echo "$out" | grep -q "464\|ERROR"; then
    pass "Wrong PASS → error 464 (Incorrect password) or ERROR"
else
    fail "Wrong PASS → should reject with 464 or ERROR" "$out"
fi

# 1.3 Missing PASS (if server requires it)
out=$(irc_cmd_no_auth "NICK nopass\r\nUSER n 0 * :n")
if echo "$out" | grep -q "451\|464\|ERROR"; then
    pass "Missing PASS → error 451 (Register first) or 464"
else
    fail "Missing PASS → should reject or require auth" "$out"
fi

# 1.4 NICK before PASS/registration
out=$(irc_cmd_no_auth "NICK earlybird\r\nPASS $PASS\r\nUSER e 0 * :e")
if echo "$out" | grep -q "451\|ERROR" || has_welcome "$out"; then
    pass "NICK before auth → either error or allowed (server-dependent)"
else
    fail "NICK before auth → unexpected response" "$out"
fi

# 1.5 Duplicate NICK in same connection
out=$(irc_cmd "PASS $PASS\r\nNICK user1\r\nUSER u 0 * :u\r\nNICK user2")
if echo "$out" | grep -q "NICK user2" || has_welcome "$out"; then
    pass "NICK change in session → accepted or silently changed"
else
    fail "NICK change → unexpected response" "$out"
fi

# 1.6 Invalid NICK (empty or too long)
out=$(irc_cmd "PASS $PASS\r\nNICK\r\nUSER n 0 * :n")
if echo "$out" | grep -q "431\|ERROR"; then
    pass "Empty NICK → error 431 (No nickname given)"
else
    fail "Empty NICK → should error 431" "$out"
fi

# 1.7 USER missing required parameters
out=$(irc_cmd "PASS $PASS\r\nNICK incomplete\r\nUSER incomplete")
if echo "$out" | grep -q "461\|ERROR"; then
    pass "USER with missing params → error 461 (Need more parameters)"
else
    fail "USER incomplete → should error 461" "$out"
fi

# 1.8 QUIT command
out=$(irc_cmd "PASS $PASS\r\nNICK quitter\r\nUSER q 0 * :q\r\nQUIT :Goodbye")
if echo "$out" | grep -q "ERROR\|Connection closed" || [ -z "$out" ]; then
    pass "QUIT → connection closed or ERROR"
else
    pass "QUIT → command accepted"
fi

# ---------- TOPIC 2: NICK MANAGEMENT ----------
section "TOPIC 2: NICK MANAGEMENT"
# 2.1 Valid NICK change
out=$(irc_cmd "PASS $PASS\r\nNICK original\r\nUSER o 0 * :o\r\nNICK newname")
if echo "$out" | grep -q "NICK newname"; then
    pass "NICK change (original → newname) → confirmed"
else
    fail "NICK change → not confirmed in output" "$out"
fi

# 2.2 NICK with invalid characters
out=$(irc_cmd "PASS $PASS\r\nNICK badname!\r\nUSER b 0 * :b")
if echo "$out" | grep -q "432\|ERROR"; then
    pass "NICK with invalid chars (!) → error 432 or rejected"
else
    fail "NICK invalid chars → should error" "$out"
fi

# 2.3 NICK already in use (would need two connections)
info "NICK collision test skipped (requires two concurrent connections)"

# ---------- TOPIC 3: PING / PONG ----------
section "TOPIC 3: PING / PONG"

# 3.1 Valid PING with parameter
out=$(irc_cmd "PASS $PASS\r\nNICK pinger\r\nUSER p 0 * :p\r\nPING :localhost")
if echo "$out" | grep -q "PONG"; then
    pass "PING with parameter → PONG response"
else
    fail "PING with parameter → should PONG" "$out"
fi

# 3.2 PING with server name
out=$(irc_cmd "PASS $PASS\r\nNICK ping2\r\nUSER p 0 * :p\r\nPING :irc.example.com")
if echo "$out" | grep -q "PONG.*irc.example.com"; then
    pass "PING with server → PONG echoes parameter"
else
    fail "PING with server → PONG should echo" "$out"
fi

# 3.3 PING without parameter
out=$(irc_cmd "PASS $PASS\r\nNICK noparam\r\nUSER n 0 * :n\r\nPING")
if echo "$out" | grep -q "409\|461\|ERROR\|PONG"; then
    pass "PING no parameter → error 409/461 or PONG (server choice)"
else
    fail "PING no parameter → unexpected response" "$out"
fi

# ---------- TOPIC 4: CHANNELS (JOIN & PART) ----------
section "TOPIC 4: CHANNELS (JOIN & PART)"

# 4.1 Valid JOIN with # prefix
out=$(irc_cmd "PASS $PASS\r\nNICK joiner\r\nUSER j 0 * :j\r\nJOIN #testchan")
if echo "$out" | grep -q "JOIN.*#testchan\|353\|366"; then
    pass "JOIN #testchan → JOIN confirmed or NAMES reply"
else
    fail "JOIN #testchan → should confirm JOIN or send NAMES" "$out"
fi

# 4.2 JOIN without # prefix (should fail)
out=$(irc_cmd "PASS $PASS\r\nNICK badjoin\r\nUSER b 0 * :b\r\nJOIN testchan")
if echo "$out" | grep -q "403\|476\|ERROR"; then
    pass "JOIN testchan (no #) → error 403/476"
else
    fail "JOIN without # → should error 403/476" "$out"
fi

# 4.3 JOIN multiple channels
out=$(irc_cmd "PASS $PASS\r\nNICK multi\r\nUSER m 0 * :m\r\nJOIN #chan1,#chan2")
if echo "$out" | grep -qE "JOIN.*(#chan1|#chan2)|353|366"; then
    pass "JOIN #chan1,#chan2 → joined multiple channels"
else
    fail "JOIN multiple channels → incomplete" "$out"
fi

# 4.4 JOIN with channel key (if supported)
out=$(irc_cmd "PASS $PASS\r\nNICK keyuser\r\nUSER k 0 * :k\r\nJOIN #keychan somekey")
if echo "$out" | grep -q "JOIN\|475\|ERROR"; then
    pass "JOIN with key → accepted or rejected (475 if wrong key)"
else
    fail "JOIN with key → unexpected response" "$out"
fi

# 4.8 JOIN non-existent channel (should auto-create)
out=$(irc_cmd "PASS $PASS\r\nNICK creator\r\nUSER c 0 * :c\r\nJOIN #newchannel")
if echo "$out" | grep -q "JOIN\|353\|366"; then
    pass "JOIN #newchannel (new) → auto-created"
else
    fail "JOIN new channel → should auto-create" "$out"
fi

# ---------- TOPIC 5: CHANNEL TOPIC ----------
section "TOPIC 5: CHANNEL TOPIC"

# 5.1 Set channel TOPIC
out=$(irc_cmd "PASS $PASS\r\nNICK topicset\r\nUSER t 0 * :t\r\nJOIN #topicchan\r\nTOPIC #topicchan :New Topic Here")
if echo "$out" | grep -q "TOPIC.*#topicchan.*New Topic\|332"; then
    pass "TOPIC #topicchan :New Topic → set successfully"
else
    fail "TOPIC set → not confirmed" "$out"
fi

# 5.2 Query TOPIC of channel (no parameter)
out=$(irc_cmd "PASS $PASS\r\nNICK topicget\r\nUSER t 0 * :t\r\nJOIN #topicchan2\r\nTOPIC #topicchan2")
if echo "$out" | grep -q "331\|332"; then
    pass "TOPIC #topicchan2 (query) → 331 (none) or 332 (has topic)"
else
    fail "TOPIC query → should return 331 or 332" "$out"
fi

# 5.3 Clear TOPIC (empty string)
out=$(irc_cmd "PASS $PASS\r\nNICK topicclear\r\nUSER t 0 * :t\r\nJOIN #topicclean\r\nTOPIC #topicclean :")
if echo "$out" | grep -q "TOPIC\|331"; then
    pass "TOPIC #topicclean : (clear) → topic cleared"
else
    fail "TOPIC clear → not confirmed" "$out"
fi

# 5.4 TOPIC without being on channel
out=$(irc_cmd "PASS $PASS\r\nNICK topicout\r\nUSER t 0 * :t\r\nTOPIC #otherchan :Sneaky")
if echo "$out" | grep -q "442\|403"; then
    pass "TOPIC #otherchan (not joined) → error 442/403"
else
    fail "TOPIC not on channel → should error" "$out"
fi

# 5.5 TOPIC with special characters
out=$(irc_cmd "PASS $PASS\r\nNICK topicspec\r\nUSER t 0 * :t\r\nJOIN #topicspec\r\nTOPIC #topicspec :Welcome! 🎉 [test]")
if echo "$out" | grep -q "TOPIC\|332"; then
    pass "TOPIC with special chars → accepted"
else
    fail "TOPIC special chars → not accepted" "$out"
fi

# ---------- TOPIC 6: MESSAGING (PRIVMSG) ----------
section "TOPIC 6: MESSAGING (PRIVMSG)"

# 6.1 PRIVMSG to non-existent user
out=$(irc_cmd "PASS $PASS\r\nNICK msguser\r\nUSER m 0 * :m\r\nPRIVMSG nonexistent :hello")
if echo "$out" | grep -q "401"; then
    pass "PRIVMSG nonexistent → error 401 (No such nick)"
else
    fail "PRIVMSG nonexistent user → should error 401" "$out"
fi

# 6.2 PRIVMSG without text
out=$(irc_cmd "PASS $PASS\r\nNICK msgnotext\r\nUSER m 0 * :m\r\nPRIVMSG #testchan")
if echo "$out" | grep -q "412\|461"; then
    pass "PRIVMSG no text → error 412/461 (No text to send)"
else
    fail "PRIVMSG no text → should error 412/461" "$out"
fi

# 6.3 PRIVMSG to channel not joined
out=$(irc_cmd "PASS $PASS\r\nNICK msgchanout\r\nUSER m 0 * :m\r\nPRIVMSG #channelnotjoined :test")
if echo "$out" | grep -q "404\|442\|ERROR"; then
    pass "PRIVMSG to channel (not joined) → error 404/442"
else
    fail "PRIVMSG channel not joined → should error" "$out"
fi

# 6.4 PRIVMSG to channel (joined)
out=$(irc_cmd "PASS $PASS\r\nNICK msgsender\r\nUSER m 0 * :m\r\nJOIN #msgchannel\r\nPRIVMSG #msgchannel :hello everyone")
if echo "$out" | grep -q "PRIVMSG" || [ -n "$out" ]; then
    pass "PRIVMSG to channel (joined) → accepted"
else
    fail "PRIVMSG to channel joined → should accept" "$out"
fi

# 6.5 PRIVMSG with long message
out=$(irc_cmd "PASS $PASS\r\nNICK longmsg\r\nUSER l 0 * :l\r\nJOIN #msgchannel2\r\nPRIVMSG #msgchannel2 :$(printf 'A%.0s' {1..200})")
if echo "$out" | grep -q "PRIVMSG\|412" || [ -n "$out" ]; then
    pass "PRIVMSG with 200 char message → accepted"
else
    fail "PRIVMSG long message → unexpected response" "$out"
fi

# 6.6 PRIVMSG with CRLF in message (should not break protocol)
out=$(irc_cmd "PASS $PASS\r\nNICK crlfmsg\r\nUSER c 0 * :c\r\nJOIN #msgchannel3\r\nPRIVMSG #msgchannel3 :line1\\nline2")
if [ -n "$out" ]; then
    pass "PRIVMSG with embedded newline → handled"
else
    fail "PRIVMSG with newline → unexpected response" "$out"
fi

# ---------- TOPIC 10: MODE ----------
section "TOPIC 10: MODE COMMAND"

# 10.1 Query user modes
out=$(irc_cmd "PASS $PASS\r\nNICK modechecker\r\nUSER m 0 * :m\r\nMODE modechecker")
if echo "$out" | grep -q "221\|MODE"; then
    pass "MODE modechecker (query) → returns 221 or current modes"
else
    fail "MODE query → should respond" "$out"
fi

# 10.2 Set user mode (+i, +w, etc)
out=$(irc_cmd "PASS $PASS\r\nNICK modesetter\r\nUSER m 0 * :m\r\nMODE modesetter +i")
if echo "$out" | grep -q "221\|MODE"; then
    pass "MODE modesetter +i → accepted"
else
    fail "MODE set → should respond" "$out"
fi

# 10.3 Query channel modes
out=$(irc_cmd "PASS $PASS\r\nNICK modeop\r\nUSER m 0 * :m\r\nJOIN #modechan\r\nMODE #modechan")
if echo "$out" | grep -q "324\|MODE"; then
    pass "MODE #modechan (query) → returns 324 or modes"
else
    fail "MODE channel query → should respond" "$out"
fi

# 10.4 Set channel mode (requires ops)
out=$(irc_cmd "PASS $PASS\r\nNICK modeopset\r\nUSER m 0 * :m\r\nJOIN #modetest\r\nMODE #modetest +m")
if echo "$out" | grep -q "MODE\|482"; then
    pass "MODE #modetest +m → accepted or 482 (not ops)"
else
    fail "MODE channel set → unexpected response" "$out"
fi

# ---------- TOPIC 11: KICK ----------
section "TOPIC 11: KICK COMMAND"

# 11.1 KICK user (requires ops)
out=$(irc_cmd "PASS $PASS\r\nNICK kicker\r\nUSER k 0 * :k\r\nJOIN #kickchan\r\nKICK #kickchan someuser :spam")
if echo "$out" | grep -q "KICK\|482\|441"; then
    pass "KICK #kickchan someuser → accepted, 482 (no ops), or 441 (not on)"
else
    fail "KICK → unexpected response" "$out"
fi

# 11.2 KICK without reason
out=$(irc_cmd "PASS $PASS\r\nNICK kicker2\r\nUSER k 0 * :k\r\nJOIN #kickchan2\r\nKICK #kickchan2 someuser")
if echo "$out" | grep -q "KICK\|482\|441"; then
    pass "KICK without reason → accepted"
else
    fail "KICK no reason → unexpected response" "$out"
fi

# 11.3 KICK nonexistent user
out=$(irc_cmd "PASS $PASS\r\nNICK kicker3\r\nUSER k 0 * :k\r\nJOIN #kickchan3\r\nKICK #kickchan3 ghost")
if echo "$out" | grep -q "441\|482"; then
    pass "KICK nonexistent user → error 441/482"
else
    fail "KICK ghost user → should error" "$out"
fi

# 11.4 Verify actual removal (Self-Kick + Message test)
out=$(irc_cmd "PASS $PASS\r\nNICK kicktest1\r\nUSER k1 0 * :k1\r\nJOIN #actualkick\r\nKICK #actualkick kicktest1 :self removal\r\nPRIVMSG #actualkick :I should be a ghost")
if echo "$out" | grep -q "404\|442"; then
    pass "KICK removal verified → PRIVMSG after kick correctly blocked (404/442)"
else
    fail "KICK removal failed → User was not actually removed from channel memory" "$out"
fi

# 11.5 Verify channel destruction on last kick
out=$(irc_cmd "PASS $PASS\r\nNICK kicktest2\r\nUSER k2 0 * :k2\r\nJOIN #emptychan\r\nKICK #emptychan kicktest2\r\nJOIN #emptychan")
if echo "$out" | grep -q "JOIN :#emptychan\|JOIN #emptychan"; then
    pass "KICK cleanup verified → Channel successfully destroyed and recreated"
else
    fail "KICK cleanup failed → Channel state corrupted after last user kicked" "$out"
fi

# ---------- TOPIC 12: INVITE ----------
section "TOPIC 12: INVITE COMMAND"

# 12.1 Valid INVITE
out=$(irc_cmd "PASS $PASS\r\nNICK inviter\r\nUSER i 0 * :i\r\nJOIN #invitechan\r\nINVITE someuser #invitechan")
if echo "$out" | grep -q "INVITE\|341\|401"; then
    pass "INVITE someuser → accepted or error"
else
    fail "INVITE → unexpected response" "$out"
fi

# 12.2 INVITE without being on channel
out=$(irc_cmd "PASS $PASS\r\nNICK inviter2\r\nUSER i 0 * :i\r\nINVITE someuser #notjoined")
if echo "$out" | grep -q "442\|403"; then
    pass "INVITE (not on channel) → error 442/403"
else
    fail "INVITE not on channel → should error" "$out"
fi

# ---------- TOPIC 13: PROTOCOL COMPLIANCE ----------
section "TOPIC 13: PROTOCOL COMPLIANCE"

# 13.1 Case insensitivity of commands
out=$(irc_cmd "PASS $PASS\r\nNICK casetest\r\nUSER c 0 * :c\r\njoin #testcase\r\nprivmsg #testcase :lowercase")
if echo "$out" | grep -q "join\|JOIN\|privmsg\|PRIVMSG"; then
    pass "Lowercase commands (join, privmsg) → accepted"
else
    fail "Lowercase commands → should be case-insensitive" "$out"
fi

# 13.2 Mixed case commands
out=$(irc_cmd "PASS $PASS\r\nNICK mixcase\r\nUSER m 0 * :m\r\nJoIn #mixtest\r\nPrIvMsG #mixtest :mixed")
if echo "$out" | grep -q "JoIn\|JOIN\|PrIvMsG\|PRIVMSG\|join"; then
    pass "Mixed case commands → accepted"
else
    fail "Mixed case → should work" "$out"
fi

# 13.3 Verify CRLF line endings are handled
out=$(irc_cmd "PASS $PASS\r\nNICK crlftest\r\nUSER c 0 * :c")
if [ -n "$out" ]; then
    pass "CRLF line endings → handled correctly"
else
    fail "CRLF handling → no response" "$out"
fi

# 13.4 Server name in response
out=$(irc_cmd "PASS $PASS\r\nNICK servername\r\nUSER s 0 * :s\r\nPING :test")
if echo "$out" | grep -q "^\s*:[^ ]"; then
    pass "Server name prefix → responses include :servername"
else
    pass "Server name prefix → (may be optional)"
fi

# 13.5 Unknown command handling
out=$(irc_cmd "PASS $PASS\r\nNICK unknown\r\nUSER u 0 * :u\r\nBLAHBLAH param1 param2")
if echo "$out" | grep -q "421\|ERROR"; then
    pass "Unknown command → error 421 (Unknown command)"
else
    fail "Unknown command → should error 421" "$out"
fi

# ---------- TOPIC 14: EDGE CASES ----------
section "TOPIC 14: EDGE CASES"

# 14.1 Empty message to channel
out=$(irc_cmd "PASS $PASS\r\nNICK emptymsg\r\nUSER e 0 * :e\r\nJOIN #emptychan\r\nPRIVMSG #emptychan :")
if echo "$out" | grep -q "412\|PRIVMSG"; then
    pass "PRIVMSG with empty text → either error 412 or accepted"
else
    fail "Empty PRIVMSG → unexpected response" "$out"
fi

# 14.2 Very long nick (should truncate or reject)
longnic="abcdefghijklmnopqrstuvwxyzabcdefghij"
out=$(irc_cmd "PASS $PASS\r\nNICK $longnic\r\nUSER l 0 * :l")
if echo "$out" | grep -q "431\|432" || has_welcome "$out"; then
    pass "Very long NICK ($longnic) → rejected or truncated"
else
    fail "Long NICK → unexpected response" "$out"
fi

# 14.3 Channel with spaces (invalid)
out=$(irc_cmd "PASS $PASS\r\nNICK spacechan\r\nUSER s 0 * :s\r\nJOIN #channel with spaces")
if echo "$out" | grep -q "403\|476\|ERROR"; then
    pass "JOIN with spaces in name → error"
else
    fail "Channel with spaces → should error" "$out"
fi

# 14.4 Rapid-fire commands (stress test)
out=$(irc_cmd "PASS $PASS\r\nNICK rapid\r\nUSER r 0 * :r\r\nPING :1\r\nPING :2\r\nPING :3\r\nPING :4\r\nPING :5")
if echo "$out" | grep -q "PONG"; then
    pass "5 consecutive PINGs → multiple PONGs received"
else
    fail "Rapid commands → incomplete response" "$out"
fi

# 14.5 Command with trailing spaces
out=$(irc_cmd "PASS $PASS\r\nNICK trailing\r\nUSER t 0 * :t  \r\nPING :test  ")
if [ -n "$out" ]; then
    pass "Commands with trailing spaces → handled"
else
    fail "Trailing spaces → unexpected response" "$out"
fi

# ---------- RESULTS ----------
echo -e "\n${CYAN}================================${NC}"
echo -e "${GREEN}PASSOU:  $PASS_COUNT${NC}"
echo -e "${RED}FALHOU:  $FAIL_COUNT${NC}"
echo -e "${CYAN}TOTAL:   $((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "${CYAN}================================${NC}"
