/**
 * @file main.cpp
 * @brief Scripted demo: two industries on the SAME generic engine, driven ONLY through
 * the WorkflowFacade (the same calls the GUI uses).
 *
 * The engine (includes/, src/) knows nothing about insurance or admissions; those live in
 * app/Industries.cpp. This file never touches engine objects, only the Facade's plain data.
 */
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Industries.h"
#include "WorkflowFacade.h"

namespace {

typedef WorkflowFacade F;

void banner(const std::string& text) {
    std::cout << "\n==============================================================\n"
              << text << "\n==============================================================\n";
}

void section(const std::string& text) { std::cout << "\n--- " << text << " ---\n"; }

void show(F& f, const std::string& key, const std::string& title) {
    section(title);
    F::InstanceInfo info = f.instance(key);
    std::cout << "  " << key << ": " << info.status << " (" << info.completed << "/" << info.total << " done)\n";
    std::vector<F::ItemInfo> items = f.items(key);
    for (std::size_t i = 0; i < items.size(); ++i)
        std::cout << "  " << items[i].state << "\t" << items[i].id << " " << items[i].name
                  << "\t(" << (items[i].assignee.empty() ? "-" : items[i].assignee) << ")\n";
    std::vector<F::ParticipantInfo> people = f.participants(key);
    for (std::size_t i = 0; i < people.size(); ++i)
        std::cout << "  " << people[i].name << ": workload " << people[i].workload
                  << (people[i].available ? "" : " (unavailable)") << "\n";
}

void history(F& f, const std::string& key) {
    section("History for " + key);
    std::vector<std::string> c = f.commands(key), e = f.events(key);
    for (std::size_t i = 0; i < c.size(); ++i) std::cout << "  command " << i + 1 << ": " << c[i] << "\n";
    for (std::size_t i = 0; i < e.size(); ++i) std::cout << "  event " << i + 1 << ": " << e[i] << "\n";
}


void work(F& f, const std::string& key, const std::string& id) {
    F::ItemInfo item = f.item(key, id);
    std::cout << "  " << item.assignee << " works on " << id << " '" << item.name << "'\n";
    f.startWork(key, id);
    if (f.item(key, id).inProgress) f.completeWork(key, id);   
}


void finish(F& f, const std::string& key) {
    for (int guard = 0; guard < 20; ++guard) {
        std::vector<F::ItemInfo> todo = f.availableWork(key);
        std::size_t i = 0;
        while (i < todo.size() && todo[i].assignee.empty()) ++i;
        if (i == todo.size()) return;
        work(f, key, todo[i].id);
    }
}

void expectRefusal(const std::string& what, const std::function<void()>& action) {
    try {
        action();
        std::cout << "  !! " << what << " was NOT refused\n";
    } catch (const std::exception& e) {
        std::cout << "  [refused] " << what << " -> " << e.what() << "\n";
    }
}

F::Params claim(const std::string& number, const std::string& amount, const std::string& bank) {
    F::Params p;
    p["claimNumber"] = number; p["amount"] = amount; p["bank"] = bank;
    return p;
}

F::Params application(const std::string& student, const std::string& average) {
    F::Params p;
    p["student"] = student; p["average"] = average;
    return p;
}

void insuranceNormal(F& f) {
    banner("INSURANCE | completion path: small claim approved and paid");
    std::string k = f.startWorkflow("INS-CLAIM", claim("CL-1001", "8000", "valid"));
    show(f, k, "Current work after start");
    section("Invalid actions are refused");
    expectRefusal("completing T1 before it was started", [&] { f.completeWork(k, "T1"); });
    expectRefusal("unknown item", [&] { f.startWork(k, "ZZ"); });
    finish(f, k);
    show(f, k, "Final state");
    history(f, k);
}

void insuranceEscalation(F& f) {
    banner("INSURANCE | exception path: assessor unavailable, escalated and reassigned; large claim re-routed");
    std::string k = f.startWorkflow("INS-CLAIM", claim("CL-1002", "35000", "valid"));
    work(f, k, "T1");
    show(f, k, "Assessment assigned by the Strategy");
    std::string who = f.item(k, "T2").assignee;
    std::cout << "\n  " << who << " goes on leave\n";
    f.setAvailable(k, who.substr(0, 3) == "Ann" ? "u2" : "u3", false);
    show(f, k, "After automatic escalation and reassignment");
    finish(f, k);
    show(f, k, "Final state");
    history(f, k);
}

void insuranceManual(F& f) {
    banner("INSURANCE | supervisor moves work by hand (Commands); claim declined by the manager");
    std::string k = f.startWorkflow("INS-CLAIM", claim("CL-1003", "250000", "valid"));
    work(f, k, "T1");
    f.escalateWork(k, "T2");
    f.assignWork(k, "T2", "u3");
    show(f, k, "After manual reassignment to Ben");
    finish(f, k);
    show(f, k, "Final state");
    history(f, k);
}

void insurancePaymentFails(F& f) {
    banner("INSURANCE | exception path: external payment service fails");
    std::string k = f.startWorkflow("INS-CLAIM", claim("CL-1004", "20000", "missing"));
    finish(f, k);
    show(f, k, "Final state");
    history(f, k);
}

void insuranceCancel(F& f) {
    banner("INSURANCE | exception path: policyholder cancels the claim");
    std::string k = f.startWorkflow("INS-CLAIM", claim("CL-1005", "8000", "valid"));
    work(f, k, "T1");
    f.cancel(k);
    expectRefusal("cancelling again", [&] { f.cancel(k); });
    expectRefusal("working on a cancelled instance", [&] { f.startWork(k, "T2"); });
    show(f, k, "After cancellation");
    history(f, k);
}

void rejectValidation(F& f, const std::string& key) {
    std::cout << "  " << f.item(key, "V1").assignee << " finds documents missing\n";
    f.startWork(key, "V1");
    f.rejectWork(key, "V1", "supporting documents missing");
}

void admissions(F& f, const std::string& story, const F::Params& p, bool documentsMissing = false) {
    banner("ADMISSIONS | " + story);
    std::string k = f.startWorkflow("EDU-APPLICATION", p);
    if (documentsMissing) rejectValidation(f, k);
    finish(f, k);
    show(f, k, "Final state");
    history(f, k);
}

void isolation(F& f) {
    banner("ADMISSIONS | isolation: two applications interleaved");
    std::string a = f.startWorkflow("EDU-APPLICATION", application("Ben", "66"));
    std::string b = f.startWorkflow("EDU-APPLICATION", application("Cara", "90"));
    rejectValidation(f, b);
    std::cout << "  " << b << " failed; " << a << " carries on unaffected\n";
    finish(f, a);
    finish(f, b);
    show(f, a, "Ben's instance");
    show(f, b, "Cara's instance");
}

} 

int main() {
    F facade;
    registerIndustries(facade);

    banner("TIVIDY demo: one generic engine, two industries, one Facade");
    std::vector<F::WorkflowInfo> list = facade.workflows();
    for (std::size_t i = 0; i < list.size(); ++i)
        std::cout << "  " << list[i].id << " - " << list[i].name << "\n    " << list[i].description << "\n";

    try {
        insuranceNormal(facade);
        insuranceEscalation(facade);
        insuranceManual(facade);
        insurancePaymentFails(facade);
        insuranceCancel(facade);
        admissions(facade, "completion path: strong application accepted", application("Ana", "82"));
        admissions(facade, "exception path: declined by the committee", application("Dan", "41"));
        admissions(facade, "exception path: documents missing, validation rejected", application("Eli", "70"), true);
        isolation(facade);
    } catch (const std::exception& e) {
        std::cout << "demo stopped: " << e.what() << "\n";
        return 1;
    }

    banner("Demo finished");
    std::vector<F::InstanceInfo> all = facade.instances();
    for (std::size_t i = 0; i < all.size(); ++i)
        std::cout << "  " << all[i].key << ": " << all[i].status << "\n";
    return 0;
}
