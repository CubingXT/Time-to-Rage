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
    
    
    void onButtonClicked(CCObject*) {

        // What the button does
        auto playLayer = PlayLayer::get();
        if (!playLayer) return;
        auto level = playLayer->m_level;

        // Popup
        ttrBtnPopup::create([this, level](float value) {
            float clamped = std::clamp(value, 0.f, 100.f);
            Mod::get()->setSavedValue<float>(getLevelKey(level), clamped);
        })->show();
    }

    // Hooking customSetup function
    void customSetup() override {
        PauseLayer::customSetup();
        auto playLayer = PlayLayer::get();
        if (!playLayer || playLayer->m_level->isPlatformer()) return;
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
            
            btn->setID("timeToRageBtn"_spr);
            menu->addChild(btn);
            menu->updateLayout();
    }
};