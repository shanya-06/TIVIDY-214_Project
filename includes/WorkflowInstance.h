#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <string>
#include <vector>
#include "WFStatus.h"

class Command;
class WorkItem;
class WorkflowMediator;

/**
 * @class WorkflowInstance
 * @brief Represents a running workflow.
 *
 * Stores workflow data, work items, command history and execution status.
 *
 * Gang of Four Pattern:
 * - Builder -> Product
 */
class WorkflowInstance
{
private:
    std::string id;
    WFStatus status;
    std::vector<Command*> history;
    std::vector<WorkItem*> workItems;
    WorkflowMediator* mediator;
public:

    /**
     * @brief Creates a workflow instance.
     *
     * @param id Workflow instance identifier.
     * @param data Initial workflow data.
     */
    WorkflowInstance(const std::string& id);

    virtual ~WorkflowInstance();

    /**
     * @brief Returns the workflow id.
     * @return Workflow id.
     */
    std::string getId() const;

    /**
     * @brief Returns the workflow status.
     * @return Current workflow status.
     */
    WFStatus getStatus() const;

    /**
     * @brief Changes the workflow status.
     * @param status New status.
     */
    void setStatus(WFStatus status);

    /**
     * @brief Adds a work item to the workflow.
     * @param item Work item to add.
     */
    void addWorkItem(WorkItem* item);

    /**
     * @brief Returns all workflow work items.
     * @return List of work items.
     */
    std::vector<WorkItem*> getWorkItems() const;

    /**
     * @brief Records an executed command.
     * @param command Executed command.
     */
    void addCommand(Command* command);

    /**
     * @brief Returns command history.
     * @return Executed command history.
     */
    std::vector<Command*> getHistory() const;

    /**
     * @brief Sets the workflow mediator.
     * @param mediator Workflow mediator.
     */
    void setMediator(
        WorkflowMediator* mediator);

    /**
     * @brief Returns the workflow mediator.
     * @return Workflow mediator.
     */
    WorkflowMediator* getMediator() const;
};

#endif
