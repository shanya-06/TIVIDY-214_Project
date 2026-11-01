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
 * @brief Represents a work item inside a workflow instance.
 *
 * - State context: forwards lifecycle operations to the current WorkItemState.
 * - Observer subject: notifies attached observers of state changes and events.
 * - Mediator colleague: calls changed() to notify the mediator of changes.
 * - Decorator component: provides virtual operations for decorators to override.
 *
 * Owns its current state but does not own its mediator, processor,
 * assignee, observers, or outermost decorator.
 *
 * A newly constructed work item has no state. The creator or builder
 * must initialize it using setState(new CreatedState()).
 */
class WorkItem : public WorkflowColleague {
public:
    /**
     * @brief Constructs a work item without an initial state.
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Whether the work item requires approval.
     * @param mediator Mediator coordinating the work item.
     */
    WorkItem(const std::string& id, const std::string& name,
             const std::string& requiredRole,
             bool needsApproval, Mediator* mediator);

    /**
     * @brief Destroys the work item and deletes its current state.
     */
    virtual ~WorkItem();

    /** @brief Prevents copying a work item. */
    WorkItem(const WorkItem&) = delete;

    /** @brief Prevents copy assignment of a work item. */
    WorkItem& operator=(const WorkItem&) = delete;

    /**
     * @brief Gets the unique identifier of the work item.
     * @return Work item identifier.
     */
    virtual std::string getId() const;

    /**
     * @brief Gets the name of the work item.
     * @return Work item name.
     */
    virtual std::string getName() const;

    /**
     * @brief Gets the role required to perform the work item.
     * @return Required role.
     */
    virtual std::string getRequiredRole() const;

    /**
     * @brief Checks whether the work item requires approval.
     * @return True if approval is required, otherwise false.
     */
    virtual bool requiresApproval() const;

    /**
     * @brief Sets the outermost decorator wrapping this work item.
     *
     * The mediator can use the outermost item so that decorators
     * participate in operations performed on the work item.
     *
     * @param item Pointer to the outermost work item.
     * @note The pointer is not owned.
     */
    virtual void setOutermost(WorkItem* item);

    /**
     * @brief Gets the outermost decorator or the item itself.
     * @return Pointer to the outermost work item.
     */
    WorkItem* getOutermost() const;

    /**
     * @brief Sets the processor associated with this work item.
     *
     * If no processor is set, the mediator may use its own processor.
     *
     * @param newProcessor Pointer to the processor.
     * @note The processor is not owned.
     */
    void setProcessor(WorkItemProcessor* newProcessor);

    /**
     * @brief Gets the processor associated with this work item.
     * @return Processor pointer, or nullptr if none is assigned.
     */
    virtual WorkItemProcessor* getProcessor() const;

    /**
     * @brief Marks the work item as belonging to a workflow instance.
     * @throws std::logic_error If the item is already owned.
     */
    void markOwned();

    /**
     * @brief Attaches an observer to the work item.
     *
     * Null observers and duplicate registrations are ignored.
     *
     * @param observer Observer to attach.
     */
    virtual void attach(WorkflowObserver* observer);

    /**
     * @brief Removes an observer from the work item.
     * @param observer Observer to remove.
     */
    virtual void detach(WorkflowObserver* observer);

    /**
     * @brief Notifies attached observers of an event.
     *
     * Copies the observer list before notifying so observers
     * can detach themselves during notification.
     *
     * @param event Description of the event.
     */
    virtual void notifyObservers(const std::string& event);

    /**
     * @brief Requests that the current state make the item available.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void makeAvailable();

    /**
     * @brief Assigns the work item to a participant.
     *
     * Delegates assignment to the current state and updates
     * participant workloads after a successful assignment.
     *
     * Restores the previous assignee if assignment fails.
     *
     * @param participant Participant receiving the work item.
     * @throws std::invalid_argument If participant is nullptr.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void assign(WorkflowParticipant* participant);

    /**
     * @brief Gets the participant assigned to the work item.
     * @return Assigned participant, or nullptr if unassigned.
     */
    virtual WorkflowParticipant* getAssignee() const;

    /**
     * @brief Requests that the current state start the work item.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void start();

    /**
     * @brief Requests completion of the work item.
     *
     * The current state determines whether the item completes
     * immediately or must first await approval.
     *
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void complete();

    /**
     * @brief Requests approval of the work item.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void approve();

    /**
     * @brief Requests rejection of the work item.
     * @param reason Reason for rejection.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void reject(const std::string& reason);

    /**
     * @brief Requests escalation of the work item.
     * @throws std::logic_error If no state has been initialized.
     */
    virtual void escalate();

    /**
     * @brief Replaces the current state with a new state.
     *
     * Deletes the previous state, notifies attached observers,
     * and informs the mediator of the change.
     *
     * Does nothing if the new state pointer equals the current one.
     *
     * @param state Pointer to the new state.
     * @note Takes ownership of the new state.
     */
    virtual void setState(WorkItemState* state);

    /**
     * @brief Gets the current state of the work item.
     * @return Current state pointer, or nullptr if not initialized.
     * @note The returned pointer is owned by the work item.
     */
    virtual WorkItemState* getState() const;

    /**
     * @brief Gets the name of the current state.
     * @return State name, or "None" if no state exists.
     */
    virtual std::string getStateName() const;

    /**
     * @brief Checks whether the current state allows assignment.
     * @return True if assignment is allowed, otherwise false.
     */
    virtual bool canBeAssigned() const;

    /**
     * @brief Checks whether the current state allows escalation.
     * @return True if escalation is allowed, otherwise false.
     */
    virtual bool canBeEscalated() const;

    /**
     * @brief Checks whether the work item is in the Created state.
     * @return True if the item is created, otherwise false.
     */
    virtual bool isCreated() const;

    /**
     * @brief Checks whether the work item is in progress.
     * @return True if the current state reports in progress.
     */
    virtual bool isInProgress() const;

    /**
     * @brief Checks whether the work item is rejected.
     * @return True if the current state reports rejected.
     */
    virtual bool isRejected() const;

    /**
     * @brief Checks whether the work item is completed.
     * @return True if the current state reports completed.
     */
    virtual bool isCompleted() const;

    /**
     * @brief Checks whether the work item is awaiting approval.
     * @return True if the current state reports awaiting approval.
     */
    virtual bool isAwaitingApproval() const;

private:
    /**
     * @brief Gets the current state, requiring it to be initialized.
     * @return Pointer to the current state.
     * @throws std::logic_error If no state has been initialized.
     */
    WorkItemState* requireState() const;

    /** @brief Unique identifier of the work item. */
    std::string id;

    /** @brief Name of the work item. */
    std::string name;

    /** @brief Role required to perform the work item. */
    std::string requiredRole;

    /** @brief Indicates whether approval is required. */
    bool needsApproval;

    /** @brief Pointer to the outermost decorator or this item. */
    WorkItem* outermost;

    /** @brief Indicates whether the item belongs to a workflow instance. */
    bool owned;

    /** @brief Non-owning pointer to the assigned processor. */
    WorkItemProcessor* processor;

    /** @brief Owned pointer to the current work item state. */
    WorkItemState* state;

    /** @brief Non-owning pointer to the assigned participant. */
    WorkflowParticipant* assignee;

    /** @brief Non-owning pointers to registered observers. */
    std::vector<WorkflowObserver*> observers;
};

#endif
