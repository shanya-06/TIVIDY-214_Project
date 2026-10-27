#ifndef WORKFLOWPARTICIPANT_H
#define WORKFLOWPARTICIPANT_H

#include <string>

#include "WorkflowColleague.h"

class Mediator;

/**
 * @brief A person or system that does work items.
 *
 * Mediator colleague: tells the mediator when its availability changes,
 * so the mediator can reassign its open work. The mediator raises and
 * lowers the workload as items are assigned and finished.
 */
class WorkflowParticipant : public WorkflowColleague {
public:
    WorkflowParticipant(const std::string& id, const std::string& name, const std::string& role,
                        Mediator* mediator);
    virtual ~WorkflowParticipant();

    std::string getId() const;
    std::string getName() const;
    std::string getRole() const;

    /** @brief Changes availability and notifies the mediator if it actually changed. */
    void setAvailable(bool isAvailable);
    bool getAvailable() const;

    int getWorkLoad() const;
    void incrementWorkload();
    /** @brief Lowers the workload by one, never below zero. */
    void decrementWorkload();

private:
    std::string id;
    std::string name;
    std::string role;
    bool available;
    int workload;
};

#endif
