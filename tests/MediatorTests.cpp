
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "WorkflowMediator.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"
#include "RoleBasedAssignmentStrategy.h"
#include "CreatedState.h"

// Checks that the Mediator assigns work to the correct participant.
TEST_CASE("Mediator assigns work to the correct participant")
{
    WorkflowMediator mediator;
    RoleBasedAssignmentStrategy strategy;

    WorkflowParticipant reviewer("P1", "John", "Reviewer", nullptr);
    WorkflowParticipant manager("P2", "Sarah", "Manager", nullptr);

    WorkItem item("W1", "Review Document", "Reviewer", false, nullptr);
    item.setState(new CreatedState());

    mediator.setAssignmentStrategy(&strategy);
    mediator.registerParticipant(&reviewer);
    mediator.registerParticipant(&manager);
    mediator.registerWorkItem(&item);

    // Making the item available triggers automatic assignment.
    item.makeAvailable();

    CHECK(item.getAssignee() == &reviewer);
    CHECK(item.getStateName() == "Assigned");
    CHECK(reviewer.getWorkLoad() == 1);
    CHECK(manager.getWorkLoad() == 0);
}


// Checks that work is not assigned to someone with the wrong role.
TEST_CASE("Mediator does not assign work to the wrong role")
{
    WorkflowMediator mediator;
    RoleBasedAssignmentStrategy strategy;

    WorkflowParticipant manager("P1", "Sarah", "Manager", nullptr);

    WorkItem item("W2", "Review Document", "Reviewer", false, nullptr);
    item.setState(new CreatedState());

    mediator.setAssignmentStrategy(&strategy);
    mediator.registerParticipant(&manager);
    mediator.registerWorkItem(&item);

    item.makeAvailable();

    CHECK(item.getAssignee() == nullptr);
    CHECK(item.getStateName() == "Available");
    CHECK(manager.getWorkLoad() == 0);
}


TEST_CASE("Mediator reassigns work when a participant becomes unavailable")
{
    WorkflowMediator mediator;
    RoleBasedAssignmentStrategy strategy;

    WorkflowParticipant john("P1", "John", "Reviewer", nullptr);
    WorkflowParticipant sarah("P2", "Sarah", "Reviewer", nullptr);

    WorkItem item("W3", "Review Document", "Reviewer", false, nullptr);
    item.setState(new CreatedState());

    mediator.setAssignmentStrategy(&strategy);
    mediator.registerParticipant(&john);
    mediator.registerParticipant(&sarah);
    mediator.registerWorkItem(&item);

    item.makeAvailable();

    // John receives the work first.
    REQUIRE(item.getAssignee() == &john);
    CHECK(john.getWorkLoad() == 1);

    // John becomes unavailable.
    john.setAvailable(false);

    // The Mediator should reassign the item to Sarah.
    CHECK(item.getAssignee() == &sarah);
    CHECK(item.getStateName() == "Assigned");
    CHECK(john.getWorkLoad() == 0);
    CHECK(sarah.getWorkLoad() == 1);
}
