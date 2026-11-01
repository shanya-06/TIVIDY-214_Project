# Testing and CI

## Running the tests

The tests use [doctest](https://github.com/doctest/doctest), installed system-wide (Ubuntu: `sudo apt-get install doctest-dev`, macOS: `brew install doctest`, or use the Docker image).

```bash
make test                                  # build and run every tests/*Tests.cpp
./build/tests/RoutingRuleTests             # run one test program
./build/tests/StateTests -tc="*approval*"  # run matching test cases only
make clean && make test EXTRA="-g -fsanitize=address,undefined"   # with sanitizers
bash scripts/gui_smoke_test.sh             # start the GUI server and drive it through its API (after make)
```

Each `tests/*Tests.cpp` file is its own test program with its own doctest `main`. `make test` builds and runs all of them and stops at the first failure.

## Test files

| File | Level | Covers |
|---|---|---|
| `StateTests.cpp` | unit | State names, lifecycle, invalid transitions, approval, rejection, escalation |
| `StrategyTests.cpp` | unit | Role, workload and rule based assignment strategies and edge cases |
| `MediatorTests.cpp` | unit | Mediator assignment and reassignment |
| `ApprovalTests.cpp` | unit | Approval handlers and the chain (forwarding, cycles, unhandled requests) |
| `WorkflowInstanceTests.cpp` | unit | Instance independence, opening the next item, start twice, cancelling |
| `WorkflowIntegrationTests.cpp` | integration | Insurance and admissions flow through the Facade, two independent instances |
| `FacadeTests.cpp` | end to end | Bad input, instance keys, completion, failing one instance, cancelling, command history, manual escalate/assign, waiting for an available participant |
| `RoutingRuleTests.cpp` | end to end | Routing rules in all three industries, approval levels by amount, declined approvals, the payment Adapter |
| `AdHocWorkItemTests.cpp` | end to end | Adding work items to a running instance and the refusals |
| `CustomWorkflowTests.cpp` | end to end | Workflows designed in the GUI: running, routing, approval limits, automatic step failure, invalid designs, removal; JSON helper |

`tests/TestHelpers.h` has helpers for the end-to-end tests: `EngineFixture` (a Facade with the industries registered), `nextStep`, `runToEnd`, `params`, `itemIds` and `contains`.

## CI pipeline (GitHub Actions)

`.github/workflows/ci.yml` runs on every push and pull request to `main` and `dev`.

| Job | What it does |
|---|---|
| Build and test (g++, clang++) | Installs doctest, builds with `-Werror`, runs `make test`, the scripted demo and the GUI smoke test, and uploads JUnit test reports |
| Unit tests with sanitizers | Runs `make test` with AddressSanitizer and UndefinedBehaviorSanitizer |
| Docker image | Builds `build/Dockerfile`, runs `make test` and `make demo` in the container and checks that the GUI answers on port 8214 |
| Package (main only) | After all jobs pass, builds an optimised release and uploads `tividy-linux.tar.gz` |

Results are in the repository's **Actions** tab. To show a status badge in the README, add (replace `OWNER/REPO`):

```markdown
![CI](https://github.com/OWNER/REPO/actions/workflows/ci.yml/badge.svg)
```
