#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <stdexcept>
#include <string>

#include "CustomWorkflow.h"
#include "Json.h"
#include "TestHelpers.h"

using namespace testhelpers;

// Workflows designed at run time (CustomWorkflowLibrary + ConfigurableBuilder) and the JSON helper.

namespace {
const char* HOSPITAL = R"({
  "name": "Hospital admission",
  "parameters": [{"name": "days", "default": "3"}, {"name": "medicalAid", "default": "MA-1"}],
  "participants": [{"name": "Nurse", "role": "nurse"}, {"name": "Doctor", "role": "doctor"}],
  "strategy": "workload", "processor": "custom",
  "approval": {"parameter": "days", "level1Max": 2, "level2Max": 10, "overLimit": "reject"},
  "items": [
    {"id": "S1", "type": "task", "name": "Triage", "role": "nurse"},
    {"id": "S2", "type": "task", "name": "Assessment", "role": "doctor"},
    {"id": "S3", "type": "approval", "name": "Approve admission", "role": "doctor"},
    {"id": "S4", "type": "task", "name": "Check medical aid", "role": "nurse", "automatic": "checkAid"}
  ],
  "routes": [{"after": "S2", "parameter": "days", "op": ">", "value": "5",
              "insert": {"id": "R1", "type": "task", "name": "Specialist", "role": "doctor"}}]
})";

struct CustomFixture : EngineFixture {
    CustomWorkflowLibrary library;
    CustomFixture() : library(facade, "") {}   // "" keeps designs in memory, nothing is written to disk
};
}

TEST_CASE_FIXTURE(CustomFixture, "A saved design becomes a workflow that runs on the engine")
{
    Json saved = library.add(Json::parse(HOSPITAL));
    CHECK(saved["id"].str() == "HOSPITAL-ADMISSION");
    CHECK(library.isCustom("HOSPITAL-ADMISSION"));
    CHECK(facade.workflows().size() == 4);

    std::string key = facade.startWorkflow("HOSPITAL-ADMISSION");
    runToEnd(facade, key);
    CHECK(facade.instance(key).status == "COMPLETED");
    CHECK_FALSE(contains(itemIds(facade, key), "R1"));
}

TEST_CASE_FIXTURE(CustomFixture, "A design's routing rule and approval limits are applied")
{
    library.add(Json::parse(HOSPITAL));

    std::string longStay = facade.startWorkflow("HOSPITAL-ADMISSION", params("days", "7"));
    runToEnd(facade, longStay);
    CHECK(contains(itemIds(facade, longStay), "R1"));
    CHECK(contains(facade.events(longStay), "S3 (Approve admission): second level approves"));

    std::string tooLong = facade.startWorkflow("HOSPITAL-ADMISSION", params("days", "30"));
    runToEnd(facade, tooLong);
    CHECK(facade.instance(tooLong).status == "FAILED");
}

TEST_CASE_FIXTURE(CustomFixture, "An automatic step with missing data fails the instance")
{
    library.add(Json::parse(HOSPITAL));
    std::string key = facade.startWorkflow("HOSPITAL-ADMISSION", params("medicalAid", ""));
    runToEnd(facade, key);
    CHECK(facade.instance(key).status == "FAILED");
    CHECK(facade.item(key, "S4").state == "Rejected");
}

TEST_CASE_FIXTURE(CustomFixture, "Invalid designs are refused with a message")
{
    CHECK_THROWS_WITH(library.add(Json::parse(R"({"name": ""})")), "Give the workflow a name");
    CHECK_THROWS_WITH(library.add(Json::parse(R"({"name": "X", "items": [{"name": "a", "role": "r"}]})")),
                      "Add at least one participant");
    CHECK_THROWS_WITH(library.add(Json::parse(R"({"name": "X", "participants": [{"name": "Ann", "role": "clerk"}],
                                                  "items": [{"name": "Check", "role": "pilot"}]})")),
                      "Nobody has the role 'pilot' needed by S1 'Check'");

    library.add(Json::parse(HOSPITAL));
    CHECK_THROWS_AS(library.add(Json::parse(HOSPITAL)), std::invalid_argument);   // same ID twice
    CHECK(facade.workflows().size() == 4);
}

TEST_CASE_FIXTURE(CustomFixture, "Custom workflows can be removed but built-in ones cannot")
{
    library.add(Json::parse(HOSPITAL));
    std::string running = facade.startWorkflow("HOSPITAL-ADMISSION");
    library.remove("HOSPITAL-ADMISSION");

    CHECK(facade.workflows().size() == 3);
    CHECK_THROWS(facade.startWorkflow("HOSPITAL-ADMISSION"));
    CHECK(facade.instance(running).status == "RUNNING");   // instances already running carry on
    CHECK_THROWS_AS(library.remove("INS-CLAIM"), std::invalid_argument);
}

TEST_CASE("JSON values survive a parse and write round trip")
{
    Json j = Json::parse(R"({"a": 1, "b": [true, null, "x\"y\n"], "c": {"d": 2.5}})");
    CHECK(j["a"].num() == 1);
    CHECK(j["b"].size() == 3);
    CHECK(j["b"][0].boolean());
    CHECK(j["b"][1].isNull());
    CHECK(j["b"][2].str() == "x\"y\n");
    CHECK(j["c"]["d"].num() == doctest::Approx(2.5));
    CHECK(j["missing"].str("fallback") == "fallback");
    CHECK(Json::parse(j.dump()).dump() == j.dump());
}

TEST_CASE("Invalid JSON is reported")
{
    CHECK_THROWS_AS(Json::parse("{bad"), std::invalid_argument);
    CHECK_THROWS_AS(Json::parse("[1, 2"), std::invalid_argument);
    CHECK_THROWS_AS(Json::parse("1 2"), std::invalid_argument);
}
