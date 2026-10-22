#ifndef WORKFLOWMEDIATOR_H
#define WORKFLOWMEDIATOR_H

#include "Mediator.h"
#include <string>
#include <vector>
#include <map>

class WorkflowColleague;
class WorkflowParticipant;
class WorkItem;
class AssignmentStrategy;
class ApprovalHandler;
class ExternalSystemAdapter;

class WorkflowMediator : public Mediator {
private:
    AssignmentStrategy* assignmentStrategy;
    std::vector<WorkflowColleague*> colleagues;
    ApprovalHandler* approvalHandler;
    ExternalSystemAdapter* externalAdapter;

public:
    WorkflowMediator();
    ~WorkflowMediator() override;
    void registerColleague(WorkflowColleague* colleague);
    void unregisterColleague(WorkflowColleague* colleague);
    void notify(WorkflowColleague* originator) override;
    void setAssignmentStrategy(AssignmentStrategy* strategy);
    void setApprovalHandler(ApprovalHandler* handler);
    void setExternalAdapter(ExternalSystemAdapter* adapter);
    void requestAssignment(WorkItem* item);
    void notifyCompleted(WorkItem* item);
    void requestApproval(WorkItem* item);
    std::string requestExternalOperation(
        const std::string& operation,const std::map<std::string, std::string>& data);
};

#endif