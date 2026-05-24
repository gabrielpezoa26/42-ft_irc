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
info() { echo -e "${YELLOW}[INFO]${NC} $1"; }

irc_cmd() {
    local cmds="$1"
    local delay="${2:-1}"
    (echo -e "$cmds"; sleep "$delay") | timeout "$((delay + 1))" nc -C "$HOST" "$PORT" 2>/dev/null
    
}

# ---------- TOPIC 1: MODE GENERAL & PERMISSIONS ----------
section "TOPIC 1: MODE GENERAL & PERMISSIONS"

# 1.1 Query channel modes (No flags)
out=$(irc_cmd "PASS $PASS\r\nNICK m_query\r\nUSER m 0 * :m\r\nJOIN #querychan\r\nMODE #querychan")
if echo "$out" | grep -q "324\|MODE"; then
    pass "MODE #querychan (query) → returns 324 (RPL_CHANNELMODEIS)"
else
    fail "MODE query → should respond with 324" "$out"
fi

# 1.2 MODE on non-existent channel
out=$(irc_cmd "PASS $PASS\r\nNICK m_nochan\r\nUSER m 0 * :m\r\nMODE #ghostchan +i")
if echo "$out" | grep -q "403"; then
    pass "MODE on non-existent channel → error 403 (ERR_NOSUCHCHANNEL)"
else
    fail "MODE non-existent channel → should error 403" "$out"
fi

# 1.3 MODE by non-operator
out=$(irc_cmd "PASS $PASS\r\nNICK m_guest\r\nUSER m 0 * :m\r\nJOIN #opchan\r\nMODE #opchan -o m_guest\r\nMODE #opchan +i")
if echo "$out" | grep -q "482"; then
    pass "MODE by non-operator → error 482 (ERR_CHANOPRIVSNEEDED)"
else
    fail "MODE by non-operator → should error 482" "$out"
fi

# 1.4 Case Sensitivity (Upper vs Lowercase flags)
out=$(irc_cmd "PASS $PASS\r\nNICK m_case\r\nUSER m 0 * :m\r\nJOIN #casechan\r\nMODE #casechan +K secret")
if ! echo "$out" | grep -q "+k"; then
    pass "MODE +K (Uppercase) → ignored safely without converting to +k"
else
    fail "MODE +K → server should not convert to +k" "$out"
fi

# ---------- TOPIC 2: MODE i & t (Invite-Only & Topic) ----------
section "TOPIC 2: MODE i & t (Invite-Only & Topic Restricted)"

# 2.1 Set +i
out=$(irc_cmd "PASS $PASS\r\nNICK m_i1\r\nUSER m 0 * :m\r\nJOIN #ichan\r\nMODE #ichan +i")
if echo "$out" | grep -q "MODE.*+i"; then
    pass "MODE +i → invite-only set and broadcasted"
else
    fail "MODE +i → failed to set/broadcast" "$out"
fi

# 2.2 Remove -i
out=$(irc_cmd "PASS $PASS\r\nNICK m_i2\r\nUSER m 0 * :m\r\nJOIN #ichan2\r\nMODE #ichan2 +i\r\nMODE #ichan2 -i")
if echo "$out" | grep -q "MODE.*-i"; then
    pass "MODE -i → invite-only removed and broadcasted"
else
    fail "MODE -i → failed to remove/broadcast" "$out"
fi

# 2.3 Set +t
out=$(irc_cmd "PASS $PASS\r\nNICK m_t1\r\nUSER m 0 * :m\r\nJOIN #tchan\r\nMODE #tchan +t")
if echo "$out" | grep -q "MODE.*+t"; then
    pass "MODE +t → topic restriction set and broadcasted"
else
    fail "MODE +t → failed to set/broadcast" "$out"
fi

# ---------- TOPIC 3: MODE k (Channel Key/Password) ----------
section "TOPIC 3: MODE k (Channel Key)"

# 3.1 Set +k with valid password
out=$(irc_cmd "PASS $PASS\r\nNICK m_k1\r\nUSER m 0 * :m\r\nJOIN #kchan\r\nMODE #kchan +k secretpass")
if echo "$out" | grep -q "MODE.*+k.*secretpass"; then
    pass "MODE +k secretpass → password set successfully"
else
    fail "MODE +k → failed to set password" "$out"
fi

# 3.2 Set +k without password (missing parameter)
out=$(irc_cmd "PASS $PASS\r\nNICK m_k2\r\nUSER m 0 * :m\r\nJOIN #kchan2\r\nMODE #kchan2 +k")
if echo "$out" | grep -q "461\|MODE.*+k"; then
    pass "MODE +k (no arg) → ignored or 461 (ERR_NEEDMOREPARAMS)"
else
    fail "MODE +k without arg → unexpected response" "$out"
fi

# 3.3 Remove -k
out=$(irc_cmd "PASS $PASS\r\nNICK m_k3\r\nUSER m 0 * :m\r\nJOIN #kchan3\r\nMODE #kchan3 +k pass\r\nMODE #kchan3 -k")
if echo "$out" | grep -q "MODE.*-k"; then
    pass "MODE -k → password removed successfully"
else
    fail "MODE -k → failed to remove password" "$out"
fi

# ---------- TOPIC 4: MODE l (User Limit) ----------
section "TOPIC 4: MODE l (User Limit)"

