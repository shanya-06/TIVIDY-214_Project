#ifndef DEFAULTWORKITEMPROCESSOR_H
#define DEFAULTWORKITEMPROCESSOR_H

#include "WorkItemProcessor.h"

/**
 * @brief Standard implementation of the WorkItemProcessor template.
 *
 * Provides the default validation, execution, and routing steps
 * for processing a work item.
 */
class DefaultWorkItemProcessor : public WorkItemProcessor {
public:
    /** @brief Constructs a default work item processor. */
    DefaultWorkItemProcessor();

    /** @brief Destroys the default work item processor. */
    ~DefaultWorkItemProcessor() override;

protected:
    /** @brief Validates the work item using default rules. */
    void validate(WorkItem* item) override;

    /** @brief Executes the work item using default behavior. */
    void execute(WorkItem* item) override;

    /** @brief Routes the work item using default behavior. */
    void route(WorkItem* item) override;
};

#endif