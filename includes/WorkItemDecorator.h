#ifndef WORKITEMDECORATOR_H
#define WORKITEMDECORATOR_H
#include "WorkItemComponent.h"
 
//abstract decorator class for the decorator pattern
class WorkItemDecorator : public WorkItemComponent {
protected:
    WorkItemComponent* component;
 
public:
    explicit WorkItemDecorator(WorkItemComponent* component);
    virtual ~WorkItemDecorator();
 
    void assign(WorkflowParticipant* participant) override;
    void start() override;
    void complete(const std::string& result) override;
    void reject(const std::string& reason) override;
    void escalate() override;
};
 
#endif
 