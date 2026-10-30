//another creator
#ifndef TASKITEMCREATOR_H
#define TASKITEMCREATOR_H
#include "WorkItemCreator.h"

class TaskItemCreator : public WorkItemCreator {
public:
    explicit TaskItemCreator(WorkflowObserver* observer);
    WorkItem* createItem(const std::string& id, const std::string& name,
                         const std::string& requiredRole, bool needsApproval,
                         Mediator* mediator) override;
};
#endif