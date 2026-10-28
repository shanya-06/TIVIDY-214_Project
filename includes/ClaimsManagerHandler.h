#ifndef CLAIMSMANAGERHANDLER_H
#define CLAIMSMANAGERHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class ClaimsManagerHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;

    ClaimsManagerHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
