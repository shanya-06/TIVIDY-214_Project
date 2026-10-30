#ifndef TASKITEM_H
#define TASKITEM_H
#include "WorkItem.h"

class TaskItem : public WorkItem {
public:
    TaskItem(const std::string& id, const std::string& name, const std::string& requiredRole,
             bool needsApproval, Mediator* mediator = nullptr);
};
#endif

//this is one of the products