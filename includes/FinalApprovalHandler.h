#ifndef FINALAPPROVALHANDLER_H
#define FINALAPPROVALHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class FinalApprovalHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;

    FinalApprovalHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
