#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <string>
#include <vector>

#include "WFStatus.h"
#include "WorkflowObserver.h"

class Command;
class WorkItem;
class WorkflowMediator;

/**
 * @brief One independent running copy of a workflow definition.
 *
 * Owns its work items, its command history and its own WorkflowMediator,
 * so nothing is shared with other instances.
 * Observes its own work items so its status stays current:
 * FAILED as soon as an item is Rejected, COMPLETED once every item is Completed.
 */
class WorkFlowInstance : public WorkflowObserver {
public:
    WorkFlowInstance(const std::string& id);
    virtual ~WorkFlowInstance();

    WorkFlowInstance(const WorkFlowInstance&) = delete;
    WorkFlowInstance& operator=(const WorkFlowInstance&) = delete;

    std::string getId() const;

    /** @brief Takes ownership of item and starts observing it. */
    void addWorkItem(WorkItem* item);
    std::vector<WorkItem*> getWorkItems() const;

    /** @brief Takes ownership of mediator (deleting any previous one). */
    void setMediator(WorkflowMediator* newMediator);
    WorkflowMediator* getMediator() const;

    /** @brief Runs command and keeps it in the history. Takes ownership of command. */
    void executeCommand(Command* command);

    /** @brief Moves to RUNNING and makes the first item available. Later items open one at a time as each completes. */
    void start();
    WFStatus getStatus() const;

    /** @brief Observer callback from one of this instance's work items. */
    virtual void update(WorkItem* item, const std::string& event) override;

private:
    void updateStatus();
    /** @brief Makes the first item still in Created available (the next one in order). */
    void openNextItem();

    std::string id;
    WFStatus status;
    std::vector<Command*> history;
    std::vector<WorkItem*> workItems;
    WorkflowMediator* mediator;
};

#endif
