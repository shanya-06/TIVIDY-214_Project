//another creator
#ifndef TASKITEMCREATOR_H
#define TASKITEMCREATOR_H
#include "WorkItemCreator.h"

class TaskItemCreator : public WorkItemCreator {
public:
    explicit TaskItemCreator(WorkflowObserver* observer);
    WorkItem* createItem() override;
};
#endif