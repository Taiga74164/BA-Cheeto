#pragma once
#include "FeatureBase.h"

namespace cheat
{
    class FeatureManager
    {
    public:
        static FeatureManager& getInstance();

        template <typename... Arg>
        void registerFeatures()
        {
            (registerFeature(std::make_unique<Arg>()), ...);
        }

        void init();

        void draw();

        // 通过翻译键获取特性
        FeatureBase* getFeature(const std::string& nameKey);
        std::vector<FeatureBase*> getFeaturesBySection(FeatureSection section);

    private:
        FeatureManager() = default;
        std::vector<std::unique_ptr<FeatureBase>> m_features;
        std::unordered_map<std::string, FeatureBase*> m_featureMap;

        void registerFeature(std::unique_ptr<FeatureBase> feature);

        static const char* getSectionName(FeatureSection section);
        static void helpMarker(const char* desc);
    };
}
