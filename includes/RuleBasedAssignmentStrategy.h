
#ifndef RULEBASEDASSIGNMENTSTRATEGY_H
#define RULEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant using configurable rules.
 *
 * Applies generic assignment rules to choose a
 * suitable participant for a work item.
 *
 * Works with different workflow scenarios without
 * requiring industry-specific code.
 */
class RuleBasedAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
