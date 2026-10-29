#ifndef SUBPROCESSITEMCREATOR_H
#define SUBPROCESSITEMCREATOR_H
#include "WorkItemCreator.h"

class SubprocessItemCreator : public WorkItemCreator {
public:
    explicit SubprocessItemCreator(WorkflowObserver* observer);
    WorkItem* createItem() override;
};
#endif