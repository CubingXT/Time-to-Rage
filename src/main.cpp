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

        /* Scripts that run on the last alive frame */

        // Declares playLayer function for use as a variable
        auto playLayer = PlayLayer::get();

        // Checking if playtesting and terminates if it is
        if (LevelEditorLayer::get()) {

            // Call original function bc were in playtesting mode
            PlayerObject::playerDestroyed(p0);

        /* Scripts that run if not in playtesting */

        } else if (playLayer) {

            // Getting bools to check if mod should run and check for overrides
            bool fromStartPosition = 
                Mod::get()->getSettingValue<bool>("startpos-override") ? 
                false : playLayer->m_isTestMode;
            bool playingInPracticeMode = 
                Mod::get()->getSettingValue<bool>("practice-mode-override") ? 
                false : playLayer->m_isPracticeMode;
            bool levelIsPlatformer = playLayer->m_isPlatformer;
            
            /* Scripts to run on the first dead frame */

            // Get death certificate
            auto level = playLayer->m_level;
            float percent = playLayer->getCurrentPercent();

            // Call original playerDestroyed function
            PlayerObject::playerDestroyed(p0);

            // Checking bools from earlier
            if (
                !levelIsPlatformer && 
                !fromStartPosition && 
                !playingInPracticeMode
            ) {

                // Gets the rage percentage
                float threshold = 
                    Mod::get()->getSavedValue<float>(getLevelKey(level), 
                    Mod::get()->getSettingValue<double>(
                        "percentage-default-value"
                    )
                );

                // Checks if the player died at or after the set threshold
                if (percent >= threshold) {
                    auto value = Mod::get()->getSettingValue<std::filesystem::path>("rage-sounds");
                    std::string logPath = geode::utils::string::pathToString(value);
                    log::debug("path = {}", logPath);
                    auto path = 
                        Mod::get()->getResourcesDir() / "ahhhhSoundEffect.wav";
                    
                    // Check if the audio file actually exists
                    // i fcking hate fmod
                    if (std::filesystem::exists(path)) {
                        auto pathSt = path.string();
                        FMODAudioEngine::sharedEngine()->preloadEffect(pathSt);
                        Loader::get()->queueInMainThread(
                        [pathSt, playLayer]() {
                            FMODAudioEngine::sharedEngine()->playEffect(pathSt);
                            playLayer->pauseGame(false);
                            FMODAudioEngine::sharedEngine()->resumeAllEffects();
                        });
                    }
                }
            }

        /* Scripts that run if something weird happens */

        } else {

            // Call original function bc rubrub hates me
            PlayerObject::playerDestroyed(p0);
        }
    }
};