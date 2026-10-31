
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "WorkflowFacade.h"
#include "Industries.h"

// Checks that the complete workflow routes work correctly.
TEST_CASE("Insurance workflow opens the next task")
{
    WorkflowFacade facade;
    facade.showLiveMessages(false);

    // Register the real industry workflows.
    registerIndustries(facade);

    // Start a new insurance claim.
    std::string key = facade.startWorkflow("INS-CLAIM");

    // The first task should be assigned automatically.
    CHECK(facade.item(key, "T1").state == "Assigned");
    CHECK(facade.item(key, "T2").state == "Created");

    // Process and complete the first task.
    facade.startWork(key, "T1");
    facade.completeWork(key, "T1");

    // The workflow should move to the next task.
    CHECK(facade.item(key, "T1").state == "Completed");
    CHECK(facade.item(key, "T2").state == "Assigned");
    CHECK(facade.instance(key).status == "RUNNING");
}


TEST_CASE("Two insurance workflows run independently")
{
    WorkflowFacade facade;
    facade.showLiveMessages(false);
    registerIndustries(facade);

    // Start two separate insurance claims.
    std::string first = facade.startWorkflow("INS-CLAIM");
    std::string second = facade.startWorkflow("INS-CLAIM");

    CHECK(first != second);

    // Progress only the first claim.
    facade.startWork(first, "T1");
    facade.completeWork(first, "T1");

    // The first claim moves forward.
    CHECK(facade.item(first, "T1").state == "Completed");
    CHECK(facade.item(first, "T2").state == "Assigned");

    // The second claim must remain unchanged.
    CHECK(facade.item(second, "T1").state == "Assigned");
    CHECK(facade.item(second, "T2").state == "Created");

    CHECK(facade.instance(first).status == "RUNNING");
    CHECK(facade.instance(second).status == "RUNNING");
}


TEST_CASE("University application routes to the next task")
{
    WorkflowFacade facade;
    facade.showLiveMessages(false);
    registerIndustries(facade);

    // Start the university application with its default average of 82%.
    std::string key = facade.startWorkflow("EDU-APPLICATION");

    // Only the first task should be assigned.
    CHECK(facade.item(key, "V1").state == "Assigned");
    CHECK(facade.item(key, "E1").state == "Created");

    // Complete the application validation.
    facade.startWork(key, "V1");
    facade.completeWork(key, "V1");

    // The eligibility assessment should become available.
    CHECK(facade.item(key, "V1").state == "Completed");
    CHECK(facade.item(key, "E1").state == "Assigned");

    // The decision task must still be waiting.
    CHECK(facade.item(key, "D1").state == "Created");
    CHECK(facade.instance(key).status == "RUNNING");
}
