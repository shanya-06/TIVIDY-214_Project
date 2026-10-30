#ifndef SUBPROCESSITEMCREATOR_H
#define SUBPROCESSITEMCREATOR_H
#include "WorkItemCreator.h"

class SubprocessItemCreator : public WorkItemCreator {
public:
    explicit SubprocessItemCreator(WorkflowObserver* observer);
    WorkItem* createItem(const std::string& id, const std::string& name,
                         const std::string& requiredRole, bool needsApproval,
                         Mediator* mediator) override;
};
#endif