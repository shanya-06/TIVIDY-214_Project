#include "CreatorRegistry.h"

void CreatorRegistry::registerCreator(const std::string& type, WorkItemCreator* creator) {
    creators[type] = creator;
}

WorkItem* CreatorRegistry::create(const std::string& type) {
    return creators.at(type)->instantiate();
}
