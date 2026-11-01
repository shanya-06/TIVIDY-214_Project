#ifndef DEADLINEDECORATOR_H
#define DEADLINEDECORATOR_H
 
#include <ctime>
 
#include "WorkItemDecorator.h"
 
/**
 * @brief Adds a deadline to a work item (Concrete Decorator).
 *
 * Reports whether the item is currently overdue and remembers whether it was
 * completed after its deadline. It does not stop or cancel late work.
 */
class DeadlineDecorator : public WorkItemDecorator {
public:
    /**
     * @brief Wraps an item with a deadline.
     * @param component Item to wrap. Ownership transfers to the decorator.
     * @param deadline Latest acceptable completion time (seconds since the epoch).
     * @throws std::invalid_argument if @p component is null.
     */
    DeadlineDecorator(WorkItem* component, std::time_t deadline);
 
    /** @brief The deadline given at construction. */
    std::time_t getDeadline() const;
 
    /** @brief True if the deadline has passed and the item is not yet Completed. */
    bool isOverdue() const;
 
    /** @brief True if complete() succeeded after the deadline had passed. */
    bool wasCompletedLate() const;
 
    /**
     * @brief Completes the wrapped item and records whether this was after the deadline.
     * @throws Whatever the item's current state throws if completion is not allowed.
     *         In that case wasCompletedLate() is left unchanged.
     */
    virtual void complete() override;
 
private:
    std::time_t deadline;
    bool completedLate;
};
 
#endif
 
