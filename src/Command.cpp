#include "Command.h"

#include "WorkItem.h"
#include "WorkflowParticipant.h"
#include "Types.h"


AssignCommand::AssignCommand(
    WorkItem* item,
    WorkflowParticipant* participant)
    : receiver(item),
      participant(participant)
{
    if (!receiver)
        throw std::invalid_argument("Receiver cannot be null");
}

void AssignCommand::execute()
{
    receiver->assign(participant);
}

void AssignCommand::undo()
{
    receiver->makeAvailable();
}

std::string AssignCommand::getDescription() const
{
    return "Assign Work Item";
}


















CompleteWorkCommand::CompleteWorkCommand(
    WorkItem* item,
    const Map& result)
    : receiver(item),
      result(result) {
    if (!receiver)
        throw std::invalid_argument("Receiver cannot be null");
}

void CompleteWorkCommand::execute()
{
    receiver->complete(result);
}

void CompleteWorkCommand::undo()
{
}

std::string CompleteWorkCommand::getDescription() const
{
    return "Complete Work Item";
}













RejectCommand::RejectCommand(
    WorkItem* item,
    const std::string& reason)
    : receiver(item),
      reason(reason) {
    if (!receiver)
        throw std::invalid_argument("Receiver cannot be null");
}

      void RejectCommand::execute()
{
    receiver->reject(reason);
}

void RejectCommand::undo()
{
}

std::string RejectCommand::getDescription() const
{
    return "Reject Work Item";
}














EscalateCommand::EscalateCommand(
    WorkItem* item)
    : receiver(item) {
    if (!receiver)
        throw std::invalid_argument("Receiver cannot be null");
}

void EscalateCommand::execute()
{
    receiver->escalate();
}

void EscalateCommand::undo()
{
}

std::string EscalateCommand::getDescription() const
{
    return "Escalate Work Item";
}
