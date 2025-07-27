#pragma once
#include <unordered_map>
#include <string>

struct TranslationEntry {
    const char* english;
    const char* chinese;
};

static const std::unordered_map<std::string, TranslationEntry> g_translations = {
    {"File", {"File", "文件"}},
    {"Exit", {"Exit", "退出"}},
    {"Windows", {"Windows", "窗口"}},
    {"Theme", {"Theme", "主题"}},
    {"Tools", {"Tools", "工具"}},
    {"Help", {"Help", "帮助"}},
    {"Player", {"Player", "角色"}},
    {"Combat", {"Combat", "战斗"}},
    {"Game", {"Game", "游戏"}},
    {"Viaual", {"Viaual", "视觉"}},
    {"Settings", {"Settings", "设置"}},
    {"Debug", {"Debug", "调试"}},
    {"English", {"English", "English"}},
    {"Chinese", {"Chinese", "简体中文"}},
    {"INSERT: Toggle GUI", {"INSERT: Toggle GUI", "INS 按键切换显示"}},
    {"God Mode", {"God Mode", "角色无敌"}},
    {"No Cost", {"No Cost", "技能无消耗"}},
    {"Makes player invincible", {"Makes player invincible", "使角色无敌"}},
    {"Removes skill costs and enables instant regeneration", {"Removes skill costs and enables instant regeneration", "移除技能消耗并启用即时恢复"}},
    {"Instant Win", {"Instant Win", "瞬间胜利"}},
    {"Instantly wins battles", {"Instantly wins battles", "立即赢得战斗"}},
    {"One Hit Kill", {"One Hit Kill", "一击必杀"}},
    {"Enemies die in one hit", {"Enemies die in one hit", "敌人一击必死"}},
    {"Dumb Enemies", {"Dumb Enemies", "敌人无脑"}},
    {"Enemies ignore targeting you", {"Enemies ignore targeting you", "敌人忽略对你的攻击"}},
    {"Misc", {"Misc", "杂项"}},
    {"Misc Features", {"Misc Features", "杂项功能"}},
    {"Example Window", {"Example Window", "示例窗口"}},
    {"Language", {"Language", "语言"}},
};
