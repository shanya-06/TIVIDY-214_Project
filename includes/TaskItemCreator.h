//another creator
#ifndef TASKITEMCREATOR_H
#define TASKITEMCREATOR_H
#include "WorkItemCreator.h"

/**
 * @brief Concrete creator for task work items.
 *
 * Creates TaskItem objects using the Factory Method pattern.
 */
class TaskItemCreator : public WorkItemCreator {
public:
    /**
     * @brief Constructs a task item creator.
     * @param observer Observer to attach to created work items.
     */
    explicit TaskItemCreator(WorkflowObserver* observer);
    
    /**
     * @brief Creates a new task work item.
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Indicates whether approval is required.
     * @param mediator Mediator responsible for coordinating the item.
     * @return Pointer to the newly created TaskItem.
     */
    WorkItem* createItem(const std::string& id, const std::string& name,
                         const std::string& requiredRole, bool needsApproval,
                         Mediator* mediator) override;
};
#endif