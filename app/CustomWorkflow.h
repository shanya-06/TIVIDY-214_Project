#ifndef CUSTOMWORKFLOW_H
#define CUSTOMWORKFLOW_H

/**
 * @file CustomWorkflow.h
 * @brief Workflows designed by the user at run time (from the GUI's line planner).
 *
 * A design is plain data (JSON): input fields, participants, work items with
 * optional decorators, the assignment strategy, the processor, approval limits
 * and routing rules. It is validated, then registered with the WorkflowFacade
 * exactly like the built-in industries; a ConfigurableBuilder (a concrete
 * Builder) turns the design into an independent instance each time one starts.
 * The engine is not changed in any way to support custom workflows.
 */

#include <string>
#include <vector>

#include "Json.h"

class WorkflowFacade;

/**
 * @class CustomWorkflowLibrary
 * @brief Keeps the user's workflow designs, registers them with the Facade and
 * saves them to a file so they survive a restart.
 */
class CustomWorkflowLibrary {
public:
    /** @param file where designs are saved (empty = keep them in memory only). */
    CustomWorkflowLibrary(WorkflowFacade& facade, const std::string& file);

    /** @brief Registers every design saved in the file. Returns warnings for designs that could not be loaded. */
    std::vector<std::string> loadSaved();

    /**
     * @brief Validates a design, registers it and saves it.
     * @return the stored design with generated IDs filled in.
     * @throws std::invalid_argument explaining what is wrong with the design.
     */
    Json add(const Json& design);

    /** @brief Removes a custom workflow; running instances of it carry on. */
    void remove(const std::string& workflowId);

    bool isCustom(const std::string& workflowId) const;
    Json design(const std::string& workflowId) const;   ///< null if not custom
    std::vector<Json> designs() const { return saved; }

    /** @brief Checks a design and fills in generated IDs without registering it. */
    Json validate(const Json& design) const;

private:
    void registerDesign(const Json& normalised);
    void save() const;

    WorkflowFacade& facade;
    std::string file;
    std::vector<Json> saved;
};

#endif
