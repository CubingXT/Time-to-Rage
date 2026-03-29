// Include headers and bring namespaces to the current scope
#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

// Modifying the menu layer
class $modify(MenuLayer) {

	// The signiture is 'bool MenuLayer::init()'
	// This part is just listening for the called class we're modifying
	bool init() {
		if (!MenuLayer::init())
			return false;

		/* We can write our code here*/
		
		auto winSize = CCDirector::get()->getWinSize();

		// CCLabelBMFont displays text
		auto label = CCLabelBMFont::create("Hello, world!", "bigFont.fnt");
		label->setPosition(winSize / 2);
		this->addChild(label);


		// This next line simply ends the modification
		return true;
	} 
};