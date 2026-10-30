#include "ApprovalItemCreator.h"
#include "ApprovalItem.h"

ApprovalItemCreator::ApprovalItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* ApprovalItemCreator::createItem(const std::string& id, const std::string& name,
                                   const std::string& requiredRole, bool needsApproval,
                                   Mediator* mediator) {
    return new ApprovalItem(id, name, requiredRole, needsApproval, mediator);
}