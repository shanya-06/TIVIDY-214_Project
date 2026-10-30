
#include "InProgressState.h"
#include "CompletedState.h"
#include "RejectedState.h"
#include "EscalatedState.h"
#include "WorkItem.h"

#include <stdexcept>

InProgressState::InProgressState()
    : awaitingApproval(false) {
}

/** @brief Returns the state name. */
std::string InProgressState::getName() const {
    return "InProgress";
}

/** @brief Completes the item or sends it for approval. */
void InProgressState::complete(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (awaitingApproval) {
        throw std::logic_error("Work item is already awaiting approval");
    }

    if (item->requiresApproval()) {
        awaitingApproval = true;
        item->notifyObservers("Work item awaiting approval");
        item->changed(); // lets the Mediator start the approval chain
        return;
    }

    item->setState(new CompletedState());
}

/** @brief Approves an item that has been submitted. */
void InProgressState::approve(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (!awaitingApproval) {
        throw std::logic_error("Work item is not awaiting approval");
    }

    item->setState(new CompletedState());
}

/** @brief Rejects the item with a reason. */
void InProgressState::reject(
    WorkItem* item,
    const std::string& reason
) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (reason.empty()) {
        throw std::invalid_argument("Rejection reason cannot be empty");
    }

    item->notifyObservers("Work item rejected: " + reason);
    item->setState(new RejectedState());
}

/** @brief Moves the item to Escalated. */
void InProgressState::escalate(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    item->setState(new EscalatedState());
}
