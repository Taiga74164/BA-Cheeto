#pragma once
#include "user/cheat/feature_base.h"
#include "utils/config_manager.h"
#include "utils/config_field.h"

struct BasisPoint;

namespace cheat::features
{
    class PlayerStats : public FeatureBase
    {
        DECL_SINGLETON(PlayerStats)

    public:
        PlayerStats();

    void init() override;
    void reloadFromConfig();
        void draw() override;

    private:
    std::unordered_map<StatType_Enum, int> m_statValues;
    std::unordered_map<StatType_Enum, Config::Field<int>> m_statFields;
        std::string m_searchFilter;
    void saveStatsToConfig();

        std::vector<StatType_Enum> getFilteredStats() const;
        void applyStats(BattleEntityStat* entityStat, BattleEntity* battleEntity) const;

        static void hNewNormalAttackAction_Update(NewNormalAttackAction* _this, Battle* battle);
    };
}
