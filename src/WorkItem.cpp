#include "WorkItem.h"

#include <algorithm>
#include <stdexcept>

#include "WorkItemState.h"
#include "WorkflowObserver.h"

WorkItem::WorkItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                   bool needsApproval, Mediator* mediator)
    : WorkflowColleague(mediator),
      id(id),
      name(name),
      requiredRole(requiredRole),
      needsApproval(needsApproval),
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
    } catch (...) {
        assignee = previous;
        throw;
    }
}

WorkflowParticipant* WorkItem::getAssignee() const {
    return assignee;
}

void WorkItem::start() {
    requireState()->start(this);
}

void WorkItem::complete(const Map& result) {
    requireState()->complete(this, result);
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

// Dependencies 

void WorkItem::addPrerequisite(WorkItem* item) {
    if (!item || item == this) {
        throw std::invalid_argument("Invalid prerequisite for work item '" + id + "'");
    }
    if (std::find(prerequisites.begin(), prerequisites.end(), item) != prerequisites.end()) {
        return;
    }
    prerequisites.push_back(item);
    item->dependents.push_back(this);
}

bool WorkItem::prerequisitesMet() const {
    for (WorkItem* item : prerequisites) {
        if (item->getStateName() != "Completed") {
            return false;
        }
    }
    return true;
}

std::vector<WorkItem*> WorkItem::getDependents() const {
    return dependents;
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

std::string WorkItem::getStateName() const {
    return state ? state->getName() : "None";
}

WorkItemState* WorkItem::requireState() const {
    if (!state) {
        throw std::logic_error("Work item '" + id + "' has no state; call setState(new CreatedState()) first");
    }
    return state;
}
