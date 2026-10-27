#include "NotificationDecorator.h"

#include <iostream>

#include "WorkflowParticipant.h"

NotificationDecorator::NotificationDecorator(WorkItem* component, const std::string& recipient)
    : WorkItemDecorator(component), recipient(recipient) {}

void NotificationDecorator::notify(const std::string& message) const {
    std::cout << "[Notify " << recipient << "] " << message << std::endl;
}

// Forward first, notify second: if the state throws, no notification is sent.
void NotificationDecorator::assign(WorkflowParticipant* participant) {
    WorkItemDecorator::assign(participant);
    notify("'" + getName() + "' assigned to " + participant->getName());
}

void NotificationDecorator::start() {
    WorkItemDecorator::start();
    notify("'" + getName() + "' started");
}

void NotificationDecorator::complete(const Map& result) {
    WorkItemDecorator::complete(result);
    notify("'" + getName() + "' completed");
}

void NotificationDecorator::reject(const std::string& reason) {
    WorkItemDecorator::reject(reason);
    notify("'" + getName() + "' rejected: " + reason);
}

void NotificationDecorator::escalate() {
    WorkItemDecorator::escalate();
    notify("'" + getName() + "' escalated");
}
