// Include the Geode header and other headers
#include <filesystem>
#include <Geode/Geode.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include "headers/utils.hpp"

// Includes headers for layers being modified, hooked, or using methods
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

// Includes dependencies
#include <cvolton.level-id-api/include/EditorIDs.hpp>

// Brings all Geode namespaces to the current scope
using namespace geode::prelude;

// Modifying PlayerObject
class $modify(TimeToRageDeath, PlayerObject) {

    // Hook playerDestroyed function
    void playerDestroyed(bool p0) {

        // Declares playLayer function for use as a variable
        auto playLayer = PlayLayer::get();

        // Checking if playtesting and terminates if it is
        if (LevelEditorLayer::get()) {

            // Call original playerDestroyed function as we are in playtesting mode
            PlayerObject::playerDestroyed(p0);

        // Scripts that run if not in playtesting    
        } else if (playLayer) {

            // Getting booleans to check if mod should run
            bool levelIsPlatformer = playLayer->m_isPlatformer;
            bool fromStartPosition = playLayer->m_isTestMode;
            bool playingInPracticeMode = playLayer->m_isPracticeMode;
            
            // Scripts to run on player death

            // Get death certificate
            auto level = playLayer->m_level;
            int levelId = level ? EditorIDs::getID(level) : -1;
            float percent = playLayer->getCurrentPercent();

            // Call original playerDestroyed function
            PlayerObject::playerDestroyed(p0);

            // Scripts to run after player death
            if (!levelIsPlatformer && !fromStartPosition && !playingInPracticeMode) {
                float threshold = Mod::get()->getSavedValue<float>(getLevelKey(level), 90.f);   // 90 is the default
                if (percent >= threshold) {
                    auto path = Mod::get()->getResourcesDir() / "ahhhSoundEffect.wav";
                    
                    // Check if file actually exists
                    if (std::filesystem::exists(path)) {
                        auto pathStr = path.string();
                        FMODAudioEngine::sharedEngine()->preloadEffect(pathStr);
                        Loader::get()->queueInMainThread([pathStr, playLayer]() {
                            FMODAudioEngine::sharedEngine()->playEffect(pathStr);
                            playLayer->pauseGame(false);
                            FMODAudioEngine::sharedEngine()->resumeAllEffects();
                        });   
                    }
                }
            }

        // Scripts that run if something weird happens
        } else {

            // Call original playerDestroyed function as something weird happened
            PlayerObject::playerDestroyed(p0);
        }  
    }
};