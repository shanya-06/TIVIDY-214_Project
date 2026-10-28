
#ifndef WORKITEMSTATE_H
#define WORKITEMSTATE_H

#include <string>

class WorkItem;
class WorkflowParticipant;

/**
 * @brief Abstract State class for a WorkItem.
 *
 * Defines the actions that a WorkItem can perform.
 * Concrete states will decide which actions are allowed.
 */
class WorkItemState {
public:
    virtual ~WorkItemState() = default;

    virtual void assign(WorkItem* item,
                        WorkflowParticipant* participant) = 0;

    virtual void start(WorkItem* item) = 0;

    virtual void complete(WorkItem* item,
                          const std::string& result) = 0;

    virtual void reject(WorkItem* item,
                        const std::string& reason) = 0;

    virtual void escalate(WorkItem* item) = 0;
};

#endif
