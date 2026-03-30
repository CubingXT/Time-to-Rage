// Include headers and bring namespaces to the current scope
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;

// Modifying the play layer
class $modify(PlayerHasDied, PlayLayer) {
    
    // Hook destroyPlayer function
    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        

        PlayLayer::destroyPlayer(player, obj);

        float percent = this->getCurrentPercent();
        if (percent > 2) log::info("Died at {}%", percent);
    } 
};