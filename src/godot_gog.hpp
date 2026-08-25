#ifndef GODOT_GOG_H
#define GODOT_GOG_H

#include <Godot.hpp>
#include <Node.hpp>

#include <galaxy/GalaxyApi.h>

namespace godot {
    class GOG : public Node {
        GODOT_CLASS(GOG, Node)

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

            OperationalStateListener *operationalStateListener = nullptr;
            AuthListener *authListener = nullptr;
            StatsRetrieveListener *statsRetrieveListener = nullptr;

            void LogAPIError();

        public:
            static void _register_methods();

            GOG();
            ~GOG();

            void _init();  // required method
            void _process(float delta);
            void initialize(String client_id, String client_secret);
            void unlock_achievement(String ach_id);
            bool is_achievement_unlocked(String ach_id);
            void reset_stats_and_achievements();
    };
}

#endif
