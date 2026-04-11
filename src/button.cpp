// Include the Geode header and other headers
#include <Geode/Geode.hpp>
#include "headers/utils.hpp"
#include "headers/percentagePopup.hpp"

// Includes headers for layers being modified, hooked, or using methods
#include <Geode/modify/PauseLayer.hpp>

// Includes dependencies
#include <cvolton.level-id-api/include/EditorIDs.hpp>

// Brings all Geode namespaces to the current scope
using namespace geode::prelude;

// Modifying PauseLayer
class $modify(timeToRagePauseLayer, PauseLayer) {
    
    // Function that runs when the button is clicked
    void onButtonClicked(CCObject*) {

        // Declare playLayer for use as a variable to save like 6 or 7 bytes
        auto playLayer = PlayLayer::get();

        // Checks if playLayer is null
        if (!playLayer) return;

        // Declares level for use as a variable inside of lambda bc C++ hates me
        auto level = playLayer->m_level;

        // Popup
        ttrBtnPopup::create([this, level](float value) {

            // Clamp the input if the user wants to set it out of range
            float clamped = std::clamp(value, 0.f, 100.f);

            // Saves the input
            Mod::get()->setSavedValue<float>(getLevelKey(level), clamped);
        })->show();
    }

    // Hooking customSetup function
    void customSetup() override {
        PauseLayer::customSetup();

        /*
            Yet again, declaring playLayer for use as a variable.
            I probably could have made it global earlier
            but my lazy ahh doesn't want to think about that
        */
        auto playLayer = PlayLayer::get();

        // Check if playLayer is null or if the player is on a platformer (ew)
        if (!playLayer || playLayer->m_level->isPlatformer()) return;

        // Make the actual button
        auto menu = this->getChildByID("right-button-menu");
        if (!menu) return;
            auto btnSprite = CircleButtonSprite::createWithSprite(
                "buttonIcon.png"_spr,
                1.f,
                CircleBaseColor::Red, CircleBaseSize::Tiny
            );
            btnSprite->setScale(1.2f);
            auto btn = CCMenuItemSpriteExtra::create(
                btnSprite,
                this,
                menu_selector(timeToRagePauseLayer::onButtonClicked)
            );
            
            btn->setID("timeToRageBtn"_spr);    // i love geode node ids
            menu->addChild(btn);
            menu->updateLayout();
    }
};