# Root-Assistant Automation Bridge

## P0.1 — Automation Bridge Contract

This directory defines the machine-readable contract used by the existing GitHub Actions + Telegram checkpoint infrastructure.

### State model

- `PENDING` — checkpoint created, not executed.
- `RUNNING` — execution in progress.
- `PASS` — all required validation gates passed.
- `FAIL` — required gate failed; progression stops.
- `BLOCKED` — required external condition unavailable.

### Required fields

- `schema_version`
- `phase`
- `checkpoint`
- `step`
- `status`
- `commit_sha`
- `run_number`
- `timestamp`
- `next_action`
- `failure_stage`
- `failure_reason`

### Progression rule

`PASS` may advance to the next checkpoint. `FAIL` and `BLOCKED` stop automatic progression.

Telegram remains the durable human-visible checkpoint.
GitHub Actions remains the execution authority.

### Scope

P0.1 only establishes the automation bridge contract.
Existing module files, engine code, build structure, and Telegram notification mechanism remain unchanged.
