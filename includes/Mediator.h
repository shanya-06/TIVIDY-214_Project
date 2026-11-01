#ifndef MEDIATOR_H
#define MEDIATOR_H

class WorkflowColleague;

/**
 * @brief Abstract Mediator interface for TIVIDY.
 *
 * Defines the operation used by workflow colleagues to report
 * changes without communicating directly with one another.
 *
 * The concrete WorkflowMediator implements the coordination rules
 * between WorkItems and WorkflowParticipants.
 */
class Mediator
{
public:
    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~Mediator() = default;

    /**
     * @brief Receives a notification from a colleague.
     *
     * The originator identifies the colleague whose state or data
     * changed. The concrete mediator decides what action should follow.
     *
     * @param originator Colleague that reported the change.
     */
    virtual void notify(WorkflowColleague* originator) = 0;
};

#endif