#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include "TestHelpers.h"

using namespace testhelpers;

// End-to-end tests through the WorkflowFacade, using the built-in industries.

TEST_CASE_FIXTURE(EngineFixture, "Facade lists the three built-in workflows")
{
    std::vector<WorkflowFacade::WorkflowInfo> list = facade.workflows();
    REQUIRE(list.size() == 3);
    CHECK(list[0].id == "INS-CLAIM");
    CHECK(list[1].id == "EDU-APPLICATION");
    CHECK(list[2].id == "LOG-EXCEPTION");
}

TEST_CASE_FIXTURE(EngineFixture, "Facade refuses bad input without starting an instance")
{
    CHECK_THROWS_AS(facade.startWorkflow("NOPE"), std::invalid_argument);
    CHECK_THROWS_AS(facade.startWorkflow("INS-CLAIM", params("colour", "red")), std::invalid_argument);
    CHECK_THROWS_WITH(facade.startWorkflow("INS-CLAIM", params("amount", "lots")),
                      "amount must be a whole number, e.g. 8000");
    CHECK(facade.instances().empty());

    // Unknown instances and work items are reported as well.
    std::string key = facade.startWorkflow("INS-CLAIM");
    CHECK_THROWS_AS(facade.instance("INS-CLAIM-99"), std::invalid_argument);
    CHECK_THROWS_AS(facade.startWork(key, "ZZ"), std::invalid_argument);
}

TEST_CASE_FIXTURE(EngineFixture, "Instance keys are numbered per workflow")
{
    CHECK(facade.startWorkflow("INS-CLAIM") == "INS-CLAIM-1");
    CHECK(facade.startWorkflow("INS-CLAIM") == "INS-CLAIM-2");
    CHECK(facade.startWorkflow("EDU-APPLICATION") == "EDU-APPLICATION-1");
}

TEST_CASE_FIXTURE(EngineFixture, "A small insurance claim runs to completion")
{
    std::string key = facade.startWorkflow("INS-CLAIM", params("amount", "8000"));
    runToEnd(facade, key);

    WorkflowFacade::InstanceInfo info = facade.instance(key);
    CHECK(info.status == "COMPLETED");
    CHECK(info.completed == info.total);
    CHECK(facade.availableWork(key).empty());
}

TEST_CASE_FIXTURE(EngineFixture, "Failing one application does not affect another")
{
    std::string a = facade.startWorkflow("EDU-APPLICATION", params("average", "82"));
    std::string b = facade.startWorkflow("EDU-APPLICATION", params("average", "82"));

    facade.startWork(b, "V1");
    facade.rejectWork(b, "V1", "documents missing");

    CHECK(facade.instance(b).status == "FAILED");
    CHECK(facade.instance(a).status == "RUNNING");
    runToEnd(facade, a);
    CHECK(facade.instance(a).status == "COMPLETED");
}

TEST_CASE_FIXTURE(EngineFixture, "A cancelled instance refuses actions and frees its participants")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    nextStep(facade, key);
    facade.cancel(key);

    CHECK(facade.instance(key).status == "CANCELLED");
    CHECK_THROWS_AS(facade.startWork(key, "T2"), std::logic_error);
    CHECK(facade.availableWork(key).empty());

    std::vector<WorkflowFacade::ParticipantInfo> people = facade.participants(key);
    for (std::size_t i = 0; i < people.size(); ++i) CHECK(people[i].workload == 0);

    // The audit trail names the instance, not just the workflow.
    CHECK(facade.events(key).back() == "instance INS-CLAIM-1: cancelled");
}

TEST_CASE_FIXTURE(EngineFixture, "Only successful commands are kept in the history")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    CHECK_THROWS(facade.completeWork(key, "T1"));   // still Assigned, so refused
    CHECK(facade.commands(key).empty());

    nextStep(facade, key);
    REQUIRE(facade.commands(key).size() == 1);
    CHECK(facade.commands(key)[0] == "Complete Work Item");
}

TEST_CASE_FIXTURE(EngineFixture, "Work can be escalated and reassigned by hand")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    nextStep(facade, key);

    CHECK_THROWS_AS(facade.assignWork(key, "T2", "u3"), std::logic_error);   // must escalate first
    facade.escalateWork(key, "T2");
    CHECK_THROWS_AS(facade.assignWork(key, "T2", "u1"), std::logic_error);   // admin is not an assessor
    facade.assignWork(key, "T2", "u3");

    CHECK(facade.item(key, "T2").assignee == "Ben (assessor)");
    CHECK(facade.commands(key).back() == "Assign Work Item");
}

TEST_CASE_FIXTURE(EngineFixture, "Work waits until someone with the role is available")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    facade.setAvailable(key, "u2", false);
    facade.setAvailable(key, "u3", false);
    nextStep(facade, key);

    CHECK(facade.item(key, "T2").state == "Available");
    CHECK(facade.item(key, "T2").assignee.empty());

    facade.setAvailable(key, "u3", true);
    CHECK(facade.item(key, "T2").assignee == "Ben (assessor)");
}
