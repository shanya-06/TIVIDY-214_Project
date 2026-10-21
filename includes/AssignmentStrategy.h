
#ifndef ASSIGNMENTSTRATEGY_H
#define ASSIGNMENTSTRATEGY_H

class WorkItem;
class WorkflowParticipant;

/**
 * @brief Abstract Strategy interface for work assignment.
 *
 * Defines a common method for selecting a suitable
 * participant for a work item.
 *
 * Concrete strategies will implement different
 * assignment algorithms.
 */
class AssignmentStrategy {
public:
    virtual ~AssignmentStrategy() = default;

    virtual WorkflowParticipant* selectParticipant(
        WorkItem* item
    ) = 0;
};

#endif
