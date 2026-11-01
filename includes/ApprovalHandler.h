#ifndef APPROVALHANDLER_H
#define APPROVALHANDLER_H

class WorkItem;

/**
 * @brief abstract handler for processing work item approvals.
 *
 * forms part of the chain of responsibility pattern.
 * each handler processes a request or forwards it to its successor.
 */
class ApprovalHandler {
protected:
    /** @brief next handler in the approval chain. */
    ApprovalHandler* successor = nullptr;

    /**
     * @brief forwards an approval request to the next handler.
     * @param item work item requiring approval.
     * @throws std::runtime_error if no successor exists.
     */
    void forwardRequest(WorkItem& item);

public:
    /** @brief virtual destructor for the approval handler. */
    virtual ~ApprovalHandler() = default;

    /**
     * @brief sets the next handler in the approval chain.
     * @param next pointer to the next handler, or nullptr.
     * @throws std::invalid_argument if the handler references itself
     * or creates a cycle in the chain.
     */
    void setSuccessor(ApprovalHandler* next);

    /**
     * @brief handles an approval request.
     * @param item work item requiring approval.
     *
     * implemented by concrete handlers.
     */
    virtual void handleRequest(WorkItem& item) = 0;
};

#endif
