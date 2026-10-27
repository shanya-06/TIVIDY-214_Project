
#ifndef WORKLOADASSIGNMENTSTRATEGY_H
#define WORKLOADASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on workload.
 *
 * Chooses an eligible participant with the
 * lowest workload.
 *
 * This strategy is independent of workflow industry.
 */
class WorkloadAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item
    ) override;
};

#endif
