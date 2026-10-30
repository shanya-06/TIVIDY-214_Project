#ifndef SUBPROCESSITEM_H
#define SUBPROCESSITEM_H
#include "WorkItem.h"

class SubprocessItem : public WorkItem {
public:
    SubprocessItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                   bool needsApproval, Mediator* mediator = nullptr);
};
#endif

//also a product