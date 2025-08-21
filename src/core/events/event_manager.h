﻿#pragma once
#include "event.h"

class EventManager
{
public:
    static EventManager& getInstance();

    static Event<> onReloadConfig;
    static Event<int, bool&> onKeyDown;

    static FastEvent<> onUpdate;
    static Event<> onBattleFinalize;

    static void shutdown();

private:
    EventManager();
    ~EventManager();
};
