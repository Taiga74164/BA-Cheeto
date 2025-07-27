#include "pch.h"
#include "FeatureManager.h"

namespace cheat
{
    FeatureManager& FeatureManager::getInstance()
    {
        static FeatureManager instance;
        return instance;
    }

    void FeatureManager::registerFeature(std::unique_ptr<FeatureBase> feature)
    {
        const std::string& nameKey = feature->getNameKey();
        m_featureMap[nameKey] = feature.get();
        m_features.push_back(std::move(feature));
    }

    void FeatureManager::init()
    {
        LOG_INFO("Initializing %zu features", m_features.size());

        for (const auto& feature : m_features)
        {
            try
            {
                feature->init();
                LOG_INFO("Feature '%s' initialized successfully", feature->getName().c_str());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Exception during initialization of feature '%s': %s", feature->getName().c_str(), e.what());
            }
        }
    }

    // TODO: Redesign with sidebar
    void FeatureManager::draw()
    {
        if (ImGui::BeginTabBar("FeatureTabs", ImGuiTabBarFlags_None))
        {
            for (auto sectionIdx = 0; sectionIdx < static_cast<int>(FeatureSection::Count); ++sectionIdx)
            {
                auto section = static_cast<FeatureSection>(sectionIdx);
                auto sectionFeatures = getFeaturesBySection(section);
                if (sectionFeatures.empty()) continue;

                bool hasAllowDraw = std::any_of(sectionFeatures.begin(), sectionFeatures.end(),
                                                [](FeatureBase* feature) { return feature->isAllowDraw(); });
                if (!hasAllowDraw) continue;

                if (ImGui::BeginTabItem(getSectionName(section)))
                {
                    ImGui::Spacing();

                    for (auto* feature : sectionFeatures)
                    {
                        bool enabled = feature->isEnabled();
                        if (ImGui::Checkbox(feature->getName().c_str(), &enabled))
                        {
                            feature->setEnabled(enabled);
                        }

                        if (!feature->getDescription().empty())
                        {
                            ImGui::SameLine();
                            helpMarker(feature->getDescription().c_str());
                        }

                        feature->draw();

                        ImGui::Spacing();
                        ImGui::Separator();
                        ImGui::Spacing();
                    }

                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }
    }

    FeatureBase* FeatureManager::getFeature(const std::string& nameKey)
    {
        const auto it = m_featureMap.find(nameKey);
        return it != m_featureMap.end() ? it->second : nullptr;
    }

    std::vector<FeatureBase*> FeatureManager::getFeaturesBySection(FeatureSection section)
    {
        std::vector<FeatureBase*> features;
        for (const auto& feature : m_features)
        {
            if (feature->getSection() == section)
            {
                features.push_back(feature.get());
            }
        }
        return features;
    }

    const char* FeatureManager::getSectionName(FeatureSection section)
    {
        switch (section)
        {
            case FeatureSection::Player:
                return LANG("Player");
            case FeatureSection::Combat:
                return LANG("Combat");
            case FeatureSection::Game:
                return LANG("Game");
            case FeatureSection::Visual:
                return LANG("Viaual");
            case FeatureSection::Settings:
                return LANG("Settings");
            case FeatureSection::Debug:
                return ("Debug");
            case FeatureSection::Hooks:
            case FeatureSection::Count:
            default:
                break;
        }

        return "Unknown";
    }

    void FeatureManager::helpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}
