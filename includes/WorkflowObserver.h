#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include <string>
#include <vector>

class WorkItem;

/**
 * @brief observer interface for monitoring work item changes.
 *
 * defines the update method that all concrete observers
 * must implement to respond to work item events.
 */
class WorkflowObserver {
public:
    /**
     * @brief destroys the workflow observer.
     */
    virtual ~WorkflowObserver() = default;

    /**
     * @brief receives notifications when a work item changes.
     * @param item pointer to the work item that changed.
     * @param event description of the event that occurred.
     */
    virtual void update(WorkItem* item, const std::string& event) = 0;
};


//----------------------------CONCRETE OBSERVERS----------------------------

/**
 * @brief records work item events for auditing.
 *
 * observes work item changes and records information
 * about events for audit purposes.
 */
class AuditObserver : public WorkflowObserver {
public:
    /**
     * @brief handles a work item event for auditing.
     * @param item pointer to the work item that changed.
     * @param event description of the event that occurred.
     */
    void update(WorkItem* item, const std::string& event) override;

    /** @brief every event recorded so far, oldest first. */
    const std::vector<std::string>& getLog() const;

private:
    std::vector<std::string> log;
};

/**
 * @brief handles notifications about work item changes.
 *
 * observes work item events and generates notifications
 * when a work item's state changes.
 */
class NotificationObserver : public WorkflowObserver {
public:
    /**
     * @brief handles a work item notification event.
     * @param item pointer to the work item that changed.
     * @param event description of the event that occurred.
     */
    void update(WorkItem* item, const std::string& event) override;
};

/**
 * @brief monitors work item progress and state changes.
 *
 * observes work item events to track the current
 * state and progress of workflow tasks.
 */
class MonitoringObserver : public WorkflowObserver {
public:
    /**
     * @brief handles a work item progress update.
     * @param item pointer to the work item that changed.
     * @param event description of the event that occurred.
     */
    void update(WorkItem* item, const std::string& event) override;
};

#endif
