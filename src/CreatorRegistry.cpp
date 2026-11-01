#include "CreatorRegistry.h"
#include <stdexcept>

void CreatorRegistry::registerCreator(const std::string& type, WorkItemCreator* creator) {
    creators[type] = creator;
}

WorkItem* CreatorRegistry::create(const std::string& type, const std::string& id,
                                  const std::string& name, const std::string& requiredRole,
                                  bool needsApproval, Mediator* mediator) {
    std::map<std::string, WorkItemCreator*>::iterator it = creators.find(type);
    if (it == creators.end() || it->second == nullptr) {
        throw std::invalid_argument("No creator registered for work item type '" + type + "'");
    }
    return it->second->instantiate(id, name, requiredRole, needsApproval, mediator);
}
