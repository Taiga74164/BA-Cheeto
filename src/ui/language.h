#pragma once
#include "pch.h"
#include <vector>
#include <string>

enum class LanguageType
{
    English,
    Chinese
};

class Language
{
public:
    static Language& getInstance();
    
    // Initialize language system - now much simpler
    bool initialize();
    
    // Get localized text by key
    const char* getText(const char* key) const;
    
    // Set current language
    void setLanguage(LanguageType lang);
    
    // Get current language
    LanguageType getCurrentLanguage() const { return m_currentLanguage; }
    
    // Get available languages
    const std::vector<std::string>& getAvailableLanguages() const { return m_availableLanguages; }
    
    // Check if language system is initialized
    bool isInitialized() const { return m_initialized; }
    
    // Get language name string
    const char* getLanguageName(LanguageType lang) const;

private:
    Language() = default;
    ~Language() = default;
    Language(const Language&) = delete;
    Language& operator=(const Language&) = delete;

private:
    LanguageType m_currentLanguage = LanguageType::English; // 默认中文
    std::vector<std::string> m_availableLanguages = {"EN", "zh-CN"};
    bool m_initialized = false;
};

// Helper macros for getting localized text
#define LANG(key) Language::getInstance().getText(key)
#define T(key) LANG(key)  // Short alias
#define TR(key) LANG(key) // Translation alias

// Convenience macros for common UI elements
//#define MENU_FILE LANG("File")
//#define MENU_WINDOWS LANG("Windows") 
//#define MENU_TOOLS LANG("Tools")
//#define MENU_HELP LANG("Help")
//
//#define BTN_OK LANG("OK")
//#define BTN_CANCEL LANG("Cancel")
//#define BTN_CLOSE LANG("Close")
//#define BTN_YES LANG("Yes")
//#define BTN_NO LANG("No")
//
//// Status indicators
//#define STATUS_ACTIVE LANG("Active")
//#define STATUS_INACTIVE LANG("Inactive")
//#define STATUS_CONNECTED LANG("Connected")
//#define STATUS_DISCONNECTED LANG("Disconnected")
//#define STATUS_INITIALIZING LANG("Initializing...")
//
//// Common labels
//#define LBL_PERFORMANCE LANG("Performance")
//#define LBL_STATUS LANG("Status")
//#define LBL_VERSION LANG("Version 1.0.0")

// Language switching helper macro
#define SWITCH_LANG(lang) Language::getInstance().setLanguage(lang)
