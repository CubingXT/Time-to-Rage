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

    log::info("[TTR] checkBools: startposOverride={} practiceOverride={} | m_isTestMode={} m_isPracticeMode={} m_isPlatformer={}",
        Mod::get()->getSettingValue<bool>("startpos-override"),
        Mod::get()->getSettingValue<bool>("practice-mode-override"),
        playLayer->m_isTestMode,
        playLayer->m_isPracticeMode,
        playLayer->m_isPlatformer
    );
    log::info("[TTR] checkBools: fromStartPosition={} playingInPracticeMode={} levelIsPlatformer={}",
        fromStartPosition, playingInPracticeMode, levelIsPlatformer
    );

    bool result = !levelIsPlatformer && !fromStartPosition && !playingInPracticeMode;
    log::info("[TTR] checkBools: result={}", result);
    return result;
}

// Play the rage sounds
void playSound(PlayLayer* playLayer, std::string(pathSt)) {
    log::info("[TTR] playSound: queueing main thread task for '{}'", pathSt);
    WeakRef<PlayLayer> weakPlayLayer = playLayer;
    Loader::get()->queueInMainThread([pathSt, weakPlayLayer]() -> void {
        log::info("[TTR] playSound: main thread task running");
        if (Ref<PlayLayer> layer = weakPlayLayer.lock()) {
            log::info("[TTR] playSound: playLayer still alive, playing effect + pausing");
            FMODAudioEngine::sharedEngine()->playEffect(pathSt);
            layer->pauseGame(false);
            FMODAudioEngine::sharedEngine()->resumeAllAudio();
            log::info("[TTR] playSound: done");
        } else {
            log::warn("[TTR] playSound: playLayer was gone, nothing done");
        }
    });
}

// Checks if the sound file exists
void checkForFile(PlayLayer* playLayer) {
    log::info("[TTR] checkForFile: called");
    auto customSound = Mod::get()->getSettingValue<asp::fs::Path>("rage-sounds");   // stfu vscode. your yellow squigglies are blatantly, confidently wrong
    auto path = customSound.exists() ? customSound : Mod::get()->getResourcesDir() / "ahhhhSoundEffect.ogg";    // even more confidently wrong than freaking ai overview
    log::info("[TTR] checkForFile: path='{}' exists={}", path.string(), path.exists());
    if (path.exists()) {
        auto pathSt = path.string();
        FMODAudioEngine::sharedEngine()->preloadEffect(pathSt);
        playSound(playLayer, pathSt);
    } else {
        log::warn("[TTR] checkForFile: file does not exist, no pause will happen");
    }
}

// Checks if the player died at or after the set threshold
void getThresh(PlayLayer* playLayer, float percent, GJGameLevel* level, float threshold) {
    log::info("[TTR] getThresh: percent={} threshold={} reached={}", percent, threshold, percent >= threshold);
    if (percent >= threshold) {
        bool silent = Mod::get()->getSettingValue<bool>("silent-mode");
        log::info("[TTR] getThresh: silent-mode={}", silent);
        if (silent) {
            log::info("[TTR] getThresh: calling pauseGame directly");
            playLayer->pauseGame(false);
        }
        else checkForFile(playLayer);
    } else {
        log::info("[TTR] getThresh: below threshold, doing nothing");
    }
}

// Run the mod
void rage(PlayLayer* playLayer, float percent, GJGameLevel* level) {
    // Gets the rage percentage
    float threshold = 
        Mod::get()->getSavedValue<float>(getLevelKey(level), 
        Mod::get()->getSettingValue<double>("percentage-default-value")
    );
    log::info("[TTR] rage: levelKey='{}' threshold={} defaultSetting={}",
        getLevelKey(level),
        threshold,
        Mod::get()->getSettingValue<double>("percentage-default-value")
    );
    getThresh(playLayer, percent, level, threshold);
}

// Checks if the mod should run
void checkIfRun(PlayLayer* playLayer, GJGameLevel* level, float percent) {
    log::info("[TTR] checkIfRun: playLayer={} percent={}", fmt::ptr(playLayer), percent);
    if (playLayer) {
        if (checkBools(playLayer)) rage(playLayer, percent, level);
        else log::info("[TTR] checkIfRun: checkBools blocked the mod from running");
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
        log::info("[TTR] playerDestroyed: hook fired, p0={} playLayer={} editorLayer={} isPlayer1={}",
            p0,
            fmt::ptr(playLayer),
            fmt::ptr(LevelEditorLayer::get()),
            playLayer ? (this == playLayer->m_player1) : false
        );

        // Check if playtesting
        if (LevelEditorLayer::get()) {
            log::info("[TTR] playerDestroyed: in editor, running original and returning");
            PlayerObject::playerDestroyed(p0);
            return;
        }

        // Get death certificate
        auto level = playLayer->m_level;
        float percent = playLayer->getCurrentPercent();
        log::info("[TTR] playerDestroyed: percent={}", percent);

        // Kill the player
        PlayerObject::playerDestroyed(p0);
        log::info("[TTR] playerDestroyed: original finished, calling checkIfRun");

        checkIfRun(playLayer, level, percent);
    }
};