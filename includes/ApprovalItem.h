//one of the product as well
#ifndef APPROVALITEM_H
#define APPROVALITEM_H
#include "WorkItem.h"

/**
 * @brief represents an approval work item in the workflow.
 *
 * concrete product in the factory method pattern.
 * inherits the lifecycle behaviour of WorkItem.
 */
class ApprovalItem : public WorkItem {
public:

    /**
     * @brief constructs an approval work item.
     * @param id unique identifier of the work item.
     * @param name name of the work item.
     * @param requiredRole role required to perform the work item.
     * @param needsApproval indicates whether approval is required.
     * @param mediator mediator responsible for coordinating the item.
     */
    ApprovalItem(const std::string& id, const std::string& name, const std::string& requiredRole,
                 bool needsApproval, Mediator* mediator = nullptr);
};
#endif