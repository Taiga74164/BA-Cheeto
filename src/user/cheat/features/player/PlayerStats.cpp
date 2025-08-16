#include "pch.h"
#include "PlayerStats.h"

namespace cheat::features
{
    PlayerStats::PlayerStats()
        : FeatureBase("Player Stats", "Modify your character's stats", FeatureSection::Player)
        , m_searchFilter("")
    {
        // Alternatively, you can find a better function to hook as long as it gets called every update
        HookManager::install(NewNormalAttackAction::Update(), hNewNormalAttackAction_Update);
    }

    void PlayerStats::init()
    {
    m_statFields.clear();
    m_statValues.clear();
        // Load persisted stat values using Field wrappers
        for (auto stat = StatType_Enum::MaxHP; stat < StatType_Enum::Max;
             stat = static_cast<StatType_Enum>(static_cast<int>(stat) + 1))
        {
            const std::string key = std::string("stat_") + std::string(magic_enum::enum_name(stat));
            m_statFields.emplace(stat, Config::Field<int>("Player", getName(), key, 0));
            int val = m_statFields[stat].get();
            if (val != 0) m_statValues[stat] = val;
        }
        reloadFromConfig();
    }

    void PlayerStats::draw()
    {
        ImGui::InputText("Search Stats", &m_searchFilter);
        ImGui::SameLine();

        if (ImGui::Button("Reset All"))
        {
            for (auto& pair : m_statValues)
            {
                pair.second = 0;
            }
            saveStatsToConfig();
        }

        if (ImGui::BeginChild("StatsList", ImVec2(0, 340), true))
        {
            for (const auto filteredStats = getFilteredStats(); auto stat : filteredStats)
            {
                const char* statName = magic_enum::enum_name(stat).data();

                if (!m_statValues.contains(stat))
                {
                    m_statValues[stat] = 0;
                }

                int before = m_statValues[stat];
                if (ImGui::InputInt(statName, &m_statValues[stat]))
                {
                    if (m_statValues[stat] != before)
                        m_statFields[stat] = m_statValues[stat];
                }
            }
        }
        ImGui::EndChild();
    }

    std::vector<StatType_Enum> PlayerStats::getFilteredStats() const
    {
        std::vector<StatType_Enum> filtered;
        for (auto stat = StatType_Enum::MaxHP; stat < StatType_Enum::Max;
             stat = static_cast<StatType_Enum>(static_cast<int>(stat) + 1))
        {
            if (m_searchFilter.empty())
            {
                filtered.push_back(stat);
            }
            else
            {
                std::string statName = magic_enum::enum_name(stat).data();
                std::ranges::transform(statName, statName.begin(), tolower);
                std::string searchLower = m_searchFilter;
                std::ranges::transform(searchLower, searchLower.begin(), tolower);

                if (statName.find(searchLower) != std::string::npos)
                {
                    filtered.push_back(stat);
                }
            }
        }
        return filtered;
    }

    void PlayerStats::applyStats(BattleEntityStat* entityStat, BattleEntity* battleEntity) const
    {
        for (const auto& [fst, snd] : m_statValues)
        {
            if (snd != 0)
            {
                BattleEntityStatProcessor::Apply()(battleEntity->statProcessor(), fst);

                BattleEntityStat::SetValue()(entityStat, fst, static_cast<int64_t>(snd));
                auto currentStat = battleEntity->statProcessor()->CurrentStat();
                battleEntity->statProcessor()->InitialStat(currentStat);
                battleEntity->statProcessor()->DefaultStat(currentStat);
                if (fst == StatType_Enum::MaxHP)
                {
                    BattleEntity::SetHitPoint()(battleEntity, snd);
                }
            }
        }
    }

    void PlayerStats::hNewNormalAttackAction_Update(NewNormalAttackAction* _this, Battle* battle)
    {
        if (s_instance->m_enabled)
        {
            if (_this->Executer()->TacticEntityType() == TacticEntityType_Enum::Student)
            {
                // LOG_DEBUG("Executer: {}", _this->Executer()->Name()->ToString().c_str());
                if (const auto entityStat = _this->Executer()->get_CurrentStat()(_this->Executer());
                    entityStat != nullptr)
                {
                    s_instance->applyStats(entityStat, _this->Executer());
                }
            }
        }

        CALL_ORIGINAL(hNewNormalAttackAction_Update, _this, battle);
    }

    void PlayerStats::saveStatsToConfig()
    {
        for (const auto& [stat, value] : m_statValues)
            m_statFields[stat] = value;
    }

    void PlayerStats::reloadFromConfig()
    {
        // Refresh current values from fields (which pull from active profile)
        for (auto stat = StatType_Enum::MaxHP; stat < StatType_Enum::Max;
             stat = static_cast<StatType_Enum>(static_cast<int>(stat) + 1))
        {
            auto itF = m_statFields.find(stat);
            if (itF == m_statFields.end()) continue;
            int v = itF->second.get();
            if (v != 0) m_statValues[stat] = v; else m_statValues.erase(stat);
        }
    }
}
