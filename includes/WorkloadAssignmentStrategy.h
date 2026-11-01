#ifndef WORKLOADASSIGNMENTSTRATEGY_H
#define WORKLOADASSIGNMENTSTRATEGY_H

#include "AssignmentStrategy.h"

/**
 * @brief Selects a participant based on workload.
 *
 * Finds available participants with the required role
 * and selects the one with the lowest workload.
 *
 * Implements the Strategy design pattern.
 */
class WorkloadAssignmentStrategy : public AssignmentStrategy {
public:
    /**
     * @brief Selects the available participant with the lowest workload.
     *
     * Only considers participants whose role matches the work item's
     * required role and who are currently available.
     *
     * If multiple participants have the same lowest workload,
     * the first matching participant is selected.
     *
     * @param item Work item requiring assignment.
     * @param participants List of participants to consider.
     * @return Pointer to the selected participant, or nullptr if
     *         the item is null or no suitable participant exists.
     *
     * @note The returned participant is not owned by this strategy.
     */
    WorkflowParticipant* selectParticipant(
        WorkItem* item,
        const std::vector<WorkflowParticipant*>& participants
    ) override;
};

#endif
