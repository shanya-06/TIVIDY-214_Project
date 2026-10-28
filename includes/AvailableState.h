
#ifndef AVAILABLESTATE_H
#define AVAILABLESTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a work item ready for assignment.
 *
 * A work item in this state can be assigned to
 * a suitable workflow participant.
 *
 * Unsupported operations are rejected by WorkItemState.
 */
class AvailableState : public WorkItemState {
public:
    /**
     * @brief Returns the name of this state.
     */
    std::string getName() const override;

    /**
     * @brief Assigns the work item to a participant.
     *
     * @param item Work item being assigned.
     * @param participant Participant receiving the work.
     */
    void assign(
        WorkItem* item,
        WorkflowParticipant* participant
    ) override;
};

#endif
