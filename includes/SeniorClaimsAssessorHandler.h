#ifndef SENIORCLAIMSASSESSORHANDLER_H
#define SENIORCLAIMSASSESSORHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

class SeniorClaimsAssessorHandler : public ApprovalHandler {
public:
    using ApprovalRule = std::function<bool(const WorkItem&)>;
    using ApprovalAction = std::function<void(WorkItem&)>;

    SeniorClaimsAssessorHandler(ApprovalRule rule, ApprovalAction action);
    void handleRequest(WorkItem& item) override;

private:
    ApprovalRule rule_;
    ApprovalAction action_;
};

#endif
