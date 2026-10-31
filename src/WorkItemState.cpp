#include "WorkItemState.h"

#include <stdexcept>
#include <string>

/**
 * @brief Throws an error when an invalid transition is attempted.
 *
 * The error identifies the operation and current state.
 */
static void invalidTransition(
    const std::string& operation,
    const std::string& stateName
) {
    throw std::logic_error(
        "Cannot " + operation + " work item in state " + stateName
    );
}

void WorkItemState::makeAvailable(WorkItem* item) {
    (void)item;
    invalidTransition("make available", getName());
}

void WorkItemState::assign(
    WorkItem* item,
    WorkflowParticipant* participant
) {
    (void)item;
    (void)participant;
    invalidTransition("assign", getName());
}

void WorkItemState::start(WorkItem* item) {
    (void)item;
    invalidTransition("start", getName());
}

void WorkItemState::complete(WorkItem* item) {
    (void)item;
    invalidTransition("complete", getName());
}

void WorkItemState::approve(WorkItem* item) {
    (void)item;
    invalidTransition("approve", getName());
}

void WorkItemState::reject(
    WorkItem* item,
    const std::string& reason
) {
    (void)item;
    (void)reason;
    invalidTransition("reject", getName());
}

void WorkItemState::escalate(WorkItem* item) {
    (void)item;
    invalidTransition("escalate", getName());
}
