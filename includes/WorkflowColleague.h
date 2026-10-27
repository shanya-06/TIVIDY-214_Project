#ifndef WORKFLOWCOLLEAGUE_H
#define WORKFLOWCOLLEAGUE_H

#include "Mediator.h"

/**
 * @brief Abstract colleague base class for the Mediator pattern.
 *
 * A WorkflowColleague stores a reference to a Mediator and uses
 * changed() to report events without communicating directly with
 * other colleagues.
 */
class WorkflowColleague
{
protected:
    /**
     * @brief Mediator responsible for coordinating this colleague.
     *
     * This pointer is non-owning. The colleague does not delete it.
     */
    Mediator* mediator;

public:
    /**
     * @brief Constructs a colleague connected to a Mediator.
     *
     * @param mediator Mediator that coordinates this colleague.
     */
    WorkflowColleague(Mediator* mediator);
    /**
     * @brief Constructs a colleague without a Mediator.
     *
     * The mediator pointer is initialized to nullptr.
     */
    WorkflowColleague();
    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~WorkflowColleague();
    /**
     * @brief Reports that this colleague has changed.
     *
     * If a Mediator is connected, this function calls
     * Mediator::notify(this).
     */
    void changed();
    /**
     * @brief Changes the Mediator used by this colleague.
     *
     * @param mediator New Mediator. May be nullptr.
     */
    void setMediator(Mediator* mediator);
    /**
     * @brief Returns the current Mediator.
     *
     * @return Non-owning pointer to the Mediator, or nullptr.
     */
    Mediator* getMediator() const;
};

#endif