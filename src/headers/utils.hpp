// Include headers
#pragma once
#include <Geode/Geode.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

inline std::string getLevelKey(GJGameLevel* level) {
    return fmt::format("threshold-{}", EditorIDs::getID(level));
}