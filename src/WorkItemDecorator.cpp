#include "WorkItemDecorator.h"

#include <stdexcept>

// Lets us validate the pointer BEFORE the WorkItem base constructor dereferences it.
static WorkItem* checked(WorkItem* component) {
    if (!component) {
        throw std::invalid_argument("A decorator needs a work item to wrap");
    }
    return component;
}

WorkItemDecorator::WorkItemDecorator(WorkItem* component)
    : WorkItem(checked(component)->getId(), component->getName(), component->getRequiredRole(),
               component->requiresApproval(), component->getMediator()),
      component(component) {
    component->setOutermost(this);   // the Mediator will now talk to the decorator
}

WorkItemDecorator::~WorkItemDecorator() {
    delete component;
}

void WorkItemDecorator::setOutermost(WorkItem* item) { component->setOutermost(item); }

WorkItemProcessor* WorkItemDecorator::getProcessor() const { return component->getProcessor(); }

// Identity
std::string WorkItemDecorator::getId() const { return component->getId(); }
std::string WorkItemDecorator::getName() const { return component->getName(); }
std::string WorkItemDecorator::getRequiredRole() const { return component->getRequiredRole(); }
bool WorkItemDecorator::requiresApproval() const { return component->requiresApproval(); }

// Observer
void WorkItemDecorator::attach(WorkflowObserver* observer) { component->attach(observer); }
void WorkItemDecorator::detach(WorkflowObserver* observer) { component->detach(observer); }
void WorkItemDecorator::notifyObservers(const std::string& event) { component->notifyObservers(event); }

// Lifecycle
void WorkItemDecorator::makeAvailable() { component->makeAvailable(); }
void WorkItemDecorator::assign(WorkflowParticipant* participant) { component->assign(participant); }
WorkflowParticipant* WorkItemDecorator::getAssignee() const { return component->getAssignee(); }
void WorkItemDecorator::start() { component->start(); }
void WorkItemDecorator::complete() { component->complete(); }
void WorkItemDecorator::approve() { component->approve(); }
void WorkItemDecorator::reject(const std::string& reason) { component->reject(reason); }
void WorkItemDecorator::escalate() { component->escalate(); }

// State
void WorkItemDecorator::setState(WorkItemState* state) { component->setState(state); }
WorkItemState* WorkItemDecorator::getState() const { return component->getState(); }
std::string WorkItemDecorator::getStateName() const { return component->getStateName(); }
bool WorkItemDecorator::canBeAssigned() const { return component->canBeAssigned(); }
bool WorkItemDecorator::isCompleted() const { return component->isCompleted(); }
bool WorkItemDecorator::isAwaitingApproval() const { return component->isAwaitingApproval(); }
bool WorkItemDecorator::canBeEscalated() const { return component->canBeEscalated(); }
bool WorkItemDecorator::isRejected() const { return component->isRejected(); }
bool WorkItemDecorator::isInProgress() const { return component->isInProgress(); }
bool WorkItemDecorator::isCreated() const { return component->isCreated(); }
