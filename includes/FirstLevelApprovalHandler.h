#ifndef FIRSTLEVELAPPROVALHANDLER_H
#define FIRSTLEVELAPPROVALHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class FirstLevelApprovalHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;
    FirstLevelApprovalHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
