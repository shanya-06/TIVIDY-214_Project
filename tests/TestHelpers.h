#ifndef TESTHELPERS_H
#define TESTHELPERS_H

/**
 * @file TestHelpers.h
 * @brief Helpers for the end-to-end tests that drive the engine through the WorkflowFacade.
 */

#include <string>
#include <vector>

#include "Industries.h"
#include "WorkflowFacade.h"

namespace testhelpers {

typedef WorkflowFacade::Params Params;

/** Builds a parameter map from up to three name/value pairs. */
inline Params params(const std::string& k1, const std::string& v1,
                     const std::string& k2 = "", const std::string& v2 = "",
                     const std::string& k3 = "", const std::string& v3 = "") {
    Params p;
    p[k1] = v1;
    if (!k2.empty()) p[k2] = v2;
    if (!k3.empty()) p[k3] = v3;
    return p;
}

/** The assignee starts the current item and completes it (the GUI's "Do Next Step"). */
inline void nextStep(WorkflowFacade& f, const std::string& key) {
    std::vector<WorkflowFacade::ItemInfo> open = f.availableWork(key);
    if (open.empty()) return;
    const std::string id = open.front().id;
    if (!f.item(key, id).inProgress) f.startWork(key, id);
    WorkflowFacade::ItemInfo now = f.item(key, id);
    if (now.inProgress && !now.awaitingApproval) f.completeWork(key, id);
}

/** Does next steps until the instance is no longer running. */
inline void runToEnd(WorkflowFacade& f, const std::string& key) {
    for (int i = 0; i < 30 && f.instance(key).status == "RUNNING"; ++i) nextStep(f, key);
}

inline std::vector<std::string> itemIds(WorkflowFacade& f, const std::string& key) {
    std::vector<std::string> ids;
    std::vector<WorkflowFacade::ItemInfo> items = f.items(key);
    for (std::size_t i = 0; i < items.size(); ++i) ids.push_back(items[i].id);
    return ids;
}

inline bool contains(const std::vector<std::string>& list, const std::string& value) {
    for (std::size_t i = 0; i < list.size(); ++i)
        if (list[i] == value) return true;
    return false;
}

/** A facade with the three built-in industries registered and the engine's console output switched off. */
struct EngineFixture {
    WorkflowFacade facade;
    EngineFixture() {
        facade.showLiveMessages(false);
        registerIndustries(facade);
    }
};

}  // namespace testhelpers

#endif
