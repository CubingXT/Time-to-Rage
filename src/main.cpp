// Include headers and bring namespaces to the current scope
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Modifying the play layer
class $modify(PlayerHasDied, PlayLayer) {
    
    // Hook destroyPlayer function
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto label = CCLabelBMFont::create("Hello world!", "bigFont.fnt");
        label->setPosition(100, 100);
        this->addChild(label);
        PlayLayer::destroyPlayer(p0, p1);
    }
};