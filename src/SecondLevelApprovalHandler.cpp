#include "SecondLevelApprovalHandler.h"
#include <stdexcept>
#include <utility>

SecondLevelApprovalHandler::SecondLevelApprovalHandler(ApprovalRule rule, ApprovalAction action)
    : rule_(std::move(rule)), action_(std::move(action)) {
    if (!rule_ || !action_) {
        throw std::invalid_argument("Approval rule and action must both be provided");
    }
}

void SecondLevelApprovalHandler::handleRequest(WorkItem& item) {
    // If this approval level is eligible, execute its configured action.
    if (rule_(item)) {
        action_(item);
        return;
    }

    // Otherwise give the next approval level a chance.
    forwardRequest(item);
}
