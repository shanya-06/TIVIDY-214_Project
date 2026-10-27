#ifndef AUDITDECORATOR_H
#define AUDITDECORATOR_H

#include <string>
#include <vector>

#include "WorkItemDecorator.h"

/** @brief Records every lifecycle action (and whether it succeeded) in an audit log. */
class AuditDecorator : public WorkItemDecorator {
public:
    explicit AuditDecorator(WorkItem* component);

    std::vector<std::string> getAuditLog() const;

    virtual void assign(WorkflowParticipant* participant) override;
    virtual void start() override;
    virtual void complete(const Map& result) override;
    virtual void approve() override;
    virtual void reject(const std::string& reason) override;
    virtual void escalate() override;

private:
    void record(const std::string& entry);
    std::vector<std::string> log;
};

#endif
