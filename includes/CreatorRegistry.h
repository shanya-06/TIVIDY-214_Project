#ifndef CREATORREGISTRY_H
#define CREATORREGISTRY_H
#include <map>
#include <string>
#include "WorkItemCreator.h"

/**
 * @class CreatorRegistry
 * @brief Registry that manages work item creators.
 *
 * Maps each work item type to its corresponding creator,
 * allowing the workflow system to create different work items
 * without using conditional statements.
 *
 * Supports the Factory Method design pattern.
 */
class CreatorRegistry {
private:
/** @brief Maps work item types to their registered creators. */
    std::map<std::string, WorkItemCreator*> creators;

public:

    /**
     * @brief Registers a creator for a specific work item type.
     * @param type Type of work item to register.
     * @param creator Pointer to the creator responsible for that type.
     */
    void registerCreator(const std::string& type, WorkItemCreator* creator);
    
    /**
     * @brief Creates a work item using its registered creator.
     * @param type Type of work item to create.
     * @param id Unique identifier of the work item.
     * @param name Name of the work item.
     * @param requiredRole Role required to perform the work item.
     * @param needsApproval Indicates whether approval is required.
     * @param mediator Mediator responsible for coordinating the work item.
     * @return Pointer to the newly created work item.
     * @throws std::invalid_argument If no valid creator is registered for the type.
     */
    WorkItem* create(const std::string& type, const std::string& id, const std::string& name,
                     const std::string& requiredRole, bool needsApproval,
                     Mediator* mediator = nullptr);
};
#endif

/*adding a registry to allow system to find the correct creator from it
the registry maps each step type to its creator, so the engine simply asks the registry for a type and gets back the right creator — no if/else is needed*/