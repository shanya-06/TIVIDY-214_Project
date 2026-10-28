#ifndef WORKFLOWMEDIATOR_H
#define WORKFLOWMEDIATOR_H

#include "Mediator.h"

#include <map>
#include <string>
#include <vector>

class WorkflowColleague;
class WorkflowParticipant;
class WorkItem;
class AssignmentStrategy;
class ApprovalHandler;
class WorkItemProcessor;
class ExternalSystemAdapter;

/**
 * @brief Concrete Mediator for the TIVIDY workflow engine.
 *
 * WorkflowMediator coordinates WorkItems and WorkflowParticipants.
 * It delegates participant selection to AssignmentStrategy, processing
 * to WorkItemProcessor, approvals to ApprovalHandler, and external
 * operations to ExternalSystemAdapter.
 *
 * The Mediator stores non-owning pointers. It does not delete the
 * registered WorkItems, participants, or service objects.
 */
class WorkflowMediator : public Mediator
{
private:
    /**
     * @brief WorkItems coordinated by this Mediator.
     */
    std::vector<WorkItem*> workItems;
    /**
     * @brief Participants available for assignment.
     */
    std::vector<WorkflowParticipant*> participants;
    /**
     * @brief Strategy used to select a participant.
     */
    AssignmentStrategy* assignmentStrategy;
    /**
     * @brief Approval chain used for approval requests.
     */
    ApprovalHandler* approvalHandler;
    /**
     * @brief Processor used to validate, execute, and route WorkItems.
     */
    WorkItemProcessor* processor;
    /**
     * @brief Adapter used to communicate with external systems.
     */
    ExternalSystemAdapter* externalAdapter;

public:
    /**
     * @brief Constructs an empty workflow Mediator.
     */
    WorkflowMediator();
    /**
     * @brief Destroys the Mediator.
     *
     * Registered objects are not deleted because the Mediator
     * does not own them.
     */
    virtual ~WorkflowMediator();
    /**
     * @brief Registers a WorkItem for coordination.
     *
     * Duplicate pointers are ignored. The WorkItem is connected
     * to this Mediator.
     *
     * @param item WorkItem to register.
     */
    void registerWorkItem(WorkItem* item);
    /**
     * @brief Removes a WorkItem from coordination.
     *
     * @param item WorkItem to unregister.
     */
    void unregisterWorkItem(WorkItem* item);
    /**
     * @brief Registers a workflow participant.
     *
     * Duplicate pointers are ignored. The participant is connected
     * to this Mediator.
     *
     * @param participant Participant to register.
     */
    void registerParticipant(WorkflowParticipant* participant);
    /**
     * @brief Removes a participant from coordination.
     *
     * @param participant Participant to unregister.
     */
    void unregisterParticipant(WorkflowParticipant* participant);
    /**
     * @brief Sets the participant assignment strategy.
     *
     * @param strategy Strategy used to select participants.
     */
    void setAssignmentStrategy(AssignmentStrategy* strategy);
    /**
     * @brief Sets the approval handler chain.
     *
     * @param handler First handler in the approval chain.
     */
    void setApprovalHandler(ApprovalHandler* handler);
    /**
     * @brief Sets the WorkItem processor.
     *
     * @param processor Processor used to process WorkItems.
     */
    void setProcessor(WorkItemProcessor* processor);
    /**
     * @brief Sets the external-system adapter.
     *
     * @param adapter Adapter used for external operations.
     */
    void setExternalAdapter(ExternalSystemAdapter* adapter);
    /**
     * @brief Responds to a change reported by a colleague.
     *
     * A WorkItem change may trigger assignment. A participant becoming
     * unavailable may trigger reassignment of its WorkItems.
     *
     * @param originator Colleague that reported the change.
     */
    void notify(WorkflowColleague* originator) override;
    /**
     * @brief Requests assignment of a WorkItem.
     *
     * The AssignmentStrategy selects a participant from the registered
     * participants. The Mediator then performs the assignment and updates
     * participant workload values.
     *
     * @param item WorkItem requiring assignment.
     */
    void requestAssignment(WorkItem* item);
    /**
     * @brief Sends a WorkItem to the approval chain.
     *
     * @param item WorkItem requiring approval.
     */
    void requestApproval(WorkItem* item);
    /**
     * @brief Handles completion of a WorkItem.
     *
     * Decreases the assignee's workload and sends the WorkItem to
     * the configured processor when one is available.
     *
     * @param item Completed WorkItem.
     */
    void notifyCompleted(WorkItem* item);
    /**
     * @brief Requests reassignment of a WorkItem.
     *
     * The current participant is replaced through the configured
     * AssignmentStrategy.
     *
     * @param item WorkItem requiring reassignment.
     */
    void reassign(WorkItem* item);
    /**
     * @brief Requests an operation from an external system.
     *
     * The request is translated and executed by ExternalSystemAdapter.
     *
     * @param operation Name of the requested operation.
     * @param data Operation data.
     * @return Result returned by the external adapter, or an empty string
     *         when no adapter is configured.
     */
    std::string requestExternalOperation(
        const std::string& operation,
        const std::map<std::string, std::string>& data);
};

#endif