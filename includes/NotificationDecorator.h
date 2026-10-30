#ifndef NOTIFICATIONDECORATOR_H
#define NOTIFICATIONDECORATOR_H

#include "WorkItemDecorator.h"

/**
 * @brief Sends a notification after each successful lifecycle action (Concrete Decorator).
 *
 * Notifications are only sent when the action succeeds: if the state rejects the action
 * (throws), nothing is sent. 
 */
class NotificationDecorator : public WorkItemDecorator {
public:
    /**
     * @brief Wraps an item so that its actions notify recipient.
     * @param component Item to wrap. Ownership transfers to the decorator.
     * @param recipient Who the notifications are addressed to.
     * @throws std::invalid_argument if @p component is null.
     */
    NotificationDecorator(WorkItem* component, const std::string& recipient);
 
    /** @brief Assigns the item, then notifies who it was assigned to. */
    virtual void assign(WorkflowParticipant* participant) override;
    /** @brief Starts the item, then notifies. */
    virtual void start() override;
    /** @brief Completes the item, then notifies. */
    virtual void complete() override;
    /** @brief Rejects the item, then notifies with the reason. */
    virtual void reject(const std::string& reason) override;
    /** @brief Escalates the item, then notifies. */
    virtual void escalate() override;
 
private:
    /** @brief Delivers one message to the recipient. */
    void notify(const std::string& message) const;
 
    std::string recipient;
};
 
#endif
