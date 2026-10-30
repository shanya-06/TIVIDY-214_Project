#ifndef WORKFLOWBUILDER_H
#define WORKFLOWBUILDER_H

#include <string>

class WorkFlowInstance;

/**
 * @class WorkflowBuilder
 * @brief Abstract Builder: the steps needed to assemble a WorkFlowInstance.
 *
 * A concrete builder (one per kind of workflow) fills in each step. The
 * WorkflowDefinition (the Director) calls the steps in the right order.
 * The concrete builder owns the parts it creates (participants, strategy,
 * approval chain, ...) and must outlive the instance it built.
 *
 * Gang of Four Pattern:
 * - Builder -> Abstract Builder
 */
class WorkflowBuilder
{
public:
    virtual ~WorkflowBuilder() {}

    /** @brief Step 1: creates the empty instance and its Mediator. */
    virtual void createInstance(const std::string& id) = 0;

    /** @brief Step 2: creates the participants and registers them with the Mediator. */
    virtual void addParticipants() = 0;

    /** @brief Step 3: gives the Mediator its strategy, approval chain, processor and adapter. */
    virtual void addCoordination() = 0;

    /** @brief Step 4: creates the work items and adds them to the instance. */
    virtual void addWorkItems() = 0;

    /** @brief The finished instance (Product). */
    virtual WorkFlowInstance* getInstance() = 0;
};

#endif
