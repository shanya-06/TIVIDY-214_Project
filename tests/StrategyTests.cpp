
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "RoleBasedAssignmentStrategy.h"
#include "WorkloadAssignmentStrategy.h"
#include "RuleBasedAssignmentStrategy.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"

#include <stdexcept>
#include <vector>

TEST_CASE("RoleBased selects an available participant with the right role") {
    WorkItem item("W001", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant manager("P001", "Manager", "Manager", nullptr);
    WorkflowParticipant reviewer("P002", "Reviewer", "Reviewer", nullptr);

    std::vector<WorkflowParticipant*> participants = {
        &manager, &reviewer
    };

    RoleBasedAssignmentStrategy strategy;

    CHECK(strategy.selectParticipant(&item, participants) == &reviewer);
}

TEST_CASE("WorkloadBased selects the participant with the lowest workload") {
    WorkItem item("W002", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant first("P003", "First", "Reviewer", nullptr);
    WorkflowParticipant second("P004", "Second", "Reviewer", nullptr);

    first.incrementWorkload();
    first.incrementWorkload();
    second.incrementWorkload();

    std::vector<WorkflowParticipant*> participants = {
        &first, &second
    };

    WorkloadAssignmentStrategy strategy;

    CHECK(strategy.selectParticipant(&item, participants) == &second);
}

TEST_CASE("RuleBased applies the configured workload limit") {
    WorkItem item("W003", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant busy("P005", "Busy", "Reviewer", nullptr);
    WorkflowParticipant free("P006", "Free", "Reviewer", nullptr);

    busy.incrementWorkload();
    busy.incrementWorkload();
    busy.incrementWorkload();

    std::vector<WorkflowParticipant*> participants = {
        &busy, &free
    };

    RuleBasedAssignmentStrategy strategy(2);

    CHECK(strategy.selectParticipant(&item, participants) == &free);
}

TEST_CASE("Strategies return nullptr when no participant qualifies") {
    WorkItem item("W004", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant manager("P007", "Manager", "Manager", nullptr);

    std::vector<WorkflowParticipant*> participants = {
        nullptr, &manager
    };

    RoleBasedAssignmentStrategy roleBased;
    WorkloadAssignmentStrategy workloadBased;
    RuleBasedAssignmentStrategy ruleBased(3);

    CHECK(roleBased.selectParticipant(&item, participants) == nullptr);
    CHECK(workloadBased.selectParticipant(&item, participants) == nullptr);
    CHECK(ruleBased.selectParticipant(&item, participants) == nullptr);
}

TEST_CASE("Strategies handle null work items") {
    std::vector<WorkflowParticipant*> participants;

    RoleBasedAssignmentStrategy roleBased;
    WorkloadAssignmentStrategy workloadBased;
    RuleBasedAssignmentStrategy ruleBased(3);

    CHECK(roleBased.selectParticipant(nullptr, participants) == nullptr);
    CHECK(workloadBased.selectParticipant(nullptr, participants) == nullptr);
    CHECK(ruleBased.selectParticipant(nullptr, participants) == nullptr);
}

TEST_CASE("RuleBased rejects a negative workload limit") {
    CHECK_THROWS_AS(RuleBasedAssignmentStrategy(-1), std::invalid_argument);
}


TEST_CASE("Strategies skip unavailable participants") {
    WorkItem item("W005", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant unavailable("P008", "First", "Reviewer", nullptr);
    WorkflowParticipant available("P009", "Second", "Reviewer", nullptr);

    unavailable.setAvailable(false);

    std::vector<WorkflowParticipant*> participants = {
        &unavailable, &available
    };

    RoleBasedAssignmentStrategy roleBased;
    WorkloadAssignmentStrategy workloadBased;
    RuleBasedAssignmentStrategy ruleBased(3);

    CHECK(roleBased.selectParticipant(&item, participants) == &available);
    CHECK(workloadBased.selectParticipant(&item, participants) == &available);
    CHECK(ruleBased.selectParticipant(&item, participants) == &available);
}

TEST_CASE("WorkloadBased chooses first participant when workloads are equal") {
    WorkItem item("W006", "Review request", "Reviewer", false, nullptr);

    WorkflowParticipant first("P010", "First", "Reviewer", nullptr);
    WorkflowParticipant second("P011", "Second", "Reviewer", nullptr);

    std::vector<WorkflowParticipant*> participants = {
        &first, &second
    };

    WorkloadAssignmentStrategy strategy;

    CHECK(strategy.selectParticipant(&item, participants) == &first);
}

TEST_CASE("Strategies handle an empty participant list") {
    WorkItem item("W007", "Review request", "Reviewer", false, nullptr);

    std::vector<WorkflowParticipant*> participants;

    RoleBasedAssignmentStrategy roleBased;
    WorkloadAssignmentStrategy workloadBased;
    RuleBasedAssignmentStrategy ruleBased(3);

    CHECK(roleBased.selectParticipant(&item, participants) == nullptr);
    CHECK(workloadBased.selectParticipant(&item, participants) == nullptr);
    CHECK(ruleBased.selectParticipant(&item, participants) == nullptr);
}
