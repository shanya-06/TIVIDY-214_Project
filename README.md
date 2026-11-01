# TIVIDY-214_Project By The Seven

COS 214 Project - TIVIDY: Generic Workflow Management System

## PDF link
https://docs.google.com/document/d/16ex4TXWQJ5i_OFzQPBb63-5x0QIYT5eZro7JFrQ3npw/edit?usp=sharing

## Repo structure

TIVIDY-214_Project/
├── .github/
│   └── workflows/
│       └── ci.yml                      
├── app/                                
│   ├── gui/
│   │   ├── GuiApi.cpp / .h          
│   │   ├── HttpServer.cpp / .h        
│   │   └── gui_main.cpp              
│   ├── CustomWorkflow.cpp / .h
│   ├── Industries.cpp / .h
│   ├── IndustryBuilder.h
│   ├── Json.cpp / .h                   
│   └── main.cpp   

## Doxygen Instructions

- Run `doxygen Doxyfile`
- Open `doxygenDocs/html/index.html` to view the documentation

## Team members:

- Shanya Nair - u25061845
- Nasiha Osman - u25110188
- Mohammed Lutchka - u25588304
- Yasar Rahman - u24596397
- Tamima Alam Touhid - u24691284
- Mueez Gani - u24983439
- Tshegofatso Kungwane - u23605032

## Build and Run

The project uses C++11 and GNU Make.

**Requirements:**
- g++ compiler
- GNU Make
- doctest (for automated testing)

### Installing Dependencies (Ubuntu / WSL)

Before building or testing the project, install the required tools:

```bash
sudo apt update
sudo apt install build-essential doctest-dev
```

This installs the C++ compiler, GNU Make and the doctest testing framework.

To verify that doctest is installed:

```bash
ls /usr/include/doctest/doctest.h
```

If the file is displayed, doctest is installed correctly.

**Build the project:**

```bash
make
```

**Run the GUI:**

```bash
make gui
```

**Run the scripted demo:**

```bash
make demo
```


## Docker Setup

The project supports Docker using an Ubuntu environment with g++, GNU Make and doctest.

**Requirements:**
- Docker Desktop or Docker Engine

**Docker files:**
- `build/Dockerfile` - Installs dependencies and builds the application.
- `.dockerignore` - Excludes unnecessary files from the Docker build.

**Build the Docker image:**

```bash
docker build -f build/Dockerfile -t tividy214 .
```

**Run the GUI:**

```bash
docker run --rm -p 8214:8214 tividy214
```

Open http://localhost:8214 in your browser.

Press `Ctrl+C` to stop the container.

**Run the scripted demo:**

```bash
docker run --rm tividy214 make demo
```

**Run automated tests:**

```bash
docker run --rm tividy214 make test
```

**Docker test results (31 October 2026):**
- 33/33 tests passed
- 123/123 assertions passed
- 0 failures

The Docker image, scripted demo, automated tests and browser GUI have all been tested successfully.


## Automated Testing

We use the doctest framework to test important workflow functionality.

Our tests cover:

- **State:** State transitions and invalid operations.
- **Strategy:** Role-based, workload-based and rule-based assignments.
- **Mediator:** Assignment, reassignment and workload updates.
- **Approval:** Approval, rejection and handler chains.
- **WorkflowInstance:** Instance isolation, routing and cancellation.
- **Integration:** Workflow behaviour across insurance claims and university applications.

**Run all tests:**

```bash
make test
```

The Makefile automatically compiles and runs all six test suites.

**Test results (31 October 2026):**

- 33/33 tests passed
- 123/123 assertions passed
- 0 failures

All test source files are stored in the `tests/` folder. Compiled test executables are generated in `build/tests/`.

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
# TIVIDY GUI

## How it works

```
browser (gui/index.html, gui/app.js, gui/style.css)
   |  JSON requests to http://127.0.0.1:8214/api/...
   v
tividy_gui (C++11)
   app/gui/HttpServer    small socket server (no external libraries)
   app/gui/GuiApi        turns each request into WorkflowFacade calls
   app/CustomWorkflow    checks and saves workflows made on the "Create Workflow" page
   WorkflowFacade  --->  the workflow engine (includes/, src/)
```

The web page has no workflow logic. Every button sends a request to the C++ program, which runs it on the engine and returns the new state of the instance. If an action is not allowed, the engine refuses it and the error is shown in the message box.

## What you can do

**Home**
- Start an instance of any workflow. You can enter the input values yourself or pick a test case.
- See all instances with their status, and open one.
- See all workflow definitions, and delete custom ones.

**Instance page**
- Do Next Step: the assigned person starts and completes the current item.
- Start, Complete, Escalate or Reject (asks for a reason) any work item.
- Assign a work item to a participant.
- Set a participant on leave (their work is escalated and reassigned) or available again.
- Add a work item after the current work: type, role, priority and notify are options. The item is created by the factory registry and wrapped in the decorators.
- Cancel the instance.
- History: the commands executed and all events recorded by the instance.

**Create Workflow**
- Name, input fields, participants, work items in order (type, role, priority, notify, automatic external call).
- Rules: assignment strategy, processor, approval limits and routing rules.
- Check asks the engine to validate the design. Save registers it and stores it in `data/custom_workflows.json`, so it is loaded again next time.

## Demo script

1. **Insurance:** start *Insurance claim assessment* with the R35 000 test case.
   - Press Complete on T2: the engine refuses it.
   - Do Next Step, then set Ann on leave: T2 is reassigned to Ben.
   - Do Next Step: routing adds T4.
   - Add a work item "Fraud check" (assessor) after T4.
   - Keep pressing Do Next Step until COMPLETED.
   - For an exception path, use the R250 000 or missing-bank test case.
2. **Admissions:** start the 66% test case. The interview step I1 is added.
   - For an exception path, Start V1 and then Reject it. The instance becomes FAILED.
3. **Custom workflow:** on Create Workflow press Load Example, then Save Workflow.
   - Start it with days = 7: a specialist consult is added by routing.
   - Start it with medicalAid empty: the automatic call fails and the instance becomes FAILED.
