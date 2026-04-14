// Tells the thing not to run multiple times
#pragma once

// Include Geode headers
#include <Geode/Geode.hpp>

// Include dependencies
#include <cvolton.level-id-api/include/EditorIDs.hpp>

// Brings all Geode namespaces to the current scope
using namespace geode::prelude;

// Get level id utility
inline std::string getLevelKey(GJGameLevel* level) {
    // love editor level ids api thx cvolton
    if (level->m_levelType == GJLevelType::Editor) {
        return fmt::format("editor-{}", EditorIDs::getID(level));
    }
    return fmt::format("lvl-{}", EditorIDs::getID(level));
}