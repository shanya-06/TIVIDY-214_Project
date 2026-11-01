#include "DeadlineDecorator.h"

DeadlineDecorator::DeadlineDecorator(WorkItem* component, std::time_t deadline)
    : WorkItemDecorator(component), deadline(deadline), completedLate(false) {}

std::time_t DeadlineDecorator::getDeadline() const { return deadline; }

bool DeadlineDecorator::isOverdue() const {
    return std::time(nullptr) > deadline && !isCompleted();
}

bool DeadlineDecorator::wasCompletedLate() const { return completedLate; }

void DeadlineDecorator::complete() {
    const bool late = std::time(nullptr) > deadline;
    WorkItemDecorator::complete(); // throws if the state does not allow it
    completedLate = late;
}
