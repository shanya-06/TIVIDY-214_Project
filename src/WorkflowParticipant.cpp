#include "WorkflowParticipant.h"

WorkflowParticipant::WorkflowParticipant(const std::string& id, const std::string& name,
                                         const std::string& role, Mediator* mediator)
    : WorkflowColleague(mediator), id(id), name(name), role(role), available(true), workload(0) {}

WorkflowParticipant::~WorkflowParticipant() {}

std::string WorkflowParticipant::getId() const {
    return id;
}

std::string WorkflowParticipant::getName() const {
    return name;
}

std::string WorkflowParticipant::getRole() const {
    return role;
}

void WorkflowParticipant::setAvailable(bool isAvailable) {
    if (available == isAvailable) {
        return;
    }
    available = isAvailable;
    changed();
}

bool WorkflowParticipant::getAvailable() const {
    return available;
}

int WorkflowParticipant::getWorkLoad() const {
    return workload;
}

void WorkflowParticipant::incrementWorkload() {
    ++workload;
}

void WorkflowParticipant::decrementWorkload() {
    if (workload > 0) {
        --workload;
    }
}
