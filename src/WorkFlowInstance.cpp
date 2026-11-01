#include "WorkFlowInstance.h"

#include <algorithm>
#include <stdexcept>

#include "Command.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"
#include "WorkflowParticipant.h"

WorkFlowInstance::WorkFlowInstance(const std::string& id)
    : id(id), status(CREATED), mediator(nullptr) {}

WorkFlowInstance::~WorkFlowInstance() {
    for (Command* command : history) {
        delete command;
    }
    for (WorkItem* item : workItems) {
        delete item;
    }
    delete mediator;
}

std::string WorkFlowInstance::getId() const {
    return id;
}

void WorkFlowInstance::addWorkItem(WorkItem* item) {
    if (!item) {
        throw std::invalid_argument("Cannot add an empty work item to instance '" + id + "'");
    }
    if (std::find(workItems.begin(), workItems.end(), item) != workItems.end()) {
        return;
    }
    item->markOwned();
    workItems.push_back(item);
    item->attach(this);
}

void WorkFlowInstance::addWorkItemAfter(WorkItem* after, WorkItem* item) {
    std::vector<WorkItem*>::iterator position = std::find(workItems.begin(), workItems.end(), after);
    if (!item || position == workItems.end()) {
        throw std::invalid_argument("Cannot insert a work item after an item that is not in instance '" + id + "'");
    }
    item->markOwned();
    workItems.insert(position + 1, item);
    item->attach(this);
}

void WorkFlowInstance::setRoutingRule(RoutingRule rule) {
    routingRule = rule;
}

const std::vector<std::string>& WorkFlowInstance::getEvents() const {
    return events;
}

std::vector<WorkItem*> WorkFlowInstance::getWorkItems() const {
    return workItems;
}

void WorkFlowInstance::setMediator(WorkflowMediator* newMediator) {
    if (newMediator == mediator) {
        return;
    }
    delete mediator;
    mediator = newMediator;
}

WorkflowMediator* WorkFlowInstance::getMediator() const {
    return mediator;
}

void WorkFlowInstance::executeCommand(Command* command) {
    if (!command) {
        throw std::invalid_argument("Cannot execute an empty command");
    }
    try {
        command->execute();
    } catch (...) {
        delete command; // a failed action is not part of the history
        throw;
    }
    history.push_back(command);
}

void WorkFlowInstance::cancel() {
    if (status != RUNNING) {
        throw std::logic_error("Instance '" + id + "' is not running, so it cannot be cancelled");
    }
    status = CANCELLED;
    events.push_back("instance " + id + ": cancelled");
    for (WorkItem* item : workItems) {
        if (item->getAssignee() && !item->isCompleted() && !item->isRejected()) {
            item->getAssignee()->decrementWorkload();
        }
    }
}

std::vector<Command*> WorkFlowInstance::getHistory() const {
    return history;
}

void WorkFlowInstance::start() {
    if (status != CREATED) {
        throw std::logic_error("Instance '" + id + "' has already been started");
    }
    status = RUNNING;

    // Items run in the order they were added: only the first one opens now.
    if (!workItems.empty()) {
        workItems.front()->makeAvailable();
    }
    updateStatus();
}

WFStatus WorkFlowInstance::getStatus() const {
    return status;
}

void WorkFlowInstance::update(WorkItem* item, const std::string& event) {
    // item may be the inner item of a decorator, so we never compare it with workItems
    events.push_back(item->getId() + " (" + item->getName() + "): " + event);
    // A state-change event carries the new state's name; other messages on a finished item must not re-route.
    const bool completed = (status == RUNNING && item->isCompleted() && event == item->getStateName());
    if (completed && routingRule) {
        routingRule(*this, item);   // the industry may add a step here
    }
    updateStatus();
    if (completed && status == RUNNING) {
        openNextItem();
    }
}

void WorkFlowInstance::openNextItem() {
    // Copy first: makeAvailable() changes state, which calls update() again.
    std::vector<WorkItem*> items(workItems);
    for (WorkItem* next : items) {
        if (next->isCreated()) {
            next->makeAvailable();
            return;
        }
    }
}

void WorkFlowInstance::updateStatus() {
    if (status != RUNNING || workItems.empty()) {
        return;
    }
    bool allCompleted = true;
    for (WorkItem* item : workItems) {
        if (item->isRejected()) {
            status = FAILED;
            return;
        }
        if (!item->isCompleted()) {
            allCompleted = false;
        }
    }
    if (allCompleted) {
        status = COMPLETED;
    }
}
