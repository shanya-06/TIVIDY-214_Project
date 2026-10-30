#ifndef WORKITEM_H
#define WORKITEM_H

#include <string>
#include <vector>

#include "WorkflowColleague.h"

class Mediator;
class WorkItemState;
class WorkflowObserver;
class WorkflowParticipant;
class WorkItemProcessor;

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

    /**
     * @brief The outermost decorator wrapping this item (the item itself when undecorated).
     *
     * The Mediator acts on this one, so decorators also see actions the Mediator performs.
     */
    virtual void setOutermost(WorkItem* item);
    WorkItem* getOutermost() const;

    /**
     * @brief Processor used for this item only (Template Method).
     *
     * Not owned. When none is set, the Mediator's own processor is used.
     */
    void setProcessor(WorkItemProcessor* newProcessor);
    virtual WorkItemProcessor* getProcessor() const;

    /** @brief Marks the item as belonging to a workflow instance. @throws std::logic_error if it already does. */
    void markOwned();

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
    /** @brief True when the current state allows assign() (asked by the Mediator before auto-assigning). */
    virtual bool canBeAssigned() const;
    /** @brief True when the current state allows escalate(). */
    virtual bool canBeEscalated() const;
    /** @brief True when the item has not been opened yet. */
    virtual bool isCreated() const;
    /** @brief True when work has started on the item (it may still be awaiting approval). */
    virtual bool isInProgress() const;
    /** @brief True when the current state is Rejected. */
    virtual bool isRejected() const;
    /** @brief True when the current state is Completed. */
    virtual bool isCompleted() const;
    /** @brief True when the item was completed but still needs an approval decision. */
    virtual bool isAwaitingApproval() const;

private:
    WorkItemState* requireState() const;

    std::string id;
    std::string name;
    std::string requiredRole;
    bool needsApproval;
    WorkItem* outermost;
    bool owned;
    WorkItemProcessor* processor;
    WorkItemState* state;
    WorkflowParticipant* assignee;
    std::vector<WorkflowObserver*> observers;
};

#endif
