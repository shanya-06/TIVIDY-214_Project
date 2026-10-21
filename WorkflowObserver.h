#ifndef WORKFLOWOBSERVER_H
#define WORKFLOWOBSERVER_H

#include <string>

class WorkItem;

class WorkflowObserver {
public:
    virtual ~WorkflowObserver() = default;

    virtual void update(WorkItem* item, const std::string& event) = 0;
};

#endif