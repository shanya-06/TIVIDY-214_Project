#ifndef ROLEBASEDASSIGNMENTSTRATEGY_H
#define ROLEBASEDASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on their role.
 *
 * Finds the first available participant whose role
 * matches the role required by the WorkItem.
 *
 * Works with any workflow scenario.
 */
class RoleBasedAssignmentStrategy : public AssignmentStrategy {
public:
    /**
     * @brief Selects the first available participant with a matching role.
     * @param item Work item requiring assignment.
     * @param participants List of participants to choose from.
     * @return Pointer to the selected participant, or nullptr
     * if no eligible participant is found or item is null.
     */
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
