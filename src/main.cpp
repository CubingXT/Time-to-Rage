// Include the Geode header and other headers
#include <Geode/Geode.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "headers/utils.hpp"

// Includes headers for layers being modified, hooked, or using methods
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>

// Includes dependencies
#include <cvolton.level-id-api/EditorIDs.hpp>

using namespace geode::prelude;

// Checks if playing from startpos, practice mode, or platformer and checks settings if they are overriden
bool checkBools(PlayLayer* playLayer) {
    bool fromStartPosition = 
        Mod::get()->getSettingValue<bool>("startpos-override") ? 
        false : playLayer->m_isTestMode;
    bool playingInPracticeMode = 
        Mod::get()->getSettingValue<bool>("practice-mode-override") ? 
        false : playLayer->m_isPracticeMode;
    bool levelIsPlatformer = playLayer->m_isPlatformer;
    bool result = !levelIsPlatformer && !fromStartPosition && !playingInPracticeMode;
    return result;
}

// Resumes all audio across platforms
void resumeAudio(FMODAudioEngine* engine) {
    #ifndef GEODE_IS_IOS
        engine->resumeAllAudio();
    #else
        if (engine->m_globalChannel) engine->m_globalChannel->setPaused(false);
        if(engine->m_reverbChannel) engine->m_reverbChannel->setPaused(false);
        engine->m_globalChannelPaused = false;
    #endif
}

// Play the rage sounds
void playSound(PlayLayer* playLayer, std::string(pathSt), FMODAudioEngine* engine) {
    WeakRef<PlayLayer> weakPlayLayer = playLayer;
    Loader::get()->queueInMainThread([pathSt, weakPlayLayer, engine]() -> void {
        if (Ref<PlayLayer> layer = weakPlayLayer.lock()) {
            engine->playEffect(pathSt);
            layer->pauseGame(false);
            
        }
    });
}

// Checks if the sound file exists
void checkForFile(PlayLayer* playLayer) {
    auto customSound = Mod::get()->getSettingValue<asp::fs::Path>("rage-sounds");   // stfu vscode. your yellow squigglies are blatantly, confidently wrong
    auto path = customSound.exists() ? customSound : Mod::get()->getResourcesDir() / "ahhhhSoundEffect.ogg";    // even more confidently wrong than freaking ai overview
    if (path.exists()) {
        auto engine = FMODAudioEngine::sharedEngine();
        auto pathSt = path.string();
        FMODAudioEngine::sharedEngine()->preloadEffect(pathSt);
        playSound(playLayer, pathSt, engine);
    }
}

// Checks if the player died at or after the set threshold
void getThresh(PlayLayer* playLayer, float percent, GJGameLevel* level, float threshold) {
    if (percent >= threshold) {
        bool silent = Mod::get()->getSettingValue<bool>("silent-mode");
        if (silent) {
            playLayer->pauseGame(false);
        }
        else checkForFile(playLayer);
    }
}

// Run the mod
void rage(PlayLayer* playLayer, float percent, GJGameLevel* level) {
    // Gets the rage percentage
    float threshold = 
        Mod::get()->getSavedValue<float>(getLevelKey(level), 
        Mod::get()->getSettingValue<double>("percentage-default-value")
    );
    getThresh(playLayer, percent, level, threshold);
}

// Checks if the mod should run
void checkIfRun(PlayLayer* playLayer, GJGameLevel* level, float percent) {
    if (playLayer) {
        if (checkBools(playLayer)) rage(playLayer, percent, level);
    }
    else {

        // Call original function bc rubrub hates me
        log::warn("Possible: Sound effect file not found or RubRub just hates me");
        return;
    }
}

// Modifying PlayerObject
class $modify(TimeToRageDeath, PlayerObject) {
    void playerDestroyed(bool p0) {
        auto playLayer = PlayLayer::get();

        // Check if playtesting
        if (LevelEditorLayer::get()) {
            PlayerObject::playerDestroyed(p0);
            return;
        }

        // Get death certificate
        auto level = playLayer->m_level;
        float percent = playLayer->getCurrentPercent();

        // Kill the player
        PlayerObject::playerDestroyed(p0);

        checkIfRun(playLayer, level, percent);
    }
};