
#ifndef ASSIGNEDSTATE_H
#define ASSIGNEDSTATE_H

#include "WorkItemState.h"

/** @brief Work item assigned to a participant. */
class AssignedState : public WorkItemState {
public:
    /** @brief Returns the state name. */
    std::string getName() const override;

    /** @brief Starts the assigned work item. */
    void start(WorkItem* item) override;

    /** @brief Escalates the item for reassignment. */
    void escalate(WorkItem* item) override;
};

#endif
