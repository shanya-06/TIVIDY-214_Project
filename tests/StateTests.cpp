
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "CreatedState.h"
#include "AvailableState.h"
#include "AssignedState.h"
#include "InProgressState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"
#include "Mediator.h"

#include <stdexcept>
#include <string>

class TestMediator : public Mediator {
public:
    int notifications = 0;

    void notify(WorkflowColleague* originator) override {
        (void)originator;
        ++notifications;
    }
};

TEST_CASE("State names are correct") {
    CreatedState created;
    AvailableState available;
    AssignedState assigned;
    InProgressState inProgress;
    CompletedState completed;
    RejectedState rejected;
    EscalatedState escalated;

    CHECK(created.getName() == "Created");
    CHECK(available.getName() == "Available");
    CHECK(assigned.getName() == "Assigned");
    CHECK(inProgress.getName() == "InProgress");
    CHECK(completed.getName() == "Completed");
    CHECK(rejected.getName() == "Rejected");
    CHECK(escalated.getName() == "Escalated");
}

TEST_CASE("Completed state rejects invalid operations") {
    CompletedState completed;

    CHECK_THROWS_AS(completed.start(nullptr), std::logic_error);
    CHECK_THROWS_AS(completed.complete(nullptr), std::logic_error);
    CHECK_THROWS_AS(completed.escalate(nullptr), std::logic_error);
}

TEST_CASE("Rejected state rejects invalid operations") {
    RejectedState rejected;

    CHECK_THROWS_AS(rejected.start(nullptr), std::logic_error);
    CHECK_THROWS_AS(rejected.complete(nullptr), std::logic_error);
    CHECK_THROWS_AS(rejected.approve(nullptr), std::logic_error);
}


TEST_CASE("WorkItem completes normal lifecycle") {
    TestMediator mediator;

    WorkItem item(
        "W001",
        "Review request",
        "Reviewer",
        false,
        &mediator
    );

    WorkflowParticipant participant(
        "P001",
        "Test Participant",
        "Reviewer",
        &mediator
    );

    item.setState(new CreatedState());
    CHECK(item.getStateName() == "Created");

    item.makeAvailable();
    CHECK(item.getStateName() == "Available");

    item.assign(&participant);
    CHECK(item.getAssignee() == &participant);
    CHECK(item.getStateName() == "Assigned");

    item.start();
    CHECK(item.getStateName() == "InProgress");

    item.complete();
    CHECK(item.getStateName() == "Completed");

    CHECK_THROWS_AS(item.start(), std::logic_error);

    CHECK(mediator.notifications >= 5);
}


TEST_CASE("WorkItem requiring approval") {
    TestMediator mediator;

    WorkItem item(
        "W002", "Approval request",
        "Reviewer", true, &mediator
    );

    WorkflowParticipant participant(
        "P002", "Test Reviewer",
        "Reviewer", &mediator
    );

    item.setState(new CreatedState());
    item.makeAvailable();
    item.assign(&participant);
    item.start();

    CHECK(item.getStateName() == "InProgress");

    // Cannot approve before the work is submitted.
    CHECK_THROWS_AS(item.approve(), std::logic_error);

    // Complete submits the item for approval.
    item.complete();
    CHECK(item.getStateName() == "InProgress");

    // Cannot submit twice.
    CHECK_THROWS_AS(item.complete(), std::logic_error);

    // Approval finishes the item.
    item.approve();
    CHECK(item.getStateName() == "Completed");

    // A completed item cannot be approved again.
    CHECK_THROWS_AS(item.approve(), std::logic_error);
}


TEST_CASE("WorkItem can be rejected") {
    TestMediator mediator;

    WorkItem item("W003", "Review request", "Reviewer", false, &mediator);
    WorkflowParticipant participant(
        "P003", "Reviewer A", "Reviewer", &mediator
    );

    item.setState(new CreatedState());
    item.makeAvailable();
    item.assign(&participant);
    item.start();

    item.reject("Missing information");

    CHECK(item.getStateName() == "Rejected");
    CHECK_THROWS_AS(item.complete(), std::logic_error);
    CHECK_THROWS_AS(item.start(), std::logic_error);
}

TEST_CASE("Escalated work item can be reassigned") {
    TestMediator mediator;

    WorkItem item("W004", "Review request", "Reviewer", false, &mediator);

    WorkflowParticipant first(
        "P004", "Reviewer A", "Reviewer", &mediator
    );

    WorkflowParticipant second(
        "P005", "Reviewer B", "Reviewer", &mediator
    );

    item.setState(new CreatedState());
    item.makeAvailable();
    item.assign(&first);
    item.start();

    item.escalate();
    CHECK(item.getStateName() == "Escalated");

    item.assign(&second);
    CHECK(item.getStateName() == "Assigned");
    CHECK(item.getAssignee() == &second);

    item.start();
    CHECK(item.getStateName() == "InProgress");

    item.complete();
    CHECK(item.getStateName() == "Completed");
}


TEST_CASE("Invalid assignments leave the work item unchanged") {
    TestMediator mediator;

    WorkItem item("W005", "Review request", "Reviewer", false, &mediator);

    WorkflowParticipant wrongRole(
        "P006", "Manager A", "Manager", &mediator
    );

    WorkflowParticipant unavailable(
        "P007", "Reviewer B", "Reviewer", &mediator
    );

    unavailable.setAvailable(false);

    item.setState(new CreatedState());
    item.makeAvailable();

    CHECK(item.getStateName() == "Available");
    CHECK(item.getAssignee() == nullptr);

    CHECK_THROWS_AS(
        item.assign(&wrongRole),
        std::logic_error
    );

    CHECK(item.getStateName() == "Available");
    CHECK(item.getAssignee() == nullptr);

    CHECK_THROWS_AS(
        item.assign(&unavailable),
        std::logic_error
    );

    CHECK(item.getStateName() == "Available");
    CHECK(item.getAssignee() == nullptr);

    CHECK_THROWS_AS(
        item.assign(nullptr),
        std::invalid_argument
    );

    CHECK(item.getStateName() == "Available");
    CHECK(item.getAssignee() == nullptr);
}


TEST_CASE("Assigned item can escalate and be reassigned") {
    TestMediator mediator;

    WorkItem item(
        "W006", "Review request", "Reviewer", false, &mediator
    );

    WorkflowParticipant first(
        "P008", "First Reviewer", "Reviewer", &mediator
    );

    WorkflowParticipant second(
        "P009", "Second Reviewer", "Reviewer", &mediator
    );

    item.setState(new CreatedState());
    item.makeAvailable();
    item.assign(&first);

    CHECK(item.getStateName() == "Assigned");
    CHECK(item.getAssignee() == &first);

    // Escalate before processing starts.
    item.escalate();
    CHECK(item.getStateName() == "Escalated");

    // Assign someone else.
    item.assign(&second);
    CHECK(item.getStateName() == "Assigned");
    CHECK(item.getAssignee() == &second);

    item.start();
    CHECK(item.getStateName() == "InProgress");

    item.complete();
    CHECK(item.getStateName() == "Completed");
}
