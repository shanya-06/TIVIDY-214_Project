#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include "TestHelpers.h"
#include "WorkItemSpec.h"

using namespace testhelpers;

// Adding work items to a running instance (WorkflowFacade::insertWorkItem).

namespace {
WorkItemSpec task(const std::string& id, const std::string& name, const std::string& role)
{
    WorkItemSpec spec;
    spec.type = "task";
    spec.id = id;
    spec.name = name;
    spec.role = role;
    return spec;
}
}

TEST_CASE_FIXTURE(EngineFixture, "An added work item runs straight after the current work")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    WorkItemSpec spec = task("X1", "Fraud check", "assessor");
    spec.priority = 3;
    facade.insertWorkItem(key, "T1", spec);

    std::vector<std::string> ids = itemIds(facade, key);
    REQUIRE(ids.size() == 6);
    CHECK(ids[1] == "X1");
    CHECK(facade.item(key, "X1").name == "[P3] Fraud check");   // PriorityDecorator applied

    nextStep(facade, key);
    CHECK(facade.availableWork(key).front().id == "X1");
    runToEnd(facade, key);
    CHECK(facade.instance(key).status == "COMPLETED");
}

TEST_CASE_FIXTURE(EngineFixture, "Invalid work items are refused")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    nextStep(facade, key);   // T1 done, T2 is the current work

    CHECK_THROWS_AS(facade.insertWorkItem(key, "T1", task("X1", "Late step", "assessor")), std::logic_error);
    CHECK_THROWS_AS(facade.insertWorkItem(key, "T2", task("X1", "Fly plane", "pilot")), std::logic_error);
    CHECK_THROWS_AS(facade.insertWorkItem(key, "T2", task("T3", "Duplicate", "assessor")), std::invalid_argument);

    WorkItemSpec meeting = task("X2", "Meeting", "assessor");
    meeting.type = "meeting";
    CHECK_THROWS_AS(facade.insertWorkItem(key, "T2", meeting), std::invalid_argument);

    CHECK(facade.instance(key).total == 5);   // nothing was added
}

TEST_CASE_FIXTURE(EngineFixture, "Work items cannot be added to a finished instance")
{
    std::string key = facade.startWorkflow("INS-CLAIM");
    runToEnd(facade, key);
    CHECK_THROWS_AS(facade.insertWorkItem(key, "S1", task("X1", "Extra", "admin")), std::logic_error);
}
