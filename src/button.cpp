#include "headers/main.hpp" // IWYU pragma: keep

using namespace geode::prelude;

// Modifying PauseLayer
class $modify(timeToRagePauseLayer, PauseLayer) {
    // Function that runs when the button is clicked
    void onButtonClicked(CCObject*) {
        PlayLayer* playLayer = PlayLayer::get();
        if (!playLayer) return;
        GJGameLevel* level = playLayer->m_level;
        // Popup
        ttrBtnPopup::create([this, level](float value) -> void {
            float clamped = std::clamp(value, 0.f, 100.f);
            Mod::get()->setSavedValue<float>(getLevelKey(level), clamped);
        }) -> show();
    }
    // Hooking customSetup function
    void customSetup() override {
        PauseLayer::customSetup();
        PlayLayer* playLayer = PlayLayer::get();

        // Check if playLayer is null or if the player is on a platformer (ew)
        if (!playLayer || playLayer->m_level->isPlatformer()) return;
        
        // Make the actual button
        CCNode* menu = this->getChildByIDRecursive("right-button-menu");
        if (!menu) return;
        CircleButtonSprite* btnSprite = CircleButtonSprite::createWithSprite(
            "buttonIcon.png"_spr,
            1.f,
            CircleBaseColor::Red, CircleBaseSize::Tiny
        );
        btnSprite->setScale(1.2f);
        CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(
            btnSprite,
            this,
            menu_selector(timeToRagePauseLayer::onButtonClicked)
        );
        btn->setID("timeToRageBtn"_spr);    // i love geode node ids 
        menu->addChild(btn);
        menu->updateLayout();
    }
};