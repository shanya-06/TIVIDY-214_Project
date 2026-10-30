//one of the product as well
#ifndef APPROVALITEM_H
#define APPROVALITEM_H
#include "WorkItem.h"

class ApprovalItem : public WorkItem {
public:
    ApprovalItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                 bool needsApproval, Mediator* mediator = nullptr);
};
#endif