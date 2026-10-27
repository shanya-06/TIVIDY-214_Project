
#ifndef RULEBASEDASSIGNMENTSTRATEGY_H
#define RULEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Assigns work using configurable eligibility rules.
 *
 * Checks participant role, availability, and
 * an optional maximum workload threshold.
 */
class RuleBasedAssignmentStrategy : public AssignmentStrategy {
private:
    int maxWorkload;

public:
    /**
     * @brief Creates a rule-based assignment strategy.
     * @param maxWorkload Maximum permitted participant workload.
     */
    explicit RuleBasedAssignmentStrategy(int maxWorkload);

    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
