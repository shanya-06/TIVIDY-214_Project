#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "TestHelpers.h"

using namespace testhelpers;

// Industry routing rules, approval decisions and the external service, run end to end.

TEST_CASE_FIXTURE(EngineFixture, "Small claims skip the extra documents step")
{
    std::string key = facade.startWorkflow("INS-CLAIM", params("amount", "8000"));
    runToEnd(facade, key);
    CHECK_FALSE(contains(itemIds(facade, key), "T4"));
    CHECK(facade.instance(key).total == 5);
}

TEST_CASE_FIXTURE(EngineFixture, "Claims over R30000 get T4 straight after the assessment")
{
    std::string key = facade.startWorkflow("INS-CLAIM", params("amount", "35000"));
    nextStep(facade, key);   // T1
    CHECK_FALSE(contains(itemIds(facade, key), "T4"));
    nextStep(facade, key);   // T2: the routing rule fires

    std::vector<std::string> ids = itemIds(facade, key);
    REQUIRE(ids.size() == 6);
    CHECK(ids[2] == "T4");
    CHECK(facade.availableWork(key).front().id == "T4");
}

TEST_CASE_FIXTURE(EngineFixture, "Borderline applications get an interview")
{
    std::string borderline = facade.startWorkflow("EDU-APPLICATION", params("average", "66"));
    std::string strong = facade.startWorkflow("EDU-APPLICATION", params("average", "82"));
    runToEnd(facade, borderline);
    runToEnd(facade, strong);

    CHECK(contains(itemIds(facade, borderline), "I1"));
    CHECK_FALSE(contains(itemIds(facade, strong), "I1"));
    CHECK(facade.instance(borderline).status == "COMPLETED");
}

TEST_CASE_FIXTURE(EngineFixture, "Damaged goods add a replacement shipment")
{
    std::string key = facade.startWorkflow("LOG-EXCEPTION", params("value", "12000", "damaged", "yes"));
    runToEnd(facade, key);
    CHECK(contains(itemIds(facade, key), "R1"));
    CHECK(facade.instance(key).status == "COMPLETED");
}

TEST_CASE_FIXTURE(EngineFixture, "The claim amount decides which level approves")
{
    std::string small = facade.startWorkflow("INS-CLAIM", params("amount", "8000"));
    std::string medium = facade.startWorkflow("INS-CLAIM", params("amount", "35000"));
    runToEnd(facade, small);
    runToEnd(facade, medium);

    CHECK(contains(facade.events(small), "A1 (Approve claim): assessor level approves"));
    CHECK(contains(facade.events(medium), "A1 (Approve claim): senior assessor level approves"));
}

TEST_CASE_FIXTURE(EngineFixture, "Claims over the cover limit are declined")
{
    std::string key = facade.startWorkflow("INS-CLAIM", params("amount", "250000"));
    runToEnd(facade, key);
    CHECK(facade.instance(key).status == "FAILED");
    CHECK(facade.item(key, "A1").state == "Rejected");
    CHECK(facade.item(key, "T3").state == "Created");   // payment never opened
}

TEST_CASE_FIXTURE(EngineFixture, "A low average is declined by the committee")
{
    std::string key = facade.startWorkflow("EDU-APPLICATION", params("average", "41"));
    runToEnd(facade, key);
    CHECK(facade.instance(key).status == "FAILED");
    CHECK(facade.item(key, "D1").state == "Rejected");
}

TEST_CASE_FIXTURE(EngineFixture, "Payment goes through the external service adapter")
{
    std::string paid = facade.startWorkflow("INS-CLAIM", params("bank", "valid"));
    std::string unpaid = facade.startWorkflow("INS-CLAIM", params("bank", "missing"));
    runToEnd(facade, paid);
    runToEnd(facade, unpaid);

    CHECK(contains(facade.events(paid), "T3 (Process payment): External system replied: Request completed successfully"));
    CHECK(facade.instance(unpaid).status == "FAILED");
    CHECK(facade.item(unpaid, "T3").state == "Rejected");
}
