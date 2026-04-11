// Make the thing not call multiple times during compilation or some sht
#pragma once

// Include Geode headers and utils header
#include <Geode/Geode.hpp>
#include "utils.hpp"

// Brings all Geode namespaces to the current scope
using namespace geode::prelude;

// Make the class for the popup
class ttrBtnPopup : public Popup {

// Secret stuff for the class
protected:

    // Declares the member variable for the callback
    std::function<void(float)> m_callback;

    // Declare the text input variable
    TextInput* m_input;

    // The actual popup itself
    bool setup(std::function<void(float)> callback) {

        // Sets value for the callback variable
        m_callback = callback;

        // Makes the title for the popup
        this->setTitle("Enter Number");

        // Set a value for the screen size (love cocos2dx)
        auto contentSize = m_mainLayer->getContentSize();

        // Declares playLayer for use as a variable
        auto playLayer = PlayLayer::get();

        // Make the placeholder for the text input
        float current = 
            Mod::get()->getSavedValue<float>(getLevelKey(playLayer->m_level),
            Mod::get()->getSettingValue<double>("percentage-default-value")
        );
        std::string inputPlaceholder = std::format("{:.2f}", current);

        // Create text input and restrictions
        m_input = TextInput::create(200.f, inputPlaceholder);
        m_input->setFilter("0123456789.");
        m_input->setMaxCharCount(5);

        // Put the popup in its place on the screen
        m_input->setPosition(contentSize / 2 + ccp(0, 10));
        m_mainLayer->addChild(m_input);

        // Confirm button
        auto confirmSpr = ButtonSprite::create(
            "OK",
            "goldFont.fnt",
            "GJ_button_01.png",
            .8f
        );
        auto confirmBtn = CCMenuItemSpriteExtra::create(
            confirmSpr,
            this,
            menu_selector(ttrBtnPopup::onConfirm)
        );
        confirmBtn->setPosition(contentSize / 2 + ccp(0, -30));
        m_buttonMenu->addChild(confirmBtn);

        // Tells the game that drawing the popup was successful
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

// Not so secret stuff for the class
public:

    /*
        Tells the game to actually run the callback,
        or just return nullptr if rubrub feels like being difficult.
    */
    static ttrBtnPopup* create(std::function<void(float)> callback) {
        auto ret = new ttrBtnPopup();
        if (ret->init(240.f, 160.f)) {
            ret->setup(callback);
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};