#ifndef PRIORITYDECORATOR_H
#define PRIORITYDECORATOR_H
 
#include "WorkItemDecorator.h"
 
/**
 * @brief Adds a priority level to a work item (Concrete Decorator).
 *
 * Priority is a plain number where a higher number is more urgent. The decorator also
 * prefixes the item's name with its priority (for example "[P2] Assess claim") and
 * raises the priority by one when the item is escalated.
 */
class PriorityDecorator : public WorkItemDecorator {
public:
    /**
     * @brief Wraps an item with a priority.
     * @param component Item to wrap. Ownership transfers to the decorator.
     * @param priority Starting priority (higher means more urgent).
     * @throws std::invalid_argument if component is null.
     */
    PriorityDecorator(WorkItem* component, int priority);
 
    /** @brief Current priority. */
    int getPriority() const;
 
    /** @brief Name of the wrapped item prefixed with "[P<priority>] ". */
    virtual std::string getName() const override;
 
    /**
     * @brief Escalates the wrapped item, then raises the priority by one.
     * @throws Whatever the item's current state throws if escalation is not allowed.
     *         In that case the priority is left unchanged.
     */
    virtual void escalate() override;
 
private:
    int priority;
};
 
#endif
