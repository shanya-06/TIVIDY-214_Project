#ifndef WORKFLOWPARTICIPANT_H
#define WORKFLOWPARTICIPANT_H

#include <string>

#include "WorkflowColleague.h"

class Mediator;

/**
 * @brief A person, team, department, or system that performs WorkItems.
 *
 * WorkflowParticipant is a concrete colleague in the Mediator pattern.
 * It reports availability changes through WorkflowColleague::changed().
 *
 * The participant does not own the Mediator or any WorkItems.
 */
class WorkflowParticipant : public WorkflowColleague
{
public:
    /**
     * @brief Constructs a workflow participant.
     *
     * @param id Unique participant identifier.
     * @param name Participant name.
     * @param role Participant role.
     * @param mediator Mediator that coordinates the participant.
     */
    WorkflowParticipant(
        const std::string& id,
        const std::string& name,
        const std::string& role,
        Mediator* mediator
    );
    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowParticipant();
    /**
     * @brief Returns the participant identifier.
     *
     * @return Participant identifier.
     */
    std::string getId() const;
    /**
     * @brief Returns the participant name.
     *
     * @return Participant name.
     */
    std::string getName() const;
    /**
     * @brief Returns the participant role.
     *
     * @return Participant role.
     */
    std::string getRole() const;
    /**
     * @brief Changes the participant's availability.
     *
     * If the value changes, the participant notifies its Mediator.
     *
     * @param isAvailable New availability value.
     */
    void setAvailable(bool isAvailable);
    /**
     * @brief Checks whether the participant is available.
     *
     * @return true if available, otherwise false.
     */
    bool getAvailable() const;
    /**
     * @brief Returns the number of assigned WorkItems.
     *
     * @return Current workload.
     */
    int getWorkLoad() const;
    /**
     * @brief Increases the participant's workload by one.
     */
    void incrementWorkload();
    /**
     * @brief Decreases the participant's workload by one.
     *
     * The workload cannot become negative.
     */
    void decrementWorkload();

private:
    /**
     * @brief Unique participant identifier.
     */
    std::string id;
    /**
     * @brief Participant name.
     */
    std::string name;
    /**
     * @brief Participant role used by assignment strategies.
     */
    std::string role;
    /**
     * @brief Indicates whether the participant can receive work.
     */
    bool available;
    /**
     * @brief Number of WorkItems currently assigned.
     */
    int workload;
};

#endif