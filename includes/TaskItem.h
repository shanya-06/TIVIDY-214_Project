#ifndef TASKITEM_H
#define TASKITEM_H
#include "WorkItem.h"

/**
 * @brief Represents a task work item in the workflow.
 *
 * Concrete product in the Factory Method pattern.
 * Inherits the lifecycle behaviour of WorkItem.
 */
class TaskItem : public WorkItem {
public:

    /**
     * @brief Constructs a task work item.
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Indicates whether approval is required.
     * @param mediator Mediator responsible for coordinating the item.
     */
    TaskItem(const std::string& id, const std::string& name, const std::string& requiredRole,
             bool needsApproval, Mediator* mediator = nullptr);
};
#endif

//this is one of the products