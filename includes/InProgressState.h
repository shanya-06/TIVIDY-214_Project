
#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item currently being processed.
 *
 * Supports completion, approval, rejection and escalation.
 * Tracks whether work requiring approval has been submitted.
 *
 */
class InProgressState : public WorkItemState {
private:
    bool awaitingApproval;

public:
    /**
     * @brief Creates an active work-item state.
     */
    InProgressState();

    /**
     * @brief Returns the current state name.
     */
    std::string getName() const override;

    /**
     * @brief Completes work or submits it for approval.
     */
    void complete(WorkItem* item) override;

    /**
     * @brief Approves previously submitted work.
     */
    void approve(WorkItem* item) override;

    /**
     * @brief Rejects the work item.
     */
    void reject(
        WorkItem* item,
        const std::string& reason
    ) override;

    /**
     * @brief Escalates the work item.
     */
    void escalate(WorkItem* item) override;
};

#endif
