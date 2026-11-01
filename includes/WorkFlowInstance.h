#ifndef WORKFLOWINSTANCE_H
#define WORKFLOWINSTANCE_H

#include <functional>
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

    /** @brief Everything that happened to this instance's items, oldest first ("id (name): event"). */
    const std::vector<std::string>& getEvents() const;

    /**
     * @brief Adds a work item to run right after another one (used for a routing decision).
     */
    void addWorkItemAfter(WorkItem* after, WorkItem* item);

    /**
     * @brief Called each time a work item completes, before the next item opens.
     *
     * Lets the industry decide the route, for example by adding an extra step
     * with addWorkItemAfter(). Without a rule the items simply run in order.
     */
    typedef std::function<void(WorkFlowInstance&, WorkItem*)> RoutingRule;
    void setRoutingRule(RoutingRule rule);

    /** @brief Commands that executed successfully, oldest first. Owned by the instance. */
    std::vector<Command*> getHistory() const;

    /** @brief Moves to RUNNING and makes the first item available. Later items open one at a time as each completes. */
    void start();

    /**
     * @brief Cancels a running instance: no further work is opened and the workload of unfinished items is released.
     * @throws std::logic_error if the instance is not running.
     */
    void cancel();
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
    std::vector<std::string> events;
    RoutingRule routingRule;
    WorkflowMediator* mediator;
};

#endif
