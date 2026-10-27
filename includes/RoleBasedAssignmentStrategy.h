
#ifndef ROLEBASEDASSIGNMENTSTRATEGY_H
#define ROLEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on their role.
 *
 * Finds an available participant whose role matches
 * the role required by the WorkItem.
 *
 * Works with any workflow scenario.
 */
class RoleBasedAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
