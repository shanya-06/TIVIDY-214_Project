
#ifndef COMPLETEDSTATE_H
#define COMPLETEDSTATE_H

#include "WorkItemState.h"

/**
 * @brief Represents a successfully completed work item.
 *
 * Completed is a terminal state.
 * No further lifecycle operations are permitted.
 *
 * All invalid operations are handled by WorkItemState.
 */
class CompletedState : public WorkItemState {
public:
    /**
     * @brief Returns the name of this state.
     * @return "Completed".
     */
    std::string getName() const override;
    bool isCompleted() const override { return true; }
};

#endif
