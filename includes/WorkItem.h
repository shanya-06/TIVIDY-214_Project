#ifndef WORKITEM_H
#define WORKITEM_H
#include <string>
#include <vector>
#include "WorkItemComponent.h"

class WorkflowObserver;
class WorkflowParticipant;
class WorkItemState;

class WorkItem : public WorkItemComponent {
private:
    WorkItemState* state;
    WorkflowParticipant* assignedParticipant;
    std::vector<WorkflowObserver*> observers;

public:
    WorkItem();
    void attach(WorkflowObserver* observer);
    void detach(WorkflowObserver* observer);
    void notifyObservers(const std::string& event);
    void assign(WorkflowParticipant* participant);
    void start();
    void complete(const std::string& result);
    void reject(const std::string& reason);
    void escalate();
    void setState(WorkItemState* state);
    WorkItemState* getState() const;
};

#endif
