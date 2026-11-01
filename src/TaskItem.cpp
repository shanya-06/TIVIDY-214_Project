#include "TaskItem.h"

TaskItem::TaskItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                    bool needsApproval, Mediator* mediator)
    : WorkItem(id, name, requiredRole, needsApproval, mediator) {}
