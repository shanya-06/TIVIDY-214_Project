#ifndef WORKITEMCOMPONENT_H
#define WORKITEMCOMPONENT_H
#include <string>
 
class WorkflowParticipant;
 
//context of decorator pattern

class WorkItemComponent {
public:
    virtual ~WorkItemComponent() {}
    virtual void assign(WorkflowParticipant* participant) = 0;
    virtual void start() = 0;
    virtual void complete(const std::string& result) = 0;
    virtual void reject(const std::string& reason) = 0;
    virtual void escalate() = 0;
};
 
#endif