#include "WorkflowDefinition.h"

#include "WorkflowInstance.h"

WorkflowDefinition::WorkflowDefinition(const std::string& id, const std::string& name, int version)
    : id(id), name(name), version(version) {}

WorkflowInstance* WorkflowDefinition::createInstance(const Map& data){
    return new WorkflowInstance(id, data);
}

std::string WorkflowDefinition::getId() const{
    return id;
}

std::string WorkflowDefinition::getName() const{
    return name;
}

int WorkflowDefinition::getVersion() const{
    return version;
}
