#include "pch.h"
#include "feature_manager.h"
#include "utils/config_manager.h"
#include "utils/hotkey_manager.h"
#include "features/player/PlayerStats.h"

namespace cheat
{
    FeatureManager& FeatureManager::getInstance()
    {
        static FeatureManager instance;
        return instance;
    }

    void FeatureManager::registerFeature(std::unique_ptr<FeatureBase> feature)
    {
        const std::string& name = feature->getName();
        m_featureMap[name] = feature.get();
        m_features.push_back(std::move(feature));
    }

    void FeatureManager::init()
    {
    LOG_INFO("Initializing {} features...", m_features.size());
    ConfigManager::getInstance().load();
    // Ensure hotkey IDs exist for all features and load their values
    {
        auto& hk = HotkeyManager::getInstance();
        for (const auto& feature : m_features)
        {
            hk.registerHotkey(std::string("feature_") + feature->getName(), 0);
        }
        hk.load();
    }

        for (const auto& feature : m_features)
        {
            try
            {
                feature->setupEnabledField(getSectionName(feature->getSection()));
                feature->init();
                LOG_INFO("Feature '{}' initialized successfully", feature->getName().c_str());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Exception during initialization of feature '{}': {}", feature->getName().c_str(), e.what());
            }
        }
    }

    // TODO: Redesign with sidebar
    void FeatureManager::draw()
    {
        if (ImGui::BeginTabBar("FeatureTabs", ImGuiTabBarFlags_None))
        {
            // (Hotkeys tab removed per request; hotkeys remain per-feature below)

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

                        // Per-feature hotkey capture row
                        {
                            auto& hk = HotkeyManager::getInstance();
                            std::string id = std::string("feature_") + feature->getName();
                            hk.registerHotkey(id, 0);
                            int current = hk.getVk(id);
                            ImGui::TextDisabled("Hotkey: [%s]", HotkeyManager::keyName(current)); ImGui::SameLine();
                            if (!hk.isCapturing())
                            {
                                if (ImGui::Button((std::string("Add Hotkey##") + id).c_str())) hk.beginCapture(id);
                                ImGui::SameLine();
                                if (current != 0 && ImGui::Button((std::string("Reset##") + id).c_str())) hk.clear(id);
                            }
                            else if (hk.captureId() == id)
                            {
                                ImGui::TextUnformatted(" Press any key..."); ImGui::SameLine();
                                if (ImGui::Button("Cancel")) hk.cancelCapture();
                            }
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

    void FeatureManager::reloadConfig()
    {
        // Reload the JSON from disk first
        ConfigManager::getInstance().load();
        // Register hotkeys for all features (in case new ones exist), then load
        {
            auto& hk = HotkeyManager::getInstance();
            for (const auto& feature : m_features)
            {
                hk.registerHotkey(std::string("feature_") + feature->getName(), 0);
            }
            hk.load();
        }
        // Reinitialize features to refresh their Config::Field bindings and cached values
        for (const auto& feature : m_features)
        {
            // Reapply enabled flag from config
            feature->setupEnabledField(getSectionName(feature->getSection()));
            feature->init();
            // Hook for features that support explicit reload
            if (auto* ps = dynamic_cast<cheat::features::PlayerStats*>(feature.get()); ps)
                ps->reloadFromConfig();
        }
    }

    bool FeatureManager::onKeyDown(int vk)
    {
        auto& hk = HotkeyManager::getInstance();
        if (hk.isCapturing()) { hk.setCaptured(vk); return true; }
        // Feature toggles: feature_<Name>
        for (const auto& f : m_features)
        {
            std::string id = std::string("feature_") + f->getName();
            if (hk.getVk(id) == vk && vk != 0)
            {
                f->setEnabled(!f->isEnabled());
                return true;
            }
        }
        return false;
    }

    FeatureBase* FeatureManager::getFeature(const std::string& name)
    {
        const auto it = m_featureMap.find(name);
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
                return "Player";
            case FeatureSection::Combat:
                return "Combat";
            case FeatureSection::Game:
                return "Game";
            case FeatureSection::Settings:
                return "Settings";
            case FeatureSection::Debug:
                return "Debug";
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
