#include "WorkItem.h"

#include <algorithm>
#include <stdexcept>

#include "WorkItemState.h"
#include "WorkflowParticipant.h"
#include "WorkflowObserver.h"

WorkItem::WorkItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                   bool needsApproval, Mediator* mediator)
    : WorkflowColleague(mediator),
      id(id),
      name(name),
      requiredRole(requiredRole),
      needsApproval(needsApproval),
      outermost(this),
      owned(false),
      processor(nullptr),
      state(nullptr),
      assignee(nullptr) {}

WorkItem::~WorkItem() {
    delete state;
}

std::string WorkItem::getId() const {
    return id;
}

std::string WorkItem::getName() const {
    return name;
}

std::string WorkItem::getRequiredRole() const {
    return requiredRole;
}

bool WorkItem::requiresApproval() const {
    return needsApproval;
}

void WorkItem::markOwned() {
    if (owned) {
        throw std::logic_error("Work item '" + id + "' already belongs to a workflow instance");
    }
    owned = true;
}

void WorkItem::setOutermost(WorkItem* item) {
    outermost = item;
}

WorkItem* WorkItem::getOutermost() const {
    return outermost;
}

void WorkItem::setProcessor(WorkItemProcessor* newProcessor) {
    processor = newProcessor;
}

WorkItemProcessor* WorkItem::getProcessor() const {
    return processor;
}

// Observer

void WorkItem::attach(WorkflowObserver* observer) {
    if (observer && std::find(observers.begin(), observers.end(), observer) == observers.end()) {
        observers.push_back(observer);
    }
}

void WorkItem::detach(WorkflowObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void WorkItem::notifyObservers(const std::string& event) {
    // Copy first: an observer may detach itself while being notified.
    std::vector<WorkflowObserver*> current(observers);
    for (WorkflowObserver* observer : current) {
        observer->update(this, event);
    }
}

// Lifecycle 

void WorkItem::makeAvailable() {
    requireState()->makeAvailable(this);
}

void WorkItem::assign(WorkflowParticipant* participant) {
    if (!participant) {
        throw std::invalid_argument("Cannot assign work item '" + id + "' to nobody");
    }
    // Set the assignee before the state changes so observers already see 
    // Roll back if the current state does not allow 
    WorkflowParticipant* previous = assignee;
    assignee = participant;
    try {
        requireState()->assign(this, participant);
        if (previous) {
            previous->decrementWorkload();
        }
        participant->incrementWorkload();
    } catch (...) {
        assignee = previous;
        throw;
    }
    changed();
}

WorkflowParticipant* WorkItem::getAssignee() const {
    return assignee;
}

void WorkItem::start() {
    requireState()->start(this);
}

void WorkItem::complete() {
    requireState()->complete(this);
}

void WorkItem::approve() {
    requireState()->approve(this);
}

void WorkItem::reject(const std::string& reason) {
    requireState()->reject(this, reason);
}

void WorkItem::escalate() {
    requireState()->escalate(this);
}

// State 

void WorkItem::setState(WorkItemState* newState) {
    if (newState == state) {
        return;
    }
    WorkItemState* old = state;
    state = newState;
    delete old;

    notifyObservers(getStateName());
    changed();
}

WorkItemState* WorkItem::getState() const {
    return state;
}

bool WorkItem::canBeAssigned() const {
    return state && state->canAssign();
}

bool WorkItem::canBeEscalated() const {
    return state && state->canEscalate();
}

bool WorkItem::isCreated() const {
    return state && state->isCreated();
}

bool WorkItem::isInProgress() const {
    return state && state->isInProgress();
}

bool WorkItem::isRejected() const {
    return state && state->isRejected();
}

bool WorkItem::isCompleted() const {
    return state && state->isCompleted();
}

bool WorkItem::isAwaitingApproval() const {
    return state && state->isAwaitingApproval();
}

std::string WorkItem::getStateName() const {
    return state ? state->getName() : "None";
}

WorkItemState* WorkItem::requireState() const {
    if (!state) {
        throw std::logic_error("Work item '" + id + "' has no state; call setState(new CreatedState()) first");
    }
    return state;
}
