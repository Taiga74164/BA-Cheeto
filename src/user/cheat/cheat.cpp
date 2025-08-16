#include "pch.h"
#include "cheat.h"

#include "feature_manager.h"

#include "features/debug/Debug.h"

#include "features/player/NoCost.h"
#include "features/player/GodMode.h"
#include "features/player/PlayerStats.h"

#include "features/combat/DumbEnemies.h"
#include "features/combat/OneHitKill.h"

#include "features/game/InstantWin.h"
#include "features/game/SkipBattleSummary.h"
#include "features/game/Timescale.h"

#include "features/hooks/BattleEntityHook.h"

void cheat::init()
{
    auto& manager = FeatureManager::getInstance();

    manager.registerFeatures<
        // features::Debug,

        // Cheat features
        features::NoCost,
        features::GodMode,
        features::PlayerStats,
        features::DumbEnemies,
        features::OneHitKill,
        features::InstantWin,
        features::SkipBattleSummary,
        features::Timescale,

        // Hooks
        features::BattleEntityHook
    >();

    const auto fullVersion = Application::get_version()()->ToString();
    const size_t dotPos = fullVersion.rfind('.');
    const auto version = fullVersion.substr(dotPos + 1);
    LOG_INFO("Blue Archive version: %s", version.c_str());
    
    manager.init();
}

void cheat::shutdown()
{
    auto& hookManager = HookManager::getInstance();
    hookManager.blockNewHooks(true);
    hookManager.shutdown();
    LOG_INFO("Hooks shutdown successfully");
}
