#ifndef SECONDLEVELAPPROVALHANDLER_H
#define SECONDLEVELAPPROVALHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class SecondLevelApprovalHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;

    SecondLevelApprovalHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
