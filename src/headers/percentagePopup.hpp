#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>

using namespace geode::prelude;

class ttrBtnPopup : public Popup {
protected:
    std::function<void(float)> m_callback;
    TextInput* m_input;

    bool setup(std::function<void(float)> callback) {
        m_callback = callback;

        this->setTitle("Enter Number");

        auto contentSize = m_mainLayer->getContentSize();

        // Create text input
        m_input = TextInput::create(200.f, "Enter a number...");
        m_input->setFilter("0123456789");
        m_input->setPosition(contentSize / 2 + ccp(0, 10));
        m_mainLayer->addChild(m_input);

        // Confirm button
        auto confirmSpr = ButtonSprite::create("OK", "goldFont.fnt", "GJ_button_01.png", .8f);
        auto confirmBtn = CCMenuItemSpriteExtra::create(confirmSpr, this, menu_selector(ttrBtnPopup::onConfirm));
        confirmBtn->setPosition(contentSize / 2 + ccp(0, -30));
        m_buttonMenu->addChild(confirmBtn);

        return true;
    }

    void onConfirm(CCObject*) {
        auto result = utils::numFromString<float>(m_input->getString());
        if (result) {
            m_callback(result.unwrap());
            this->onClose(nullptr);
        }
    }

public:
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