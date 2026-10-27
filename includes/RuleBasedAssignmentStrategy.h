
#ifndef RULEBASEDASSIGNMENTSTRATEGY_H
#define RULEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant using configurable rules.
 *
 * This strategy allows different workflow scenarios
 * to apply assignment rules without changing the
 * generic workflow engine.
 */
class RuleBasedAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item
    ) override;
};

#endif
