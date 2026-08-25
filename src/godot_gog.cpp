#include "godot_gog.hpp"

using namespace godot;

bool init_succeeded = false;
bool send_init_finished_signal = false;
bool signedIn = false;

void GOG::_register_methods() {
	register_method("_process", &GOG::_process);
    register_method("initialize", &GOG::initialize);
    register_method("unlock_achievement", &GOG::unlock_achievement);
    register_method("is_achievement_unlocked", &GOG::is_achievement_unlocked);
    register_method("reset_stats_and_achievements", &GOG::reset_stats_and_achievements);
    register_signal<GOG>((char *)"init_finished", "success", GODOT_VARIANT_TYPE_BOOL);
}

GOG::GOG() {}

GOG::~GOG() {
    delete(operationalStateListener);
    delete(authListener);
    delete(statsRetrieveListener);

    galaxy::api::Shutdown();
}

void GOG::_init() {
}

void GOG::_process(float delta) {
    if (send_init_finished_signal) {
        emit_signal("init_finished", init_succeeded);
        send_init_finished_signal = false;
    }
    galaxy::api::ProcessData();
}

void GOG::initialize(String client_id, String client_secret) {
    galaxy::api::InitOptions initOptions = {client_id.alloc_c_string(), client_secret.alloc_c_string()};
    galaxy::api::Init(initOptions);
    LogAPIError();

    operationalStateListener = new OperationalStateListener();
    authListener = new AuthListener();
    statsRetrieveListener = new StatsRetrieveListener();

    galaxy::api::User()->SignInGalaxy();
    LogAPIError();
}

void GOG::unlock_achievement(String ach_id) {
    if(signedIn) {
        galaxy::api::Stats()->SetAchievement(ach_id.alloc_c_string());
        LogAPIError();
        galaxy::api::Stats()->StoreStatsAndAchievements();
        LogAPIError();
        Godot::print("GOG: Achievement unlocked: " + ach_id);
    }
}

bool GOG::is_achievement_unlocked(String ach_id) {
    if(signedIn) {
        bool unlocked;
        uint32_t unlock_time;
        galaxy::api::Stats()->GetAchievement(ach_id.alloc_c_string(), unlocked, unlock_time);
        LogAPIError();
        return unlocked;
    }
    return false;
}

// only use to debug
void GOG::reset_stats_and_achievements() {
    if(signedIn) {
        galaxy::api::Stats()->ResetStatsAndAchievements();
    }
}

// helpers
void GOG::LogAPIError() {
    const galaxy::api::IError *error = galaxy::api::GetError();
    if(error != NULL) {
        Godot::print("GOG: " + String(error->GetName()) + ": " + String(error->GetMsg()));
    }
}

// Listeners
void GOG::AuthListener::OnAuthSuccess() {
    galaxy::api::Stats()->RequestUserStatsAndAchievements();
    Godot::print("GOG: Auth success");
}

void GOG::AuthListener::OnAuthFailure(FailureReason failureReason) {
    init_succeeded = false;
    send_init_finished_signal = true;
    Godot::print("GOG: Auth failed. Error: " + String().num(failureReason));
}

void GOG::AuthListener::OnAuthLost() {
    Godot::print("GOG: Auth lost");
}

void GOG::OperationalStateListener::OnOperationalStateChanged(uint32_t operationalState) {
    signedIn = galaxy::api::User()->SignedIn();
    if(signedIn == false) {
        Godot::print("GOG: Connection lost: signedIn is false");
    }
}

void GOG::StatsRetrieveListener::OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID) {
    init_succeeded = true;
    send_init_finished_signal = true;
    Godot::print("GOG: Achievements fetched. Init successful");
}

void GOG::StatsRetrieveListener::OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, FailureReason failureReason) {
    Godot::print("GOG: Achievement fetch failed" + String(failureReason));
}
