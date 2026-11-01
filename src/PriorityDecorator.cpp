#include "PriorityDecorator.h"

PriorityDecorator::PriorityDecorator(WorkItem* component, int priority)
    : WorkItemDecorator(component), priority(priority) {}

int PriorityDecorator::getPriority() const { return priority; }

std::string PriorityDecorator::getName() const {
    return "[P" + std::to_string(priority) + "] " + WorkItemDecorator::getName();
}

void PriorityDecorator::escalate() {
    WorkItemDecorator::escalate(); // may throw; only bump priority if it succeeded
    ++priority;
}
