#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include <string>

class WorkItem;

class WorkflowObserver {
public:
    virtual ~WorkflowObserver() = default;

    virtual void update(WorkItem* item, const std::string& event) = 0;
};


//----------------------------CONCRETE OBSERVERS----------------------------

// records work item events for auditing
class AuditObserver : public WorkflowObserver {
public:
    void update(WorkItem* item, const std::string& event) override;
};

// sends notifications when work items change
class NotificationObserver : public WorkflowObserver {
public:
    void update(WorkItem* item, const std::string& event) override;
};

// monitors work item progress and state changes
class MonitoringObserver : public WorkflowObserver {
public:
    void update(WorkItem* item, const std::string& event) override;
};


#endif