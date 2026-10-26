#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include "Types.h"

class WorkItem;
class WorkflowParticipant;

/** 
 * @class Command
 * @brief Abstract command interface for the Command design pattern.
 * 
 * Encapsulates a request as an object so that workflow operations
 * can be executed independently from the object that performs them.
 *
 * Concrete commands store a reference to a WorkItem receiver and
 * invoke the appropriate operation on that receiver when execute()
 * is called.
 * Gang of Four pattern: 
 * - Command-> Abstract Command class
 *
**/

class Command {
public:
    /**
     * @brief Executes the encapsulated workflow action.
     **/
    virtual void execute() =0;


    /**
    * @brief Reverses a previously executed command.
    **/
    virtual void undo() {};

    /**
    * @brief Returns a human-readable description of the command.
    **/
    virtual std::string getDescription() const = 0;

    virtual ~Command() = default;
}; 

/**
 * @class AssignCommand
 * @brief Command that assigns a participant to a work item.
 *
 * Stores all information required to perform an assignment
 * operation on a WorkItem receiver.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
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

/**
 * @class CompleteWorkCommand
 * @brief Command that completes a work item.
 *
 * Stores completion result data and submits it to the
 * receiver when executed.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
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



/**
 * @class RejectCommand
 * @brief Command that rejects a work item.
 *
 * Stores a rejection reason and passes it to the receiver
 * when executed.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
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


/**
 * @class EscalateCommand
 * @brief Command that escalates a work item.
 *
 * Requests escalation of the receiver work item through
 * the workflow system.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
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