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
    virtual WorkItem* createItem(const std::string& id, const std::string& name,
                                 const std::string& requiredRole, bool needsApproval,
                                 Mediator* mediator) = 0;
    WorkItem* instantiate(const std::string& id, const std::string& name,
                          const std::string& requiredRole, bool needsApproval,
                          Mediator* mediator = nullptr);
};
#endif

//this is the abstract creator and workItem will be the abstract product