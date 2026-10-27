#include "ApprovalHandler.h"
#include <stdexcept>

void ApprovalHandler::setSuccessor(ApprovalHandler* next) {
    if (next == this) {
        throw std::invalid_argument("A handler cannot be its own successor");
    }
    for (ApprovalHandler* current = next; current != nullptr;
         current = current->successor) {
        if (current == this) {
            throw std::invalid_argument("Approval handler chain cannot contain a cycle");
        }
    }
    successor = next;
}

void ApprovalHandler::forwardRequest(WorkItem& item) {
    if (successor != nullptr) {
        successor->handleRequest(item);
        return;
    }
    throw std::runtime_error("No approval handler accepted this work item");
}
