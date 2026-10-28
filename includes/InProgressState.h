
#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item currently being processed.
 *
 * Supports completing, approving, rejecting,
 * and escalating work where permitted.
 *
 * Unsupported operations inherit the default
 * invalid-transition handling from WorkItemState.
 */
class InProgressState : public WorkItemState {
public:
    std::string getName() const override;

    void complete(WorkItem* item) override;

    void approve(WorkItem* item) override;

    void reject(
        WorkItem* item,
        const std::string& reason
    ) override;

    void escalate(WorkItem* item) override;
};

#endif
