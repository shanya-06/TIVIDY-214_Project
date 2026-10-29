#ifndef WORKFLOWBUILDER_H
#define WORKFLOWBUILDER_H

class WorkflowDefinition;
class WorkflowInstance;

/**
 * @class WorkflowBuilder
 * @brief Builds WorkflowInstance objects in pieces
 *
 * Stores construction parameters and creates
 * WorkflowInstance objects through build().
 *
 * Gang of Four Pattern:
 * - Builder -> Concrete Builder
 */
class WorkflowBuilder
{
private:
    WorkflowDefinition* definition;
public:

    /**
     * @brief Creates an empty workflow builder.
     */
    WorkflowBuilder();

    /**
     * @brief Sets the workflow definition used
     *        during construction.
     *
     * @param def Workflow definition.
     *
     * @return Reference to this builder.
     */
    WorkflowBuilder& withDefinition(WorkflowDefinition* def);


    /**
     * @brief Builds a WorkflowInstance.
     *
     * @throws std::logic_error if no definition
     *         has been provided.
     *
     * @return Newly created workflow instance.
     */
    WorkflowInstance* build();
};

#endif
