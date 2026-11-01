#include "SubprocessItemCreator.h"
#include "SubprocessItem.h"

SubprocessItemCreator::SubprocessItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* SubprocessItemCreator::createItem(const std::string& id, const std::string& name,
                                   const std::string& requiredRole, bool needsApproval,
                                   Mediator* mediator) {
    return new SubprocessItem(id, name, requiredRole, needsApproval, mediator);
}