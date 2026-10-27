#ifndef WFSTATUS_H
#define WFSTATUS_H

/**
 * @enum WFStatus
 * @brief Represents the overall execution states of a workflow instance object.
 */
enum WFStatus
{
    CREATED,
    RUNNING,
    COMPLETED,
    CANCELLED,
    FAILED
};

#endif // WFSTATUS_H
