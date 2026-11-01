#!/usr/bin/env bash
# Smoke test for the GUI server: starts ./tividy_gui, drives one workflow through
# the same JSON API the web page uses, then stops the server.
# Usage: bash scripts/gui_smoke_test.sh   (after "make tividy_gui")
set -euo pipefail

PORT="${PORT:-8765}"
BASE="http://127.0.0.1:${PORT}"
DATA="$(mktemp -d)/custom_workflows.json"

./tividy_gui --no-browser --port "$PORT" --data "$DATA" > gui_server.log 2>&1 &
SERVER=$!
trap 'kill "$SERVER" 2>/dev/null || true' EXIT

# wait for the server to accept connections
for i in $(seq 1 50); do
  curl -s "$BASE/api/overview" > /dev/null && break
  sleep 0.2
done

check() {   # check <description> <python expression on the JSON reply "d">
  local description="$1" expression="$2" reply
  reply="$(cat)"
  if python3 -c "import json,sys; d=json.loads(sys.argv[1]); sys.exit(0 if ($expression) else 1)" "$reply"; then
    echo "ok   - $description"
  else
    echo "FAIL - $description"; echo "  reply: $reply"; exit 1
  fi
}

post() { curl -s -X POST -H "Content-Type: application/json" -d "$2" "$BASE$1"; }

curl -s "$BASE/" | grep -q "TIVIDY" && echo "ok   - web page is served"
curl -s "$BASE/api/overview" | check "three built-in workflows" "len(d['definitions']) == 3"
post /api/instances '{"workflow":"INS-CLAIM","params":{"amount":"35000"}}' \
  | check "instance started" "d['ok'] and d['key'] == 'INS-CLAIM-1'"
post /api/instances/INS-CLAIM-1/complete '{"item":"T2"}' \
  | check "invalid move is refused" "not d['ok'] and 'Cannot complete' in d['report']['error']"
post /api/instances/INS-CLAIM-1/next '{}' | check "next step" "d['ok']"
post /api/instances/INS-CLAIM-1/next '{}' | check "routing adds T4" "[a['id'] for a in d['report']['added']] == ['T4']"
for i in 1 2 3 4 5; do post /api/instances/INS-CLAIM-1/next '{}' > /dev/null; done
curl -s "$BASE/api/instances/INS-CLAIM-1" | check "instance completed" "d['status'] == 'COMPLETED'"
post /api/workflows '{"name":"Smoke","participants":[{"name":"Ann","role":"clerk"}],"items":[{"name":"Check","role":"clerk"}]}' \
  | check "custom workflow saved" "d['ok'] and d['design']['id'] == 'SMOKE'"
post /api/instances '{"workflow":"SMOKE","params":{}}' | check "custom workflow runs" "d['ok']"

echo "GUI smoke test passed"
