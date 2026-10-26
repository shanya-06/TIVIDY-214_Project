#include "../includes/Command.h"
class WorkItem;
class WorkflowParticipant;
class AssignCommand : public Command
{
private:
    WorkItem* item;
    WorkflowParticipant* participant;

public:
    AssignCommand(
        WorkItem* item,
        WorkflowParticipant* participant);

    void execute() override{
        item->assign(participant);
    }
    void undo() override{
        item->makeAvailable();
    }
};



class CompleteWorkCommand : public Command
{
private:
    WorkItem* item;
    std::string result;

public:
    CompleteWorkCommand(
        WorkItem* item,
        const std::string& result);

    void execute() override{
        item->complete(result);
    }
    void undo() override{
        item->setState(new InProgressState());
    }
};



void RejectCommand::execute()
{
    item->reject(reason);
}

void RejectCommand::undo()
{
    item->setState(
        new InProgressState());
}

void EscalateCommand::execute()
{
    item->escalate();
}


void EscalateCommand::undo()
{
    item->setState(
        new AssignedState());
}