#pragma once

#include "utils/singleton.h"
#include "../../ui/language.h"

namespace cheat
{
    enum class FeatureSection
    {
        Player,
        Combat,
        Game,
        Visual,
        Settings,
        Debug,
        Hooks,
        Count
    };

    class FeatureBase
    {
    public:
        FeatureBase(const std::string& nameKey, const std::string& descriptionKey, FeatureSection section)
            : m_nameKey(nameKey)
            , m_descriptionKey(descriptionKey)
            , m_section(section)
            , m_enabled(false)
            , m_allowDraw(section != FeatureSection::Hooks && section != FeatureSection::Count)
        {
        }

        virtual ~FeatureBase() = default;

        virtual void init()
        {
        }

        virtual void draw()
        {
        }

        virtual void onEnable()
        {
            LOG_INFO("%s enabled", getName().c_str());
        }

        virtual void onDisable()
        {
            LOG_INFO("%s disabled", getName().c_str());
        }

        // 动态获取翻译后的名称
        std::string getName() const 
        { 
            return TR(m_nameKey.c_str()); 
        }
        
        // 动态获取翻译后的描述
        std::string getDescription() const 
        { 
            return TR(m_descriptionKey.c_str()); 
        }
        
        // 获取翻译键（用于内部查找）
        const std::string& getNameKey() const { return m_nameKey; }
        const std::string& getDescriptionKey() const { return m_descriptionKey; }

        FeatureSection getSection() const { return m_section; }
        bool isEnabled() const { return m_enabled; }

        void setEnabled(bool enabled)
        {
            if (m_enabled != enabled)
            {
                m_enabled = enabled;
                if (m_enabled)
                {
                    onEnable();
                }
                else
                {
                    onDisable();
                }
            }
        }

        bool isAllowDraw() const { return m_allowDraw; }

    protected:
        std::string m_nameKey;      
        std::string m_descriptionKey; 
        FeatureSection m_section;
        bool m_enabled;
        bool m_allowDraw;
    };
}
