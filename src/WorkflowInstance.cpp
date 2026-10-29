#include "WorkflowInstance.h"

#include "Command.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"

WorkflowInstance::WorkflowInstance(const std::string& id)
                  : id(id), status(CREATED), mediator(nullptr) {}

WorkflowInstance::~WorkflowInstance() {}

std::string WorkflowInstance::getId() const{
    return id;
}

WFStatus WorkflowInstance::getStatus() const{
    return status;
}

void WorkflowInstance::setStatus(WFStatus status){
    this->status = status;
}

void WorkflowInstance::addWorkItem(WorkItem* item){
    if (item){
        workItems.push_back(item);
    }
}

std::vector<WorkItem*> WorkflowInstance::getWorkItems() const{
    return workItems;
}

void WorkflowInstance::addCommand(Command* command){
    if (command){
        history.push_back(command);
    }
}

std::vector<Command*> WorkflowInstance::getHistory() const{
    return history;
}

void WorkflowInstance::setMediator(WorkflowMediator* mediator){
    this->mediator = mediator;
}

WorkflowMediator* WorkflowInstance::getMediator() const{
    return mediator;
}

