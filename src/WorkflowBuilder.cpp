#include "WorkflowBuilder.h"
#include "WorkflowDefinition.h"
#include "WorkflowInstance.h"

WorkflowBuilder::WorkflowBuilder()
                  : definition(nullptr) {}

WorkflowBuilder& WorkflowBuilder::withDefinition(WorkflowDefinition* def){
    definition = def;
    return *this;
}

WorkflowBuilder& WorkflowBuilder::withData(const Map& data){
    this->data = data;
    return *this;
}

WorkflowInstance* WorkflowBuilder::build(){
    if (!definition){
        //return message or print err msg to console
    }

    return definition->createInstance(data);
}
