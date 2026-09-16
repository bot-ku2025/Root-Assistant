#!/usr/bin/env bash
set -euo pipefail

REPO="${RA_REPO:-bot-ku2025/Root-Assistant}"
BRANCH="${RA_BRANCH:-main}"
INTERVAL="${RA_INTERVAL:-15}"
MAX_WAIT="${RA_MAX_WAIT:-1800}"

echo "========================================"
echo "ROOT-ASSISTANT AUTOMATION CONTROLLER"
echo "PHASE 5 — P0 AUTOMATION LOOP"
echo "========================================"
echo "REPO      : $REPO"
echo "BRANCH    : $BRANCH"
echo "INTERVAL  : ${INTERVAL}s"
echo "MAX WAIT  : ${MAX_WAIT}s"
echo ""

START_TS="$(date +%s)"
LAST_SHA=""

while true; do
    NOW="$(date +%s)"

    if (( NOW - START_TS >= MAX_WAIT )); then
        echo "STATUS: TIMEOUT"
        exit 2
    fi

    API="https://api.github.com/repos/${REPO}/actions/runs?branch=${BRANCH}&per_page=5"

    RUN_JSON="$(curl -fsSL \
        -H 'Accept: application/vnd.github+json' \
        "$API")"

    RUN="$(printf '%s' "$RUN_JSON" |
        python3 -c '
import json,sys
d=json.load(sys.stdin)
runs=d.get("workflow_runs",[])
for r in runs:
    if r.get("event")=="push":
        print(json.dumps(r))
        break
')"

    if [ -z "$RUN" ]; then
        echo "STATE: WAITING_FOR_GITHUB_ACTIONS"
        sleep "$INTERVAL"
        continue
    fi

    RUN_ID="$(printf '%s' "$RUN" | python3 -c 'import json,sys; print(json.load(sys.stdin)["id"])')"
    RUN_NO="$(printf '%s' "$RUN" | python3 -c 'import json,sys; print(json.load(sys.stdin)["run_number"])')"
    SHA="$(printf '%s' "$RUN" | python3 -c 'import json,sys; print(json.load(sys.stdin)["head_sha"])')"
    STATUS="$(printf '%s' "$RUN" | python3 -c 'import json,sys; print(json.load(sys.stdin)["status"])')"
    CONCLUSION="$(printf '%s' "$RUN" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("conclusion") or "")')"

    if [ "$SHA" != "$LAST_SHA" ]; then
        LAST_SHA="$SHA"
        echo "RUN       : #$RUN_NO"
        echo "COMMIT    : ${SHA:0:7}"
        echo "STATUS    : $STATUS"
        echo "CONCLUSION: ${CONCLUSION:-RUNNING}"
    fi

    STATE="$(curl -fsSL \
        -H 'Accept: application/vnd.github+json' \
        "https://raw.githubusercontent.com/${REPO}/${SHA}/.github/automation/state.json" \
        2>/dev/null || true)"

    if [ -n "$STATE" ]; then
        CHECKPOINT="$(printf '%s' "$STATE" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("checkpoint",""))' 2>/dev/null || true)"
        STATE_STATUS="$(printf '%s' "$STATE" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("status",""))' 2>/dev/null || true)"
        FAILURE_STAGE="$(printf '%s' "$STATE" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("failure_stage",""))' 2>/dev/null || true)"
        FAILURE_REASON="$(printf '%s' "$STATE" | python3 -c 'import json,sys; print(json.load(sys.stdin).get("failure_reason",""))' 2>/dev/null || true)"

        echo "CHECKPOINT: ${CHECKPOINT:-UNKNOWN}"
        echo "STATE     : ${STATE_STATUS:-UNKNOWN}"

        if [ -n "$FAILURE_STAGE" ]; then
            echo "FAILURE   : $FAILURE_STAGE"
        fi

        if [ -n "$FAILURE_REASON" ]; then
            echo "REASON    : $FAILURE_REASON"
        fi
    fi

    if [ "$STATUS" = "completed" ]; then
        case "$CONCLUSION" in
            success)
                echo ""
                echo "========================================"
                echo "AUTOMATION RESULT: PASS"
                echo "RUN: #$RUN_NO"
                echo "CHECKPOINT: ${CHECKPOINT:-UNKNOWN}"
                echo "========================================"
                exit 0
                ;;
            failure|cancelled|timed_out)
                echo ""
                echo "========================================"
                echo "AUTOMATION RESULT: FAIL"
                echo "RUN: #$RUN_NO"
                echo "CHECKPOINT: ${CHECKPOINT:-UNKNOWN}"
                echo "FAILURE_STAGE: ${FAILURE_STAGE:-UNKNOWN}"
                echo "FAILURE_REASON: ${FAILURE_REASON:-UNKNOWN}"
                echo "========================================"
                exit 1
                ;;
        esac
    fi

    sleep "$INTERVAL"
done
