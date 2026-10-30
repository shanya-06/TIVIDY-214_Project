#ifndef WORKITEMPROCESSOR_H
#define WORKITEMPROCESSOR_H

class WorkItem;

/**
 * @brief Abstract Template Method processor for WorkItems.
 *
 * The process() function defines the fixed processing sequence:
 *
 * 1. validate()
 * 2. execute()
 * 3. route()
 *
 * Concrete processors customise these steps without changing
 * the overall algorithm.
 */
class WorkItemProcessor
{
public:
    /**
     * @brief Constructs a WorkItem processor.
     */
    WorkItemProcessor();
    /**
     * @brief Virtual destructor for safe polymorphic destruction.
     */
    virtual ~WorkItemProcessor();
    /**
     * @brief Processes a WorkItem using the fixed algorithm.
     *
     * @param item WorkItem to validate, execute, and route.
     *
     * @throws std::invalid_argument if item is nullptr.
     */
    void process(WorkItem* item);

protected:
    /**
     * @brief Validates the WorkItem before processing.
     *
     * @param item WorkItem being validated.
     */
    virtual void validate(WorkItem* item) = 0;
    /**
     * @brief Executes the required work.
     *
     * @param item WorkItem being executed.
     */
    virtual void execute(WorkItem* item) = 0;
    /**
     * @brief Routes the WorkItem after execution.
     *
     * @param item WorkItem being routed.
     */
    virtual void route(WorkItem* item) = 0;
};

#endif