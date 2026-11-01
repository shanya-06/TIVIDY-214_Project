#include "RoleBasedAssignmentStrategy.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"

/**
 * @brief Selects the first available participant
 * whose role matches the work item's required role.
 */
WorkflowParticipant* RoleBasedAssignmentStrategy::selectParticipant(
    WorkItem* item,
    const std::vector<WorkflowParticipant*>& participants
) {
    if (item == nullptr) {
        return nullptr;
    }

    const std::string requiredRole = item->getRequiredRole();

    for (WorkflowParticipant* participant : participants) {
        if (participant == nullptr) {
            continue;
        }

        if (participant->getAvailable() &&
            participant->getRole() == requiredRole) {
            return participant;
        }
    }

    return nullptr;
}
