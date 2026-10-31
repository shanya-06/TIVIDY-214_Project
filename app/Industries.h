#ifndef INDUSTRIES_H
#define INDUSTRIES_H

#include <string>
#include <vector>

#include "WorkflowFacade.h"

/**
 * @brief Registers the two industries (insurance claims, university admissions)
 * with the generic engine's Facade. All industry knowledge lives in app/Industries.cpp.
 */
void registerIndustries(WorkflowFacade& facade);

/**
 * @brief A ready-made set of start parameters ("test case") for one workflow,
 * e.g. a claim large enough to trigger extra routing. Offered by the GUI when
 * starting an instance; the user can always type their own values instead.
 */
struct DemoPreset {
    std::string workflowId;
    std::string label;
    WorkflowFacade::Params params;
};

/** @brief Realistic test cases covering the completion and exception paths of both industries. */
std::vector<DemoPreset> demoPresets();

#endif
