#ifndef NOTIFICATIONDECORATOR_H
#define NOTIFICATIONDECORATOR_H

#include "WorkItemDecorator.h"

/** @brief Sends a notification (printed to stdout) after each successful lifecycle action. */
class NotificationDecorator : public WorkItemDecorator {
public:
    NotificationDecorator(WorkItem* component, const std::string& recipient);

    virtual void assign(WorkflowParticipant* participant) override;
    virtual void start() override;
    virtual void complete(const Map& result) override;
    virtual void reject(const std::string& reason) override;
    virtual void escalate() override;

private:
    void notify(const std::string& message) const;
    std::string recipient;
};

#endif
