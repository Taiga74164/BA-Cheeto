﻿#include "pch.h"
#include "InstantWin.h"

namespace cheat::features
{
    InstantWin::InstantWin()
        : FeatureBase("Instant Win", "Instantly wins battles",
                      FeatureSection::Game)
    {
        // Alternatively, hook Begin(). But the user can just enable the feature anytime during the battle.
        Battle::Update_Hook().set(Battle_Update_Hook);
    }

    void InstantWin::Battle_Update_Hook(Battle* _this)
    {
        if (s_instance->isEnabled())
        {
            _this->BattleSummary_()->Winner(GroupTag_Enum::Group01);
            _this->BattleSummary_()->EndType(BattleEndType_Enum::Clear);
            _this->BattleSummary_()->EndFrame(0);
            _this->BattleSummary_()->ElapsedRealtime(0.f);

            // auto nowTicks = DateTime::get_Now()().get_Ticks()();
            // auto deltaTicks = nowTicks - _this->StartTickRealTime();
            // double seconds = deltaTicks / 10'000'000.0; //
            // int minutes = static_cast<int>(seconds / 60);
            // int secondsPart = static_cast<int>(seconds) % 60;
            // LOG_DEBUG("In-game time: %02d:%02d", minutes, secondsPart);

            UNITY_CALL(Battle::set_GameState, _this, BattleLogicState_Enum::Finished)
        }

        Battle::Update_Hook().call(_this);
    }
}
