#ifndef WORKITEMDECORATOR_H
#define WORKITEMDECORATOR_H

#include "WorkItem.h"

/**
 * @brief Base decorator: IS a WorkItem and HAS a WorkItem.
 *
 * Every virtual operation of WorkItem is forwarded to the wrapped component.
 * Concrete decorators override only the operations they want to add behaviour to,
 * and call WorkItemDecorator::operation() (or component->operation()) to do the real work.
 *
 * Owns the wrapped component: deleting the outermost decorator deletes the whole chain.
 * The decorator's own inherited WorkItem state stays empty; the real state lives in the component.
 */
class WorkItemDecorator : public WorkItem {
public:
    explicit WorkItemDecorator(WorkItem* component);
    virtual ~WorkItemDecorator();

    // Identity
    virtual std::string getId() const override;
    virtual std::string getName() const override;
    virtual std::string getRequiredRole() const override;
    virtual bool requiresApproval() const override;

    // Observer
    virtual void attach(WorkflowObserver* observer) override;
    virtual void detach(WorkflowObserver* observer) override;
    virtual void notifyObservers(const std::string& event) override;

    // Lifecycle
    virtual void makeAvailable() override;
    virtual void assign(WorkflowParticipant* participant) override;
    virtual WorkflowParticipant* getAssignee() const override;
    virtual void start() override;
    virtual void complete() override;
    virtual void approve() override;
    virtual void reject(const std::string& reason) override;
    virtual void escalate() override;

    // State
    virtual void setState(WorkItemState* state) override;
    virtual WorkItemState* getState() const override;
    virtual std::string getStateName() const override;

protected:
    WorkItem* component;
};

#endif
