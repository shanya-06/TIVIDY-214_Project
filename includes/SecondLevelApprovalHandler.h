#ifndef SECONDLEVELAPPROVALHANDLER_H
#define SECONDLEVELAPPROVALHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

/**
 * @brief Handles the second level of work item approval.
 *
 * Evaluates an approval rule and performs the corresponding
 * action if the rule matches. Otherwise, forwards the
 * request to the next handler.
 */
class SecondLevelApprovalHandler : public ApprovalHandler {
public:
    /** @brief Function used to evaluate an approval request. */
    using ApprovalRule = std::function<bool(const WorkItem&)>;

    /** @brief Function executed when the approval rule matches. */
    using ApprovalAction = std::function<void(WorkItem&)>;

    /**
     * @brief Constructs a second-level approval handler.
     * @param rule Condition used to evaluate the work item.
     * @param action Action performed when the rule matches.
     * @throws std::invalid_argument If rule or action is missing.
     */
    SecondLevelApprovalHandler(ApprovalRule rule, ApprovalAction action);

    /**
     * @brief Processes an approval request.
     * @param item Work item requiring approval.
     * @throws std::runtime_error If the request reaches the
     * end of the chain without being handled.
     */
    void handleRequest(WorkItem& item) override;

private:
    /** @brief Rule used to evaluate approval requests. */
    ApprovalRule rule_;

    /** @brief Action performed when the rule matches. */
    ApprovalAction action_;
};

#endif
