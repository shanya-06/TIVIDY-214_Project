#include "TaskItemCreator.h"
#include "TaskItem.h"

TaskItemCreator::TaskItemCreator(WorkflowObserver* observer)
    : WorkItemCreator(observer) {}

WorkItem* TaskItemCreator::createItem(const std::string& id, const std::string& name,
                                   const std::string& requiredRole, bool needsApproval,
                                   Mediator* mediator) {
    return new TaskItem(id, name, requiredRole, needsApproval, mediator);
}