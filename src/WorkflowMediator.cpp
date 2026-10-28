#include "WorkflowMediator.h"

#include "ApprovalHandler.h"
#include "AssignmentStrategy.h"
#include "ExternalSystemAdapter.h"
#include "WorkItem.h"
#include "WorkItemProcessor.h"
#include "WorkflowColleague.h"
#include "WorkflowParticipant.h"

#include <algorithm>

WorkflowMediator::WorkflowMediator()
    : assignmentStrategy(nullptr),
      approvalHandler(nullptr),
      processor(nullptr),
      externalAdapter(nullptr)
{
}

WorkflowMediator::~WorkflowMediator()
{
    /*
     * The Mediator does not own these objects.
     * They are managed by the workflow instance or another owner.
     */
}

void WorkflowMediator::registerWorkItem(WorkItem* item)
{
    if (item == nullptr)
    {
        return;
    }

    if (std::find(workItems.begin(), workItems.end(), item) == workItems.end())
    {
        workItems.push_back(item);
        item->setMediator(this);
    }
}

void WorkflowMediator::unregisterWorkItem(WorkItem* item)
{
    workItems.erase(std::remove(workItems.begin(), workItems.end(), item), workItems.end());
}

void WorkflowMediator::registerParticipant(WorkflowParticipant* participant)
{
    if(participant == nullptr)
    {
        return;
    }

    if(std::find(participants.begin(), participants.end(), participant)== participants.end())
    {
        participants.push_back(participant);
        participant->setMediator(this);
    }
}

void WorkflowMediator::unregisterParticipant(
    WorkflowParticipant* participant)
{
    participants.erase(
        std::remove(participants.begin(), participants.end(), participant),
        participants.end()
    );
}

void WorkflowMediator::setAssignmentStrategy(AssignmentStrategy* strategy)
{
    assignmentStrategy = strategy;
}

void WorkflowMediator::setApprovalHandler(ApprovalHandler* handler)
{
    approvalHandler = handler;
}

void WorkflowMediator::setProcessor(WorkItemProcessor* newProcessor)
{
    processor = newProcessor;
}

void WorkflowMediator::setExternalAdapter(ExternalSystemAdapter* adapter)
{
    externalAdapter = adapter;
}

void WorkflowMediator::notify(WorkflowColleague* originator)
{
    if(originator == nullptr)
    {
        return;
    }

    WorkItem* item = dynamic_cast<WorkItem*>(originator);

    if(item != nullptr)
    {
        /*
         * A WorkItem reports its change to the Mediator.
         * The Mediator can coordinate the next workflow action.
         */
        if(item->getAssignee() == nullptr)
        {
            requestAssignment(item);
        }

        return;
    }

    WorkflowParticipant* participant = dynamic_cast<WorkflowParticipant*>(originator);

    if(participant != nullptr && !participant->getAvailable())
    {
        for(std::vector<WorkItem*>::iterator it = workItems.begin();it != workItems.end(); ++it)
        {
            WorkItem* itemForParticipant = *it;

            if (itemForParticipant != nullptr && itemForParticipant->getAssignee() == participant)
            {
                reassign(itemForParticipant);
            }
        }
    }
}

void WorkflowMediator::requestAssignment(WorkItem* item)
{
    if (item == nullptr || assignmentStrategy == nullptr)
    {
        return;
    }

    WorkflowParticipant* selectedParticipant =
        assignmentStrategy->selectParticipant(item, participants);

    if (selectedParticipant == nullptr)
    {
        return;
    }

    WorkflowParticipant* previousParticipant =
        item->getAssignee();

    if (previousParticipant == selectedParticipant)
    {
        return;
    }

    item->assign(selectedParticipant);

    if (previousParticipant != nullptr)
    {
        previousParticipant->decrementWorkload();
    }

    selectedParticipant->incrementWorkload();
}

void WorkflowMediator::requestApproval(WorkItem* item)
{
    if(item == nullptr || approvalHandler == nullptr)
    {
        return;
    }

    approvalHandler->handleRequest(*item);
}

void WorkflowMediator::notifyCompleted(WorkItem* item)
{
    if (item == nullptr)
    {
        return;
    }

    WorkflowParticipant* participant = item->getAssignee();

    if(participant != nullptr)
    {
        participant->decrementWorkload();
    }

    if(processor != nullptr)
    {
        processor->process(item);
    }
}

void WorkflowMediator::reassign(WorkItem* item)
{
    if (item == nullptr)
    {
        return;
    }

    requestAssignment(item);
}

std::string WorkflowMediator::requestExternalOperation(const std::string& operation,const std::map<std::string, std::string>& data)
{
    if (externalAdapter == nullptr)
    {
        return "";
    }

    return externalAdapter->executeOperation(operation, data);
}