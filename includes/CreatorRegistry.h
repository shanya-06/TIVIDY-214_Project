#ifndef CREATORREGISTRY_H
#define CREATORREGISTRY_H
#include <map>
#include <string>
#include "WorkItemCreator.h"

class CreatorRegistry {
private:
    std::map<std::string, WorkItemCreator*> creators;

public:
    void registerCreator(const std::string& type, WorkItemCreator* creator);
    WorkItem* create(const std::string& type);
};
#endif

/*adding a registry to allow system to find the correct creator from it
the registry maps each step type to its creator, so the engine simply asks the registry for a type and gets back the right creator — no if/else is needed*/