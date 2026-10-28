
#include "AvailableState.h"
#include "AssignedState.h"
#include "WorkItem.h"
#include "WorkflowParticipant.h"

#include <stdexcept>

/**
 * @brief Returns the name of the current state.
 */
std::string AvailableState::getName() const {
    return "Available";
}

/**
 * @brief Validates the participant and transitions
 * the work item from Available to Assigned.
 */
void AvailableState::assign(
    WorkItem* item,
    WorkflowParticipant* participant
) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (participant == nullptr) {
        throw std::invalid_argument("Participant cannot be null");
    }

    if (!participant->getAvailable()) {
        throw std::logic_error("Participant is not available");
    }

    if (participant->getRole() != item->getRequiredRole()) {
        throw std::logic_error("Participant does not have the required role");
    }

    item->setState(new AssignedState());
}
