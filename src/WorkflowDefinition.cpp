#include "WorkflowDefinition.h"
#include "WorkflowBuilder.h"

WorkflowDefinition::WorkflowDefinition(const std::string& id, const std::string& name, int version)
    : id(id), name(name), version(version) {}

WorkFlowInstance* WorkflowDefinition::createInstance(WorkflowBuilder& builder){
    return createInstance(builder, id);
}

WorkFlowInstance* WorkflowDefinition::createInstance(WorkflowBuilder& builder, const std::string& instanceId){
    builder.createInstance(instanceId);
    builder.addParticipants();
    builder.addCoordination();
    builder.addWorkItems();
    return builder.getInstance();
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
