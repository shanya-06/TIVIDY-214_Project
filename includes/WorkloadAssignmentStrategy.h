
#ifndef WORKLOADASSIGNMENTSTRATEGY_H
#define WORKLOADASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on workload.
 *
 * Finds available participants with the required
 * role and selects the one with the lowest workload.
 *
 * Works with any workflow scenario.
 */
class WorkloadAssignmentStrategy : public AssignmentStrategy {
public:
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
