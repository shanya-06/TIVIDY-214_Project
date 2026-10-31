
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
