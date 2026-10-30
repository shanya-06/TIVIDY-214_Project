
#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

#include <vector>

class WorkItem;
class WorkflowParticipant;

/**
 * @brief Abstract Strategy interface for work assignment.
 *
 * Allows different algorithms to select a suitable
 * workflow participant for a work item.
 *
 * The strategy does not own the work item or participants.
 */
class AssignmentStrategy {
public:
    virtual ~AssignmentStrategy() = default;

    /**
     * @brief Selects a suitable participant.
     * @param item Work item requiring assignment.
     * @param participants Available candidates to evaluate.
     * @return Selected participant, or nullptr if none qualifies.
     */
    virtual WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) = 0;
};

#endif
