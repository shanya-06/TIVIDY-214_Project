
#include "RuleBasedAssignmentStrategy.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"

#include <stdexcept>

/**
 * @brief Constructs a strategy with a maximum workload rule.
 */
RuleBasedAssignmentStrategy::RuleBasedAssignmentStrategy(int maxWorkload)
    : maxWorkload(maxWorkload) {

    if (maxWorkload < 0) {
        throw std::invalid_argument(
            "Maximum workload cannot be negative"
        );
    }
}

/**
 * @brief Finds the first participant satisfying the
 * configured assignment rules.
 */
WorkflowParticipant* RuleBasedAssignmentStrategy::selectParticipant(
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
            participant->getRole() == requiredRole &&
            participant->getWorkLoad() <= maxWorkload) {
            return participant;
        }
    }

    return nullptr;
}
