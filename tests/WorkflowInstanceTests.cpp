
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "WorkFlowInstance.h"
#include "WorkItem.h"
#include "CreatedState.h"

// Checks that two workflow instances operate independently.
TEST_CASE("Workflow instances operate independently")
{
    WorkFlowInstance workflowA("A");
    WorkFlowInstance workflowB("B");

    WorkItem* itemA = new WorkItem("W1", "Task A", "Reviewer", false, nullptr);
    WorkItem* itemB = new WorkItem("W2", "Task B", "Reviewer", false, nullptr);

    itemA->setState(new CreatedState());
    itemB->setState(new CreatedState());

    // Each workflow owns its own work item.
    workflowA.addWorkItem(itemA);
    workflowB.addWorkItem(itemB);

    // Start only workflow A.
    workflowA.start();

    CHECK(workflowA.getStatus() == RUNNING);
    CHECK(workflowB.getStatus() == CREATED);

    CHECK(itemA->getStateName() == "Available");
    CHECK(itemB->getStateName() == "Created");

    // Workflow B must remain unchanged.
    CHECK(workflowA.getEvents().size() > 0);
    CHECK(workflowB.getEvents().empty());
}


#include "WorkflowParticipant.h"

// Checks that completing one task opens the next task.
TEST_CASE("Workflow opens the next work item after completion")
{
    WorkFlowInstance workflow("WF1");

    WorkflowParticipant reviewer("P1", "John", "Reviewer", nullptr);

    WorkItem* first = new WorkItem("W1", "Review Document", "Reviewer", false, nullptr);
    WorkItem* second = new WorkItem("W2", "Verify Document", "Reviewer", false, nullptr);

    first->setState(new CreatedState());
    second->setState(new CreatedState());

    workflow.addWorkItem(first);
    workflow.addWorkItem(second);

    // Starting the workflow opens only the first task.
    workflow.start();

    CHECK(first->getStateName() == "Available");
    CHECK(second->getStateName() == "Created");

    // Complete the first task.
    first->assign(&reviewer);
    first->start();
    first->complete();

    // The next task should open automatically.
    CHECK(first->getStateName() == "Completed");
    CHECK(second->getStateName() == "Available");
    CHECK(workflow.getStatus() == RUNNING);
}


#include <stdexcept>

// Checks that a workflow cannot be started twice.
TEST_CASE("Workflow cannot be started twice")
{
    WorkFlowInstance workflow("WF2");

    workflow.start();

    // Starting the same workflow again must throw an exception.
    CHECK_THROWS_AS(workflow.start(), std::logic_error);

    // The workflow must remain in its Running state.
    CHECK(workflow.getStatus() == RUNNING);
}


// Checks that a running workflow can be cancelled.
TEST_CASE("Running workflow can be cancelled")
{
    WorkFlowInstance workflow("WF3");

    WorkItem* item = new WorkItem("W3", "Review Document", "Reviewer", false, nullptr);
    item->setState(new CreatedState());

    workflow.addWorkItem(item);
    workflow.start();

    // Cancel the running workflow.
    workflow.cancel();

    CHECK(workflow.getStatus() == CANCELLED);

    // A cancelled workflow cannot be restarted.
    CHECK_THROWS_AS(workflow.start(), std::logic_error);

    // Cancelling it again is also invalid.
    CHECK_THROWS_AS(workflow.cancel(), std::logic_error);
}
