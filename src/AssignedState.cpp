
#include "AssignedState.h"
#include "InProgressState.h"
#include "WorkItem.h"

#include <stdexcept>

/**
 * @brief Returns the name of this lifecycle state.
 */
std::string AssignedState::getName() const {
    return "Assigned";
}

/**
 * @brief Starts processing an assigned work item.
 */
void AssignedState::start(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    item->setState(new InProgressState());
}
