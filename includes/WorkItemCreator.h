#ifndef WORKITEMCREATOR_H
#define WORKITEMCREATOR_H
#include "WorkItem.h"
#include "WorkflowObserver.h"

class WorkItemCreator {
private:
    WorkflowObserver* auditObserver;

public:
    explicit WorkItemCreator(WorkflowObserver* observer);
    virtual ~WorkItemCreator() = default;
    virtual WorkItem* createItem() = 0;
    WorkItem* instantiate();
};
#endif

//this is the abstract creator and workItem will be the abstract product