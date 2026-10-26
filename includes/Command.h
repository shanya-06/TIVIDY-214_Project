#ifndef COMMAND_H
#define COMMAND_H
#include <string>

class Command {
public:
    virtual void execute() =0;
    virtual void undo() {};

    virtual ~Command() = default;
}; 


class AssignCommand : public Command
{
private:
    WorkItem* receiver;
    WorkflowParticipant* participant;

public:
    AssignCommand(
        WorkItem* item,
        WorkflowParticipant* participant);

    void execute() override;
    void undo() override;

    std::string getDescription() const override;
};


class CompleteWorkCommand : public Command
{
private:
    WorkItem* receiver;
    Map result;

public:
    CompleteWorkCommand(
        WorkItem* item,
        const Map& result);

    void execute() override;
    void undo() override;

    std::string getDescription() const override;
};



class RejectCommand : public Command
{
private:
    WorkItem* receiver;
    std::string reason;

public:
    RejectCommand(
        WorkItem* item,
        const std::string& reason);

    void execute() override;
    void undo() override;

    std::string getDescription() const override;
};



class EscalateCommand : public Command
{
private:
    WorkItem* receiver;

public:
    EscalateCommand(
        WorkItem* item);

    void execute() override;
    void undo() override;

    std::string getDescription() const override;
};


#endif //COMMAND_H