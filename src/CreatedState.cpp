#include "CreatedState.h"
#include "AvailableState.h"
#include "WorkItem.h"

#include <stdexcept>

/**
 * @brief Returns the name of this lifecycle state.
 */
std::string CreatedState::getName() const {
    return "Created";
}

/**
 * @brief Changes a newly created work item to AvailableState.
 */
void CreatedState::makeAvailable(WorkItem* item) {
    if (item == nullptr) {
        throw std::invalid_argument("WorkItem cannot be null");
    }

    item->setState(new AvailableState());
}
