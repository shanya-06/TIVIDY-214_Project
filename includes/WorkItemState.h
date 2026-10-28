
#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

class WorkItem;
class WorkflowParticipant;

/**
 * @brief Abstract base class for WorkItem lifecycle states.
 *
 * Controls which actions are permitted depending on
 * the current state of a WorkItem.
 *
 * Concrete states implement valid transitions.
 * Invalid operations will throw std::logic_error.
 */
class WorkItemState {
public:
    virtual ~WorkItemState() = default;

    /**
     * @brief Returns the name of the current state.
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Makes a created work item available.
     */
    virtual void makeAvailable(WorkItem* item);

    /**
     * @brief Assigns a work item to a participant.
     */
    virtual void assign(
        WorkItem* item,
        WorkflowParticipant* participant
    );

    /**
     * @brief Starts processing a work item.
     */
    virtual void start(WorkItem* item);

    /**
     * @brief Completes a work item.
     */
    virtual void complete(WorkItem* item);

    /**
     * @brief Approves a work item.
     */
    virtual void approve(WorkItem* item);

    /**
     * @brief Rejects a work item.
     */
    virtual void reject(
        WorkItem* item,
        const std::string& reason
    );

    /**
     * @brief Escalates a work item.
     */
    virtual void escalate(WorkItem* item);
};

#endif
