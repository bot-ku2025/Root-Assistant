# Root-Assistant Automation Controller

## P0 Automation Loop

The controller monitors the GitHub Actions push workflow and reads the
machine-readable `.github/automation/state.json` checkpoint.

State flow:

PENDING → RUNNING → PASS
                 ↘ FAIL
                 ↘ BLOCKED

Telegram remains the durable human checkpoint.

The controller does not modify engine code, existing build infrastructure,
or established repository filenames.

Usage:

./.github/automation/controller.sh
