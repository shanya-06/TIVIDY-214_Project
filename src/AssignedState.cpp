
#include "AssignedState.h"
#include "InProgressState.h"
#include "EscalatedState.h"
#include "WorkItem.h"

#include <stdexcept>

/** @brief Returns the state name. */
std::string AssignedState::getName() const {
    return "Assigned";
}

/** @brief Starts an assigned work item. */
void AssignedState::start(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    if (item->getAssignee() == nullptr) {
        throw std::logic_error(
            "Work item has no assigned participant"
        );
    }

    item->setState(new InProgressState());
}

/** @brief Escalates an assigned item for reassignment. */
void AssignedState::escalate(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    item->setState(new EscalatedState());
}
