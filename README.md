# TIVIDY-214_Project

COS 214 Project - TIVIDY

## Repo structure

- Dev/ working branch
  - includes/ for .h files
  - src/ for .cpp files
  - build/ for Docker and similar files

## Team members:

Shanya Nair - u25061845
Nasiha Osman - u25110188
Mohammed Lutchka - u25588304
Yasar Rahman - u24596397
Tamima Alam Touhid - u24691284
Mueez Gani - u24983439

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