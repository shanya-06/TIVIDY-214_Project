#ifndef APPROVALITEMCREATOR_H
#define APPROVALITEMCREATOR_H
#include "WorkItemCreator.h"

class ApprovalItemCreator : public WorkItemCreator {
public:
    explicit ApprovalItemCreator(WorkflowObserver* observer);
    WorkItem* createItem() override;
};
#endif