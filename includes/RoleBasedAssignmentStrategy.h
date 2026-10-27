
#ifndef ROLEBASEDASSIGNMENTSTRATEGY_H
#define ROLEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on a required role.
 *
 * This strategy is generic and can be used by
 * any workflow scenario.
 */
class RoleBasedAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item
    ) override;
};

#endif
