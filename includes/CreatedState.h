
#ifndef CREATEDSTATE_H
#define CREATEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a newly created work item.
 *
 * A work item in this state can become available
 * for assignment.
 *
 * All other lifecycle operations are invalid.
 */
class CreatedState : public WorkItemState {
public:
    std::string getName() const override;

    /**
     * @brief Transitions the work item to AvailableState.
     * @param item Work item whose state will change.
     */
    void makeAvailable(WorkItem* item) override;
};

#endif
