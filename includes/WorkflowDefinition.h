#ifndef WORKFLOWDEFINITION_H
#define WORKFLOWDEFINITION_H

#include <string>
class WorkFlowInstance;
class WorkflowBuilder;

/**
 * @class WorkflowDefinition
 * @brief Template workflow configuration.
 *
 * A WorkflowDefinition stores the data needed to
 * create workflow instances. New WorkflowInstance
 * objects are created through createInstance().
 *
 * Gang of Four Pattern:
 * - Builder -> Director
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
     * @brief Director: has the builder build an instance, step by step.
     *
     * @param builder Concrete builder for this kind of workflow.
     * @return The finished instance (owned by the builder).
     */
    WorkFlowInstance* createInstance(WorkflowBuilder& builder);
    /** @brief Same, but gives the new instance its own id (e.g. "INS-CLAIM-3") so its audit trail names it. */
    WorkFlowInstance* createInstance(WorkflowBuilder& builder, const std::string& instanceId);

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
