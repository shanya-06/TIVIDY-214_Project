#ifndef APPROVALITEMCREATOR_H
#define APPROVALITEMCREATOR_H
#include "WorkItemCreator.h"

class ApprovalItemCreator : public WorkItemCreator {
public:
    explicit ApprovalItemCreator(WorkflowObserver* observer);
    WorkItem* createItem(const std::string& id, const std::string& name,
                         const std::string& requiredRole, bool needsApproval,
                         Mediator* mediator) override;
};
#endif