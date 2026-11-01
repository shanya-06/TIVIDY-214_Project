#ifndef COMMAND_H
#define COMMAND_H

#include <string>

class WorkItem;
class WorkflowParticipant;

/**
 * @class Command
 * @brief Abstract command interface for the Command design pattern.
 *
 * Encapsulates workflow operations as objects so they can be
 * executed independently from the object requesting them.
 *
 * Gang of Four Pattern:
 * - Command -> Abstract Command
 */
class Command {
public:
    /**
     * @brief Executes the encapsulated workflow action.
     */
    virtual void execute() = 0;

    /**
     * @brief Attempts to undo a previously executed command.
     *
     * The base implementation does nothing.
     * Concrete commands throw std::logic_error because
     * their workflow operations cannot be undone.
     */
    virtual void undo() {};

    /**
     * @brief Returns a description of the command.
     * @return Human-readable command description.
     */
    virtual std::string getDescription() const = 0;

    /** @brief Virtual destructor for the command interface. */
    virtual ~Command() = default;
};

/**
 * @class AssignCommand
 * @brief Command that assigns a participant to a work item.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
class AssignCommand : public Command {
private:
    WorkItem* receiver;
    WorkflowParticipant* participant;

public:
    /**
     * @brief Constructs an assignment command.
     * @param item Work item to assign.
     * @param participant Participant receiving the assignment.
     * @throws std::invalid_argument If item is null.
     */
    AssignCommand(WorkItem* item, WorkflowParticipant* participant);

    /** @brief Assigns the participant to the work item. */
    void execute() override;

    /**
     * @brief Indicates that assignment cannot be undone.
     * @throws std::logic_error Always.
     */
    void undo() override;

    /**
     * @brief Returns the assignment command description.
     * @return "Assign Work Item".
     */
    std::string getDescription() const override;
};

/**
 * @class CompleteWorkCommand
 * @brief Command that completes a work item.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
class CompleteWorkCommand : public Command {
private:
    WorkItem* receiver;

public:
    /**
     * @brief Constructs a completion command.
     * @param item Work item to complete.
     * @throws std::invalid_argument If item is null.
     */
    explicit CompleteWorkCommand(WorkItem* item);

    /** @brief Completes the work item. */
    void execute() override;

    /**
     * @brief Indicates that completion cannot be undone.
     * @throws std::logic_error Always.
     */
    void undo() override;

    /**
     * @brief Returns the completion command description.
     * @return "Complete Work Item".
     */
    std::string getDescription() const override;
};

/**
 * @class RejectCommand
 * @brief Command that rejects a work item.
 *
 * Stores a rejection reason and passes it to the receiver.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
class RejectCommand : public Command {
private:
    WorkItem* receiver;
    std::string reason;

public:
    /**
     * @brief Constructs a rejection command.
     * @param item Work item to reject.
     * @param reason Reason for rejecting the work item.
     * @throws std::invalid_argument If item is null.
     */
    RejectCommand(WorkItem* item, const std::string& reason);

    /** @brief Rejects the work item with the given reason. */
    void execute() override;

    /**
     * @brief Indicates that rejection cannot be undone.
     * @throws std::logic_error Always.
     */
    void undo() override;

    /**
     * @brief Returns the rejection command description.
     * @return "Reject Work Item".
     */
    std::string getDescription() const override;
};

/**
 * @class EscalateCommand
 * @brief Command that escalates a work item.
 *
 * Gang of Four Pattern:
 * - Command -> Concrete Command
 */
class EscalateCommand : public Command {
private:
    WorkItem* receiver;

public:
    /**
     * @brief Constructs an escalation command.
     * @param item Work item to escalate.
     * @throws std::invalid_argument If item is null.
     */
    EscalateCommand(WorkItem* item);

    /** @brief Escalates the work item. */
    void execute() override;

    /**
     * @brief Indicates that escalation cannot be undone.
     * @throws std::logic_error Always.
     */
    void undo() override;

    /**
     * @brief Returns the escalation command description.
     * @return "Escalate Work Item".
     */
    std::string getDescription() const override;
};

#endif
