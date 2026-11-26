#pragma once

#include <Geode/Geode.hpp>
#include <cvolton.level-id-api/EditorIDs.hpp>

using namespace geode::prelude;

// Get level id utility
inline std::string getLevelKey(GJGameLevel* level) {
    // love editor level ids api thx cvolton
    fmt::string_view type =
        level->m_levelType == GJLevelType::Editor ? "editor" :
        level->m_levelType == GJLevelType::Main ? "main" :
        "level"
    ;
    return fmt::format("{}-{}", type, EditorIDs::getID(level));
}
