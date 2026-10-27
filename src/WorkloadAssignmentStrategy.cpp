
#include "WorkloadAssignmentStrategy.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"

/**
 * @brief Selects the available participant with the
 * lowest workload who matches the required role.
 */
WorkflowParticipant* WorkloadAssignmentStrategy::selectParticipant(
    WorkItem* item,
    const std::vector<WorkflowParticipant*>& participants
) {
    if (item == nullptr) {
        return nullptr;
    }

    const std::string requiredRole = item->getRequiredRole();

    WorkflowParticipant* selected = nullptr;

    for (WorkflowParticipant* participant : participants) {
        if (participant == nullptr) {
            continue;
        }

        if (!participant->getAvailable() ||
            participant->getRole() != requiredRole) {
            continue;
        }

        if (selected == nullptr ||
            participant->getWorkLoad() < selected->getWorkLoad()) {
            selected = participant;
        }
    }

    return selected;
}

