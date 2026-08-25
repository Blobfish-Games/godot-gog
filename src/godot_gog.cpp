#include "godot_gog.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

bool init_succeeded = false;
bool send_init_finished_signal = false;
bool send_overlay_toggled = false;
bool signedIn = false;
bool overlay_active = false;

GOG::GOG() {
}

GOG::~GOG() {
    delete(operationalStateListener);
    delete(authListener);
    delete(statsRetrieveListener);
    delete(overlayStateListener);

    galaxy::api::Shutdown();
}

void GOG::initialize(String client_id, String client_secret) {
    // ensure the returned CharString from utf8() lives until the Init() call is executed
    CharString cs_client_id = client_id.utf8();
    CharString cs_client_secret = client_secret.utf8();
    galaxy::api::InitOptions initOptions = {cs_client_id.get_data(), cs_client_secret.get_data()};
    galaxy::api::Init(initOptions);
    LogAPIError();
    print_line("GOG: Initializing");

    operationalStateListener = new OperationalStateListener();
    authListener = new AuthListener();
    statsRetrieveListener = new StatsRetrieveListener();
    overlayStateListener = new OverlayStateListener();

    galaxy::api::User()->SignInGalaxy();
    LogAPIError();
    print_line("GOG: SignIn started");
}

void GOG::_process(double delta) {
    if (send_init_finished_signal) {
        emit_signal("init_finished", init_succeeded);
        send_init_finished_signal = false;
    }
    if (send_overlay_toggled) {
        emit_signal("overlay_toggled", overlay_active);
        send_overlay_toggled = false;
    }
    galaxy::api::ProcessData();
}

bool GOG::is_overlay_enabled() {
    return overlay_active;
}

void GOG::unlock_achievement(String achId) {
    if(signedIn) {
        galaxy::api::Stats()->SetAchievement(achId.utf8());
        LogAPIError();
        galaxy::api::Stats()->StoreStatsAndAchievements();
        LogAPIError();
        print_line("GOG: Achievement unlocked: " + achId);
    }
}

bool GOG::is_achievement_unlocked(String achId) {
    if(signedIn) {
        bool unlocked;
        uint32_t unlock_time;
        galaxy::api::Stats()->GetAchievement(achId.utf8(), unlocked, unlock_time);
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
        print_line("GOG: " + String(error->GetName()) + ": " + String(error->GetMsg()));
    }
}

// Listeners
void GOG::AuthListener::OnAuthSuccess() {
    galaxy::api::Stats()->RequestUserStatsAndAchievements();
    print_line("GOG: Auth success");
}

void GOG::AuthListener::OnAuthFailure(FailureReason failureReason) {
    init_succeeded = false;
    send_init_finished_signal = true;
    print_line("GOG: Auth failed. Error: " + String().num(failureReason));
}

void GOG::AuthListener::OnAuthLost() {
    print_line("GOG: Auth lost");
}

void GOG::OperationalStateListener::OnOperationalStateChanged(uint32_t operationalState) {
    signedIn = galaxy::api::User()->SignedIn();
    if(signedIn == false) {
        print_line("GOG: Connection lost: signedIn is false");
    }
}

void GOG::StatsRetrieveListener::OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID) {
    init_succeeded = true;
    send_init_finished_signal = true;
    print_line("GOG: Achievements fetched. Init successful");
}

void GOG::StatsRetrieveListener::OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, FailureReason failureReason) {
    print_line("GOG: Achievement fetch failed" + String().num(failureReason));
}

void GOG::OverlayStateListener::OnOverlayVisibilityChanged(bool overlayVisible) {
    overlay_active = overlayVisible;
    send_overlay_toggled = true;
    print_line("GOG: Overlay visibility changed to: " + String(overlayVisible ? "true" : "false"));
}

void GOG::_bind_methods() {
    ClassDB::bind_method(D_METHOD("initialize", "client_id", "client_secret"), &GOG::initialize);
    ClassDB::bind_method(D_METHOD("is_overlay_enabled"), &GOG::is_overlay_enabled);
    ClassDB::bind_method(D_METHOD("unlock_achievement", "ach_id"), &GOG::unlock_achievement);
    ClassDB::bind_method(D_METHOD("is_achievement_unlocked", "ach_id"), &GOG::is_achievement_unlocked);
    ClassDB::bind_method(D_METHOD("reset_stats_and_achievements"), &GOG::reset_stats_and_achievements);
    ADD_SIGNAL(MethodInfo("init_finished", PropertyInfo(Variant::BOOL, "success")));
    ADD_SIGNAL(MethodInfo("overlay_toggled", PropertyInfo(Variant::BOOL, "active")));
}
