#include "pch.h"
#include "InstantWin.h"

namespace cheat::features
{
    InstantWin::InstantWin()
        : FeatureBase("Instant Win", "Instantly wins battles",
                      FeatureSection::Game)
    {
        // Alternatively, hook Begin(). But the user can just enable the feature anytime during the battle.
        HookManager::install(Battle::Update(), hBattle_Update);
    }

    void InstantWin::hBattle_Update(Battle* _this)
    {
        if (s_instance->isEnabled())
        {
            _this->BattleSummary_()->Winner(GroupTag_Enum::Group01);
            _this->BattleSummary_()->EndType(BattleEndType_Enum::Clear);
            _this->BattleSummary_()->EndFrame(0);
            _this->BattleSummary_()->ElapsedRealtime(0.f);

            UNITY_CALL(Battle::Finalize, _this, BattleEndType_Enum::Clear)
        }

        CALL_ORIGINAL(hBattle_Update, _this);
    }
}
