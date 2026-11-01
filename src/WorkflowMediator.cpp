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

std::vector<WorkflowParticipant*> WorkflowMediator::getParticipants() const
{
    return participants;
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
        item = item->getOutermost();   // act through the decorators
    }

    if(item != nullptr)
    {
        /*
         * A WorkItem reports its change to the Mediator.
         * The Mediator can coordinate the next workflow action.
         */
        if(item->isCompleted())
        {
            notifyCompleted(item);
        }
        else if(item->isRejected())
        {
            // A rejected item is finished: free the assignee, but do not process it further.
            if(item->getAssignee() != nullptr)
            {
                item->getAssignee()->decrementWorkload();
            }
        }
        else if(item->isAwaitingApproval())
        {
            requestApproval(item);
        }
        else if(item->isInProgress())
        {
            // Work just started: validate, execute and route it (Template Method).
            WorkItemProcessor* itemProcessor = item->getProcessor();
            if(itemProcessor == nullptr)
            {
                itemProcessor = processor;
            }
            if(itemProcessor != nullptr)
            {
                itemProcessor->process(item);
            }
        }
        else if(item->getAssignee() == nullptr && item->canBeAssigned())
        {
            requestAssignment(item);
        }

        return;
    }

    WorkflowParticipant* participant = dynamic_cast<WorkflowParticipant*>(originator);

    if(participant == nullptr)
    {
        return;
    }

    if(!participant->getAvailable())
    {
        /*
         * The participant became unavailable: every item they hold is
         * escalated (the only lifecycle move that allows reassignment)
         * and then handed to someone else by the AssignmentStrategy.
         */
        for(std::vector<WorkItem*>::iterator it = workItems.begin();it != workItems.end(); ++it)
        {
            WorkItem* itemForParticipant = *it;

            if (itemForParticipant != nullptr
                && itemForParticipant->getAssignee() == participant
                && itemForParticipant->canBeEscalated())
            {
                itemForParticipant->escalate();
                reassign(itemForParticipant);
            }
        }
    }
    else
    {
        /*
         * The participant is available again: items that were waiting
         * for someone (Available or Escalated) get another chance.
         */
        for(std::vector<WorkItem*>::iterator it = workItems.begin();it != workItems.end(); ++it)
        {
            WorkItem* waiting = *it;

            if (waiting != nullptr && waiting->canBeAssigned())
            {
                requestAssignment(waiting);
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

    if (previousParticipant == selectedParticipant && !item->canBeAssigned())
    {
        return;
    }

    // WorkItem::assign() keeps the participants' workloads up to date.
    item->assign(selectedParticipant);
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