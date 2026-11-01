#ifndef WORKITEMCREATOR_H
#define WORKITEMCREATOR_H
#include "WorkItem.h"
#include "WorkflowObserver.h"

/**
 * @brief Abstract creator for work items.
 *
 * Defines the factory method used by concrete creators
 * to construct different types of WorkItem objects.
 *
 * The instantiate() method initializes the work item's
 * state and attaches the configured observer.
 */
class WorkItemCreator {
private:
    /** @brief Observer attached to newly created work items. */
    WorkflowObserver* auditObserver;

public:
    /**
     * @brief Constructs a work item creator.
     * @param observer Observer to attach to created work items.
     */
    explicit WorkItemCreator(WorkflowObserver* observer);

    /** @brief Virtual destructor for the creator interface. */
    virtual ~WorkItemCreator() = default;

    /**
     * @brief Creates a work item of a concrete type.
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Indicates whether approval is required.
     * @param mediator Mediator coordinating the work item.
     * @return Pointer to the newly created work item.
     */
    virtual WorkItem* createItem(const std::string& id, const std::string& name,
                                 const std::string& requiredRole, bool needsApproval,
                                 Mediator* mediator) = 0;

    /**
     * @brief Creates and initializes a work item.
     *
     * Calls createItem(), sets the initial state to CreatedState,
     * and attaches the configured observer.
     *
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Indicates whether approval is required.
     * @param mediator Mediator coordinating the work item.
     * @return Pointer to the initialized work item.
     *
     * @note The caller is responsible for managing the returned pointer.
     */
    WorkItem* instantiate(const std::string& id, const std::string& name,
                          const std::string& requiredRole, bool needsApproval,
                          Mediator* mediator = nullptr);
};

#endif
