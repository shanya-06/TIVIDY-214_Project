
#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item assigned to a participant.
 *
 * An assigned work item can transition to InProgress
 * when processing begins.
 *
 * Unsupported operations are rejected by WorkItemState.
 */
class AssignedState : public WorkItemState {
public:
    /**
     * @brief Returns the name of this lifecycle state.
     */
    std::string getName() const override;

    /**
     * @brief Starts processing the assigned work item.
     *
     * @param item Work item whose processing will begin.
     */
    void start(WorkItem* item) override;
};

#endif
