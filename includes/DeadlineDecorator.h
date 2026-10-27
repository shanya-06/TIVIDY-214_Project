#ifndef DEADLINEDECORATOR_H
#define DEADLINEDECORATOR_H

#include <ctime>

#include "WorkItemDecorator.h"

/** @brief Adds a deadline. Tracks whether the item is overdue and whether it finished late. */
class DeadlineDecorator : public WorkItemDecorator {
public:
    DeadlineDecorator(WorkItem* component, std::time_t deadline);

    std::time_t getDeadline() const;
    /** @brief True if the deadline has passed and the item is not Completed. */
    bool isOverdue() const;
    /** @brief True if complete() succeeded after the deadline. */
    bool wasCompletedLate() const;

    virtual void complete(const Map& result) override;

private:
    std::time_t deadline;
    bool completedLate;
};

#endif
