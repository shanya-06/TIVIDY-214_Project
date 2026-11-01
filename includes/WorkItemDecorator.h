#ifndef WORKITEMDECORATOR_H
#define WORKITEMDECORATOR_H
 
#include "WorkItem.h"
 
/**
 * @brief Base class for all work-item decorators (Decorator pattern).
 *
 * A decorator both IS a WorkItem 
 * and HAS a WorkItem (the component it wraps). Every operation is forwarded to the
 * wrapped component; concrete decorators override only the operations they extend.
 *
 * Ownership: owns the wrapped component and deletes it, so deleting the outermost
 * decorator deletes the whole chain.
 *
 * @note Observers and the mediator are notified by the innermost WorkItem with its own
 */
class WorkItemDecorator : public WorkItem {
public:
    /**
     * @brief Wraps a work item.
     * @param component The item to wrap. Ownership transfers to the decorator.
     * @throws std::invalid_argument if @p component is null.
     */
    explicit WorkItemDecorator(WorkItem* component);
 
    /** @brief Deletes the wrapped component (and so the rest of the chain). */
    virtual ~WorkItemDecorator();
 
    /** @name Identity (forwarded to the wrapped item) */
    ///@{
    /** @copydoc WorkItem::getId */
    virtual void setOutermost(WorkItem* item) override;
    virtual WorkItemProcessor* getProcessor() const override;
    virtual std::string getId() const override;
    /** @copydoc WorkItem::getName */
    virtual std::string getName() const override;
    /** @copydoc WorkItem::getRequiredRole */
    virtual std::string getRequiredRole() const override;
    /** @copydoc WorkItem::requiresApproval */
    virtual bool requiresApproval() const override;
    ///@}
 
    /** @name Observer (forwarded, so observers attach to the real item) */
    ///@{
    /** @copydoc WorkItem::attach */
    virtual void attach(WorkflowObserver* observer) override;
    /** @copydoc WorkItem::detach */
    virtual void detach(WorkflowObserver* observer) override;
    /** @copydoc WorkItem::notifyObservers */
    virtual void notifyObservers(const std::string& event) override;
    ///@}
 
    /** @name Lifecycle (forwarded to the wrapped item's current state) */
    ///@{
    /** @copydoc WorkItem::makeAvailable */
    virtual void makeAvailable() override;
    /** @copydoc WorkItem::assign */
    virtual void assign(WorkflowParticipant* participant) override;
    /** @copydoc WorkItem::getAssignee */
    virtual WorkflowParticipant* getAssignee() const override;
    /** @copydoc WorkItem::start */
    virtual void start() override;
    /** @copydoc WorkItem::complete */
    virtual void complete() override;
    /** @copydoc WorkItem::approve */
    virtual void approve() override;
    /** @copydoc WorkItem::reject */
    virtual void reject(const std::string& reason) override;
    /** @copydoc WorkItem::escalate */
    virtual void escalate() override;
    ///@}
 
    /** @name State (forwarded to the wrapped item) */
    ///@{
    /** @copydoc WorkItem::setState */
    virtual void setState(WorkItemState* state) override;
    /** @copydoc WorkItem::getState */
    virtual WorkItemState* getState() const override;
    /** @copydoc WorkItem::getStateName */
    virtual std::string getStateName() const override;
    virtual bool canBeAssigned() const override;
    virtual bool canBeEscalated() const override;
    virtual bool isCreated() const override;
    virtual bool isInProgress() const override;
    virtual bool isRejected() const override;
    virtual bool isCompleted() const override;
    virtual bool isAwaitingApproval() const override;
    ///@}
 
protected:
    /** @brief The wrapped item. Owned. Never null after construction. */
    WorkItem* component;
};
 
#endif
 
