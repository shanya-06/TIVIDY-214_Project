#include "Command.h"

#include "WorkItem.h"
#include "WorkflowParticipant.h"
#include "Types.h"

//Assign Command
AssignCommand::AssignCommand(WorkItem* item, WorkflowParticipant* participant)
                : receiver(item), participant(participant){ 
    if (!receiver) throw std::invalid_argument("Receiver cannot be null"); 
}

void AssignCommand::execute(){
    receiver->assign(participant);
}

void AssignCommand::undo(){
    receiver->makeAvailable();
}

std::string AssignCommand::getDescription() const{
    return "Assign Work Item";
}



//CompleteWork Command
CompleteWorkCommand::CompleteWorkCommand( WorkItem* item, const Map& result)
                    : receiver(item), result(result) {
    if (!receiver)
        throw std::invalid_argument("Receiver cannot be null");
}

void CompleteWorkCommand::execute(){
    receiver->complete();
}

void CompleteWorkCommand::undo(){
  throw std::logic_error("Cannot undo a completed item.");
}

std::string CompleteWorkCommand::getDescription() const{
    return "Complete Work Item";
}



//Reject Command
RejectCommand::RejectCommand( WorkItem* item, const std::string& reason)
                : receiver(item), reason(reason) {
    if (!receiver) throw std::invalid_argument("Receiver cannot be null");
}

void RejectCommand::execute(){
    receiver->reject(reason);
}

void RejectCommand::undo() {
  throw std::logic_error("Cannot undo Reject command!");
}

std::string RejectCommand::getDescription() const{
    return "Reject Work Item";
}



//Escalate Command
EscalateCommand::EscalateCommand( WorkItem* item)
                : receiver(item) {
    if (!receiver) throw std::invalid_argument("Receiver cannot be null");
}

void EscalateCommand::execute(){
    receiver->escalate();
}

void EscalateCommand::undo(){
  throw std::logic_error("Cannot de-escalate!");
}

std::string EscalateCommand::getDescription() const{
    return "Escalate Work Item";
}
