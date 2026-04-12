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
    return fmt::format("threshold-{}", EditorIDs::getID(level));
}