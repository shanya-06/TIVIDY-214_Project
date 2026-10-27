#include "WorkFlowInstance.h"

#include <algorithm>
#include <stdexcept>

#include "Command.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"

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
    workItems.push_back(item);
    item->attach(this);
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
    (void)item; // may be the inner item of a decorator, so we never compare it with workItems
    updateStatus();
    if (status == RUNNING && event == "Completed") {
        openNextItem();
    }
}

void WorkFlowInstance::openNextItem() {
    // Copy first: makeAvailable() changes state, which calls update() again.
    std::vector<WorkItem*> items(workItems);
    for (WorkItem* next : items) {
        if (next->getStateName() == "Created") {
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
        const std::string state = item->getStateName();
        if (state == "Rejected") {
            status = FAILED;
            return;
        }
        if (state != "Completed") {
            allCompleted = false;
        }
    }
    if (allCompleted) {
        status = COMPLETED;
    }
}
