#ifndef CLAIMSASSESSORHANDLER_H
#define CLAIMSASSESSORHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class ClaimsAssessorHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;
    ClaimsAssessorHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
