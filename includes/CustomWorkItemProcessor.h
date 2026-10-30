#ifndef CUSTOMWORKITEMPROCESSOR_H
#define CUSTOMWORKITEMPROCESSOR_H

#include "WorkItemProcessor.h"

/**
 * @brief Template Method processor with custom workflow behavior.
 *
 * Overrides the primitive operations defined by WorkItemProcessor
 * while preserving the standard processing sequence.
 */
class CustomWorkItemProcessor : public WorkItemProcessor {
public:
    /** @brief Constructs a custom work item processor. */
    CustomWorkItemProcessor();

    /** @brief Destroys the custom work item processor. */
    ~CustomWorkItemProcessor() override;

protected:
    /** @brief Performs custom validation on a work item. */
    void validate(WorkItem* item) override;

    /** @brief Performs custom execution on a work item. */
    void execute(WorkItem* item) override;

    /** @brief Performs custom routing on a work item. */
    void route(WorkItem* item) override;
};

#endif