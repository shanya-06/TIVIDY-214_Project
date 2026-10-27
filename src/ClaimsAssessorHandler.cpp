#include "ClaimsAssessorHandler.h"
#include <stdexcept>
#include <utility>

ClaimsAssessorHandler::ClaimsAssessorHandler(ApprovalRule rule, ApprovalAction action)
    : rule_(std::move(rule)), action_(std::move(action)) {
    if (!rule_ || !action_) {
        throw std::invalid_argument("Approval rule and action must both be provided");
    }
}

void ClaimsAssessorHandler::handleRequest(WorkItem& item) {
    if (rule_(item)) {
        action_(item);
        return;
    }


    forwardRequest(item);
}
