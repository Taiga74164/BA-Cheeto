#include "pch.h"
#include "language.h"
#include "translations.h"

Language& Language::getInstance()
{
    static Language instance;
    return instance;
}

bool Language::initialize()
{
    if (m_initialized)
    {
        return true;
    }

    LOG_INFO("[Language] Initializing language system...");
    
    // No complex loading needed - translations are embedded in code
    m_initialized = true;
    
    LOG_INFO("[Language] Language system initialized successfully with %zu translations", g_translations.size());
    return true;
}

const char* Language::getText(const char* key) const
{
    if (!key)
    {
        return "";
    }

    // Find the translation entry
    auto it = g_translations.find(key);
    if (it != g_translations.end())
    {
        const TranslationEntry& entry = it->second;
        
        // Return the appropriate language
        switch (m_currentLanguage)
        {
            case LanguageType::English:
                return entry.english ? entry.english : key;
            case LanguageType::Chinese:
                return entry.chinese ? entry.chinese : key;
            default:
                return entry.english ? entry.english : key;
        }
    }

    // If no translation found, return original key
    return key;
}

void Language::setLanguage(LanguageType lang)
{
    LanguageType oldLang = m_currentLanguage;
    m_currentLanguage = lang;
    const char* langName = getLanguageName(lang);
    LOG_INFO("[Language] Language changed from %s to: %s", getLanguageName(oldLang), langName);
    
    // Test a few translations to verify the change
    // LOG_INFO("[Language] Test translations after change:");
    // LOG_INFO("[Language]   'English' -> '%s'", getText("English"));
    // LOG_INFO("[Language]   'Chinese' -> '%s'", getText("Chinese"));
    // LOG_INFO("[Language]   'Language' -> '%s'", getText("Language"));
}

const char* Language::getLanguageName(LanguageType lang) const
{
    switch (lang)
    {
    case LanguageType::English:
        return TR("English");
    case LanguageType::Chinese:
        return TR("Chinese");
    default:
        return "Unknown";
    }
}
