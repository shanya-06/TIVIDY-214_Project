
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "FirstLevelApprovalHandler.h"
#include "WorkItem.h"
#include "CreatedState.h"
#include "WorkflowParticipant.h"

// Checks that an eligible approval handler approves a work item.
TEST_CASE("First level handler approves a work item")
{
    WorkItem item("W1", "Review Document", "Reviewer", true, nullptr);
    item.setState(new CreatedState());

    // Prepare the work item for approval without a Mediator.
    item.makeAvailable();

    WorkflowParticipant reviewer("P1", "John", "Reviewer", nullptr);
    item.assign(&reviewer);
    item.start();
    item.complete();

    // The rule accepts this item and the action approves it.
    FirstLevelApprovalHandler handler(
        [](const WorkItem& item) {
            return item.requiresApproval();
        },
        [](WorkItem& item) {
            item.approve();
        }
    );

    handler.handleRequest(item);

    CHECK(item.getStateName() == "Completed");
}


// Checks that an approval handler can reject a work item.
TEST_CASE("Approval handler rejects a work item")
{
    WorkItem item("W2", "Review Document", "Reviewer", true, nullptr);
    item.setState(new CreatedState());

    WorkflowParticipant reviewer("P2", "Sarah", "Reviewer", nullptr);

    item.makeAvailable();
    item.assign(&reviewer);
    item.start();
    item.complete();

    FirstLevelApprovalHandler handler(
        [](const WorkItem& item) {
            return item.requiresApproval();
        },
        [](WorkItem& item) {
            item.reject("Document does not meet requirements");
        }
    );

    handler.handleRequest(item);

    CHECK(item.getStateName() == "Rejected");
}


#include "SecondLevelApprovalHandler.h"

// Checks that an approval request reaches the next handler.
TEST_CASE("Approval request moves to the next handler")
{
    WorkItem item("W3", "Review Document", "Reviewer", true, nullptr);
    item.setState(new CreatedState());

    WorkflowParticipant reviewer("P3", "John", "Reviewer", nullptr);

    item.makeAvailable();
    item.assign(&reviewer);
    item.start();
    item.complete();

    // First handler cannot approve this request.
    FirstLevelApprovalHandler first(
        [](const WorkItem&) {
            return false;
        },
        [](WorkItem& item) {
            item.approve();
        }
    );

    // Second handler accepts and approves the request.
    SecondLevelApprovalHandler second(
        [](const WorkItem&) {
            return true;
        },
        [](WorkItem& item) {
            item.approve();
        }
    );

    first.setSuccessor(&second);
    first.handleRequest(item);

    CHECK(item.getStateName() == "Completed");
}


#include <stdexcept>

// Checks that invalid approval chains are rejected.
TEST_CASE("Approval handlers cannot form circular chains")
{
    FirstLevelApprovalHandler first(
        [](const WorkItem&) { return false; },
        [](WorkItem&) {}
    );

    SecondLevelApprovalHandler second(
        [](const WorkItem&) { return false; },
        [](WorkItem&) {}
    );

    // A handler cannot point to itself.
    CHECK_THROWS_AS(first.setSuccessor(&first), std::invalid_argument);

    // Create a valid link from first to second.
    first.setSuccessor(&second);

    // Linking second back to first would create a cycle.
    CHECK_THROWS_AS(second.setSuccessor(&first), std::invalid_argument);
}


TEST_CASE("Unhandled approval request throws an exception")
{
    WorkItem item("W4", "Review Document", "Reviewer", true, nullptr);
    item.setState(new CreatedState());

    FirstLevelApprovalHandler first(
        [](const WorkItem&) { return false; },
        [](WorkItem&) {}
    );

    SecondLevelApprovalHandler second(
        [](const WorkItem&) { return false; },
        [](WorkItem&) {}
    );

    first.setSuccessor(&second);

    // Neither handler accepts the request.
    CHECK_THROWS_AS(first.handleRequest(item), std::runtime_error);
}
