#include "WorkflowInstance.h"

#include "Command.h"
#include "WorkItem.h"
#include "WorkflowMediator.h"

WorkflowInstance::WorkflowInstance(const std::string& id, const Map& data)
                  : id(id), status(CREATED), data(data), mediator(nullptr) {}

WorkflowInstance::~WorkflowInstance() {}
