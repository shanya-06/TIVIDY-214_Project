#ifndef INPROGRESSSTATE_H
#define INPROGRESSSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item currently being processed.
 *
 * Supports completion, approval, rejection, and escalation.
 * Tracks whether work requiring approval has been submitted.
 *
 * Concrete state in the State design pattern.
 */
class InProgressState : public WorkItemState {
private:
    /** @brief Indicates whether the work item is awaiting approval. */
    bool awaitingApproval;

public:
    /**
     * @brief Constructs an in-progress state.
     *
     * Initializes awaitingApproval to false.
     */
    InProgressState();

    /**
     * @brief Returns the current state name.
     * @return Name of the in-progress state.
     */
    std::string getName() const override;

    /**
     * @brief Checks whether escalation is supported.
     * @return True, since escalation is allowed in this state.
     */
    bool canEscalate() const override { return true; }

    /**
     * @brief Checks whether the work item is in progress.
     * @return True, since this is the in-progress state.
     */
    bool isInProgress() const override { return true; }

    /**
     * @brief Checks whether the work item is awaiting approval.
     * @return True if awaiting approval, otherwise false.
     */
    bool isAwaitingApproval() const override { return awaitingApproval; }

    /**
     * @brief Completes the work item or submits it for approval.
     *
     * If approval is required, marks the item as awaiting approval.
     * Otherwise, transitions the item to CompletedState.
     *
     * @param item Work item being completed.
     */
    void complete(WorkItem* item) override;

    /**
     * @brief Approves a work item awaiting approval.
     *
     * Transitions the work item to CompletedState
     * when approval is permitted.
     *
     * @param item Work item being approved.
     */
    void approve(WorkItem* item) override;

    /**
     * @brief Rejects the work item.
     *
     * Transitions the work item to RejectedState
     * when rejection is permitted.
     *
     * @param item Work item being rejected.
     * @param reason Reason for rejection.
     */
    void reject(
        WorkItem* item,
        const std::string& reason
    ) override;

    /**
     * @brief Escalates the work item.
     *
     * Transitions the work item to EscalatedState.
     *
     * @param item Work item being escalated.
     */
    void escalate(WorkItem* item) override;
};

#endif
