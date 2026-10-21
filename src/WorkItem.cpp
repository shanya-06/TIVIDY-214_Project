#include "WorkItem.h"
#include "WorkflowObserver.h"
#include "WorkflowParticipant.h"
#include "WorkItemState.h"
#include <algorithm>

WorkItem::WorkItem():state(nullptr),assignedParticipant(nullptr){
}

void WorkItem::attach(WorkflowObserver* observer){
    observers.push_back(observer);
}

void WorkItem::detach(WorkflowObserver* observer){
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
        observers.end());
}

void WorkItem::notifyObservers(const std::string& event){
    for (WorkflowObserver* observer : observers){
        observer->update(event);
    }
}

void WorkItem::assign(WorkflowParticipant* participant){
    assignedParticipant = participant;
    notifyObservers("Work item assigned");
}

void WorkItem::start(){
    if (state != nullptr){
        state->start(this);
    }
    notifyObservers("Work item started");
}

void WorkItem::complete(const std::string& result){
    if (state != nullptr){
        state->complete(this);
    }
    notifyObservers("Work item completed: " + result);
}

void WorkItem::reject(const std::string& reason){
    notifyObservers("Work item rejected: " + reason);
}

void WorkItem::escalate(){
    notifyObservers("Work item escalated");
}

void WorkItem::setState(WorkItemState* newState){
    state = newState;
}

WorkItemState* WorkItem::getState() const{
    return state;
}