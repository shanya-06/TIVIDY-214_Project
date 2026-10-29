#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <string>
class WorkflowInstance;

/**
 * @class WorkflowDefinition
 * @brief Template workflow configuration.
 *
 * A WorkflowDefinition stores the data needed to
 * create workflow instances. New WorkflowInstance
 * objects are created through createInstance().
 *
 * Gang of Four Pattern:
 * - Builder -> Abstract Product
 */
class WorkflowDefinition
{
private:
    std::string id;
    std::string name;
    int version;

public:
    /**
     * @brief Constructs a workflow definition.
     *
     * @param id Unique workflow definition identifier.
     * @param name Workflow name.
     * @param version Definition version number.
     */
    WorkflowDefinition(const std::string& id, const std::string& name, int version);

    /**
     * @brief Creates a workflow instance from this definition.
     *
     *
     * @return Newly created workflow instance.
     */
    WorkflowInstance* createInstance();

    /**
     * @brief getId
     * @return id variable
     */
    std::string getId() const;

    /**
     * @brief getName
     * @return name variable
     */
    std::string getName() const;

    /**
     * @brief getVersion
     * @return version variable
     */
    int getVersion() const;
};

#endif