# 4.1 Set +l with valid integer
out=$(irc_cmd "PASS $PASS\r\nNICK m_l1\r\nUSER m 0 * :m\r\nJOIN #lchan\r\nMODE #lchan +l 5")
if echo "$out" | grep -q "MODE.*+l.*5"; then
    pass "MODE +l 5 → user limit set successfully"
else
    fail "MODE +l 5 → failed to set limit" "$out"
fi

# 4.2 Set +l with missing integer
out=$(irc_cmd "PASS $PASS\r\nNICK m_l2\r\nUSER m 0 * :m\r\nJOIN #lchan2\r\nMODE #lchan2 +l")
if echo "$out" | grep -q "461\|MODE.*+l"; then
    pass "MODE +l (no arg) → ignored or 461 (ERR_NEEDMOREPARAMS)"
else
    fail "MODE +l without arg → unexpected response" "$out"
fi

# 4.3 Set +l with invalid characters (letters)
out=$(irc_cmd "PASS $PASS\r\nNICK m_l3\r\nUSER m 0 * :m\r\nJOIN #lchan3\r\nMODE #lchan3 +l abc")
if ! echo "$out" | grep -q "MODE.*+l.*abc"; then
    pass "MODE +l abc → invalid limit rejected/ignored safely"
else
    fail "MODE +l abc → server wrongly accepted letters as a limit" "$out"
fi

# 4.4 Set +l with negative/zero number
out=$(irc_cmd "PASS $PASS\r\nNICK m_l4\r\nUSER m 0 * :m\r\nJOIN #lchan4\r\nMODE #lchan4 +l -5\r\nMODE #lchan4 +l 0")
if ! echo "$out" | grep -q "MODE.*+l.*-5"; then
    pass "MODE +l -5 or 0 → negative/zero limits rejected safely"
else
    fail "MODE +l negative → server wrongly accepted invalid numbers" "$out"
fi

# 4.5 Remove -l
out=$(irc_cmd "PASS $PASS\r\nNICK m_l5\r\nUSER m 0 * :m\r\nJOIN #lchan5\r\nMODE #lchan5 +l 10\r\nMODE #lchan5 -l")
if echo "$out" | grep -q "MODE.*-l"; then
    pass "MODE -l → limit removed successfully"
else
    fail "MODE -l → failed to remove limit" "$out"
fi

# ---------- TOPIC 5: MODE o (Operator Privileges) ----------
section "TOPIC 5: MODE o (Operator Privileges)"

# 5.1 Set +o missing target nickname
out=$(irc_cmd "PASS $PASS\r\nNICK m_o1\r\nUSER m 0 * :m\r\nJOIN #ochan1\r\nMODE #ochan1 +o")
if echo "$out" | grep -q "461\|MODE"; then
    pass "MODE +o (no arg) → ignored or 461 (ERR_NEEDMOREPARAMS)"
else
    fail "MODE +o without arg → unexpected response" "$out"
fi

# 5.2 Set +o for target not in channel
out=$(irc_cmd "PASS $PASS\r\nNICK m_o2\r\nUSER m 0 * :m\r\nJOIN #ochan2\r\nMODE #ochan2 +o ghostuser")
if echo "$out" | grep -q "441\|401"; then
    pass "MODE +o ghostuser → error 441 (ERR_USERNOTINCHANNEL) or 401"
else
    fail "MODE +o on ghost → should error 441/401" "$out"
fi

# 5.3 Set +o and -o on valid user (Simulation)
# Note: Full valid +o testing technically requires 2 connected sockets simultaneously
# We test syntax acceptance here.
out=$(irc_cmd "PASS $PASS\r\nNICK m_o3\r\nUSER m 0 * :m\r\nJOIN #ochan3\r\nMODE #ochan3 -o m_o3")
if echo "$out" | grep -q "MODE.*-o.*m_o3"; then
    pass "MODE -o valid_user → operator removal broadcasted"
else
    fail "MODE -o → failed to broadcast operator change" "$out"
fi

# ---------- TOPIC 6: COMBINED MODES ----------
section "TOPIC 6: COMBINED MODES"

# 6.1 Multiple valid flags together (+it)
out=$(irc_cmd "PASS $PASS\r\nNICK m_c1\r\nUSER m 0 * :m\r\nJOIN #cchan1\r\nMODE #cchan1 +it")
if echo "$out" | grep -q "MODE.*+it\|MODE.*+i.*+t"; then
    pass "MODE +it → combined flags parsed and broadcasted"
else
    fail "MODE +it → failed to parse combined flags" "$out"
fi

# 6.2 Opposite flags (+i -t)
out=$(irc_cmd "PASS $PASS\r\nNICK m_c2\r\nUSER m 0 * :m\r\nJOIN #cchan2\r\nMODE #cchan2 +i -t")
if echo "$out" | grep -q "MODE.*+i.*-t\|MODE.*+i.*MODE.*-t"; then
    pass "MODE +i -t → opposite combined flags parsed"
else
    fail "MODE +i -t → failed to parse mixed addition/removal" "$out"
fi

# ---------- RESULTS ----------
echo -e "\n${CYAN}================================${NC}"
echo -e "${GREEN}PASSOU:  $PASS_COUNT${NC}"
echo -e "${RED}FALHOU:  $FAIL_COUNT${NC}"
echo -e "${CYAN}TOTAL:   $((PASS_COUNT + FAIL_COUNT))${NC}"
echo -e "${CYAN}================================${NC}"