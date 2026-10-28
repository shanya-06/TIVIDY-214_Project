
#include "InProgressState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "WorkItem.h"

#include <stdexcept>

/**
 * @brief Returns the current state's name.
 */
std::string InProgressState::getName() const {
    return "InProgress";
}

/**
 * @brief Completes work that does not require approval.
 */
void InProgressState::complete(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (item->requiresApproval()) {
        throw std::logic_error(
            "Work item requires approval before completion"
        );
    }

    item->setState(new CompletedState());
}

/**
 * @brief Approval integration is not yet implemented.
 */
void InProgressState::approve(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    throw std::logic_error(
        "Approval handling is not yet implemented"
    );
}

/**
 * @brief Rejects a work item.
 */
void InProgressState::reject(
    WorkItem* item,
    const std::string& reason
) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (reason.empty()) {
        throw std::invalid_argument(
            "Rejection reason cannot be empty"
        );
    }

    item->setState(new RejectedState());
}

/**
 * @brief Escalates a work item for further handling.
 */
void InProgressState::escalate(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    item->setState(new EscalatedState());
}
