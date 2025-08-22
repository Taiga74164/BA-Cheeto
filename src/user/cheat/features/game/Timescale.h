#pragma once
#include "user/cheat/feature_base.h"

namespace cheat::features
{
    class Timescale : public FeatureBase<Timescale>
    {
        DECL_SINGLETON(Timescale)

    public:
        Timescale();

        void draw() override;

    private:
        static void BattleGameTime_Tick_Hook(BattleGameTime* _this);

        CONFIG_FIELD(float, m_scaleField, 1.0f);
    };
}
