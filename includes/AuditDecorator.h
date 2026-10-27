#ifndef AUDITDECORATOR_H
#define AUDITDECORATOR_H

#include <string>
#include <vector>

#include "WorkItemDecorator.h"

/** @brief Records every lifecycle action (and whether it succeeded) in an audit log. */
class AuditDecorator : public WorkItemDecorator {
public:
    /**
     * @brief Wraps an item with an empty audit log.
     * @param component Item to wrap. Ownership transfers to the decorator.
     * @throws std::invalid_argument if component is null.
     */
    explicit AuditDecorator(WorkItem* component);

    /** @brief A copy of the log, oldest entry first, e.g. "start: ok", "complete: FAILED". */
    std::vector<std::string> getAuditLog() const;

    /** @brief Assigns the item and logs "assign to <name>: ok" or ": FAILED". */
    virtual void assign(WorkflowParticipant* participant) override;
    /** @brief Starts the item and logs the outcome. */
    virtual void start() override;
    /** @brief Completes the item and logs the outcome. */
    virtual void complete() override;
    /** @brief Approves the item and logs the outcome. */
    virtual void approve() override;
    /** @brief Rejects the item and logs the outcome together with the reason. */
    virtual void reject(const std::string& reason) override;
    /** @brief Escalates the item and logs the outcome. */
    virtual void escalate() override;
 
private:
    /** @brief Appends one entry to the log. */
    void record(const std::string& entry);
 
    std::vector<std::string> log;
};

#endif
