
#ifndef ESCALATEDSTATE_H
#define ESCALATEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item requiring further handling.
 *
 * An escalated work item can be reassigned to an
 * eligible participant and return to AssignedState.
 *
 * Other unsupported operations are rejected.
 */
class EscalatedState : public WorkItemState {
public:
    /**
     * @brief Returns the name of this lifecycle state.
     */
    std::string getName() const override;
    bool canAssign() const override { return true; }

    /**
     * @brief Reassigns an escalated work item.
     *
     * @param item Work item being reassigned.
     * @param participant New participant receiving the work.
     */
    void assign(
        WorkItem* item,
        WorkflowParticipant* participant
    ) override;
};

#endif
