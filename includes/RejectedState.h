
#ifndef REJECTEDSTATE_H
#define REJECTEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a rejected work item.
 *
 * Rejected is a terminal state.
 * Further lifecycle operations are not permitted.
 *
 * Invalid operations are handled by WorkItemState.
 */
class RejectedState : public WorkItemState {
public:
    /**
     * @brief Returns the name of this lifecycle state.
     * @return "Rejected".
     */
    std::string getName() const override;
};

#endif
