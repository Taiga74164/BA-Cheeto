#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <filesystem>

#include <UnityResolve.hpp>

#include <magic_enum.hpp>

#include <imgui.h>

// Project includes
#include "utils/dx_utils.h"
#include "utils/logger.h"
#include "memory/hook_manager.h"
#include "ui/gui.h"
#include "appdata/helpers.h"
#include "appdata/types.h"
#include "ui/language.h"
