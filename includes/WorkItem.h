#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <vector>

#include "WorkflowColleague.h"

class Mediator;
class WorkItemState;
class WorkflowObserver;
class WorkflowParticipant;

/**
 * @brief One task inside a workflow instance.
 *
 * - State context: every lifecycle action is forwarded to the current WorkItemState.
 * - Observer subject: attached WorkflowObservers are told about every state change.
 * - Mediator colleague: calls changed() on every state change so the
 *   WorkflowMediator can decide what happens next.
 * - Decorator component: operations are virtual so WorkItemDecorator can wrap them.
 *
 * Owns its current state. Does not own its assignee or observers.
 *
 * A new item has no state; whoever creates it (the factory or builder)
 * must call setState(new CreatedState()) before using it.
 */
class WorkItem : public WorkflowColleague {
public:
    WorkItem(const std::string& id, const std::string& name, const std::string& requiredRole,
             bool needsApproval, Mediator* mediator);
    virtual ~WorkItem();

    WorkItem(const WorkItem&) = delete;
    WorkItem& operator=(const WorkItem&) = delete;

    virtual std::string getId() const;
    virtual std::string getName() const;
    virtual std::string getRequiredRole() const;
    virtual bool requiresApproval() const;

    // Observer: subject side
    virtual void attach(WorkflowObserver* observer);
    virtual void detach(WorkflowObserver* observer);
    virtual void notifyObservers(const std::string& event);

    // Lifecycle: each call is forwarded to the current state, which throws if the move is not allowed
    virtual void makeAvailable();
    virtual void assign(WorkflowParticipant* participant);
    virtual WorkflowParticipant* getAssignee() const;
    virtual void start();
    virtual void complete();
    virtual void approve();
    virtual void reject(const std::string& reason);
    virtual void escalate();

    // State
    /**
     * @brief Replaces (and deletes) the current state, then notifies observers and the mediator.
     *
     * Called by the concrete states. Takes ownership of state.
     */
    virtual void setState(WorkItemState* state);
    virtual WorkItemState* getState() const;
    /** @brief Name of the current state, or "None" before the first setState(). */
    virtual std::string getStateName() const;

private:
    WorkItemState* requireState() const;

    std::string id;
    std::string name;
    std::string requiredRole;
    bool needsApproval;
    WorkItemState* state;
    WorkflowParticipant* assignee;
    std::vector<WorkflowObserver*> observers;
};

#endif
