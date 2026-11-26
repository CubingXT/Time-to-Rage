#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>
#include "utils.hpp"

using namespace geode::prelude;

// Time To Rage Button Popup
class ttrBtnPopup : public Popup {

protected:

    std::function<void(float)> m_callback;

    TextInput* m_input;

    // The actual popup itself
    bool setup(std::function<void(float)> callback) {
        m_callback = callback;

        // Makes the title for the popup
        this->setTitle("Enter Number");

        // Set a value for the screen size (love cocos2dx)
        CCSize contentSize = m_mainLayer->getContentSize();

        PlayLayer* playLayer = PlayLayer::get();

        // Make the placeholder for the text input
        float current = 
            Mod::get()->getSavedValue<float>(getLevelKey(playLayer->m_level),
            Mod::get()->getSettingValue<double>("percentage-default-value")
        );
        std::string inputPlaceholder = fmt::format("{:.2f}", current);

        // Create text input and restrictions
        m_input = TextInput::create(200.f, inputPlaceholder);
        m_input->setFilter("0123456789.");
        m_input->setNumericMode(true);
        m_input->setMaxCharCount(5);

        // Put the popup in its place on the screen
        m_input->setPosition(contentSize / 2 + ccp(0, 10));
        m_mainLayer->addChild(m_input);

        // Confirm button
        ButtonSprite* confirmSpr = ButtonSprite::create(
            "OK",
            "goldFont.fnt",
            "GJ_button_01.png",
            .8f
        );
        CCMenuItemSpriteExtra* confirmBtn = CCMenuItemSpriteExtra::create(
            confirmSpr,
            this,
            menu_selector(ttrBtnPopup::onConfirm)
        );
        confirmBtn->setPosition(contentSize / 2 + ccp(0, -30));
        m_buttonMenu->addChild(confirmBtn);
        return true;
    }

    // What happens when you click ok
    void onConfirm(CCObject*) {
        auto result = utils::numFromString<float>(m_input->getString());
        if (result) {
            m_callback(result.unwrap());
            this->onClose(nullptr);
        }
    }

public:

    /*
        Tells the game to actually run the callback,
        or just return nullptr if rubrub feels like being difficult.
    */
    static ttrBtnPopup* create(std::function<void(float)> callback) {
        ttrBtnPopup* ret = new ttrBtnPopup();
        if (ret->init(240.f, 160.f)) {
            ret->setup(callback);
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};