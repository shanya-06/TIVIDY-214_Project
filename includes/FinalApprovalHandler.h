#ifndef FINALAPPROVALHANDLER_H
#define FINALAPPROVALHANDLER_H

#include "ApprovalHandler.h"
#include <functional>

/**
 * @class FinalApprovalHandler
 * @brief Concrete handler for the final approval stage.
 *
 * Evaluates an approval rule and performs the corresponding
 * action if the rule is satisfied. Otherwise, forwards
 * the request to the next handler in the chain.
 *
 * Gang of Four Pattern:
 * - Chain of Responsibility -> Concrete Handler
 */
class FinalApprovalHandler : public ApprovalHandler {
public:
    /** @brief Function used to determine whether the handler accepts a request. */
    using ApprovalRule = std::function<bool(const WorkItem&)>;

    /** @brief Function executed when the approval rule is satisfied. */
    using ApprovalAction = std::function<void(WorkItem&)>;

    /**
     * @brief Constructs a final approval handler.
     * @param rule Condition used to evaluate the work item.
     * @param action Action performed when the rule is satisfied.
     * @throws std::invalid_argument If rule or action is missing.
     */
    FinalApprovalHandler(ApprovalRule rule, ApprovalAction action);

    /**
     * @brief Handles an approval request.
     * @param item Work item requiring approval.
     * @throws std::runtime_error If no handler accepts the request.
     *
     * Executes the approval action if the rule matches.
     * Otherwise, forwards the request to the successor.
     */
    void handleRequest(WorkItem& item) override;

private:
    /** @brief Rule used to evaluate approval requests. */
    ApprovalRule rule_;

    /** @brief Action performed when the approval rule matches. */
    ApprovalAction action_;
};

#endif
