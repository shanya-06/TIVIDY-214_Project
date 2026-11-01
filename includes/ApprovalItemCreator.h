#ifndef APPROVALITEMCREATOR_H
#define APPROVALITEMCREATOR_H
#include "WorkItemCreator.h"

/**
 * @brief concrete creator for approval work items.
 *
 * implements the factory method pattern by creating
 * ApprovalItem objects.
 */
class ApprovalItemCreator : public WorkItemCreator {
public:

     /**
     * @brief constructs an approval item creator.
     * @param observer observer to attach to created work items.
     */
    explicit ApprovalItemCreator(WorkflowObserver* observer);
    
    /**
     * @brief creates a new approval work item.
     * @param id unique identifier of the work item.
     * @param name name of the work item.
     * @param requiredRole role required to perform the work item.
     * @param needsApproval indicates whether approval is required.
     * @param mediator mediator responsible for coordinating the item.
     * @return pointer to the newly created ApprovalItem.
     */
    WorkItem* createItem(const std::string& id, const std::string& name,
                         const std::string& requiredRole, bool needsApproval,
                         Mediator* mediator) override;
};
#endif