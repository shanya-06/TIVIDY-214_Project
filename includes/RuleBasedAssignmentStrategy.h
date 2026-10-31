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
    /** @brief Maximum workload allowed for a participant. */
    int maxWorkload;

public:
    /**
     * @brief Creates a rule-based assignment strategy.
     * @param maxWorkload Maximum permitted participant workload.
     */
    explicit RuleBasedAssignmentStrategy(int maxWorkload);

    /**
     * @brief Selects an eligible participant based on assignment rules.
     * @param item Work item requiring assignment.
     * @param participants List of available candidates.
     * @return Pointer to an eligible participant, or nullptr
     * if no participant satisfies the rules.
     */
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
