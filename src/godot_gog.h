#ifndef GODOT_GOG_H
#define GODOT_GOG_H

#include <godot_cpp/classes/node.hpp>
#include <galaxy/GalaxyApi.h>

using namespace godot;

class GOG : public Node {
    GDCLASS(GOG, Node)

    private:
        class AuthListener : public galaxy::api::GlobalAuthListener {
        public:
            virtual void OnAuthSuccess() override;
            virtual void OnAuthFailure(FailureReason failureReason) override;
            virtual void OnAuthLost() override;
        };

        class OperationalStateListener : public galaxy::api::GlobalOperationalStateChangeListener {
        public:
            virtual void OnOperationalStateChanged(uint32_t operationalState) override;
        };

        class StatsRetrieveListener : public galaxy::api::GlobalUserStatsAndAchievementsRetrieveListener {
        public:
            virtual void OnUserStatsAndAchievementsRetrieveSuccess(galaxy::api::GalaxyID userID) override;
            virtual void OnUserStatsAndAchievementsRetrieveFailure(galaxy::api::GalaxyID userID, FailureReason failureReason) override;
        };

        class OverlayStateListener : public galaxy::api::GlobalOverlayVisibilityChangeListener {
        public:
            virtual void OnOverlayVisibilityChanged(bool overlayVisible) override;
        };

        OperationalStateListener *operationalStateListener = nullptr;
        AuthListener *authListener = nullptr;
        StatsRetrieveListener *statsRetrieveListener = nullptr;
        OverlayStateListener *overlayStateListener = nullptr;

        void LogAPIError();

    protected:
        static void _bind_methods();
    
    public:
        GOG();
        ~GOG();

        void _process(double delta) override;
        void initialize(String client_id, String client_secret);
        bool is_overlay_enabled();
        void unlock_achievement(String achId);
        bool is_achievement_unlocked(String achId);
        void reset_stats_and_achievements();
};

#endif
