#include "GameSence.h"

USING_NS_CC;

Scene* GameSence::createScene() {
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = GameSence::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool GameSence::init() {

	if (!Layer::init()) {
		return false;
	}

	// add touch listener
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(GameSence::onTouchBegan, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// set bacground image
	auto bg = Sprite::create("level-background-0.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	// create stone sprite
	stone = Sprite::create("stone.png");
	stone->setPosition(Vec2(origin.x + 560, origin.y + 480));
	this->addChild(stone, 1);

	// load game resource--learn from AppDelegate.cpp
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("level-sheet.plist");
	char totalFrames = 8;
	char frameName[20];
	Animation* legAnimation = Animation::create();

	for (int i = 0; i < totalFrames; i++) {
		sprintf(frameName, "gem-mouse-%d.png", i);
		legAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName));
	}
	legAnimation->setDelayPerUnit(0.1);
	AnimationCache::getInstance()->addAnimation(legAnimation, "legAnimation");

	// create mouse sprite by Sprite Sheet
	mouse = Sprite::createWithSpriteFrameName("gem-mouse-0.png");
	Animate* legAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("legAnimation"));
	mouse->runAction(RepeatForever::create(legAnimate));
	mouse->setPosition(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);
	this->addChild(mouse, 2);
	
	// shoot label
	auto shootLabel = Label::create("Shoot", "fonts/Marker Felt.ttf", 50);
	auto shoot = MenuItemLabel::create(shootLabel, CC_CALLBACK_1(GameSence::shootMenuCallback, this));
	shoot->setPosition(Vec2(origin.x + visibleSize.width * 4 / 5, origin.y + visibleSize.height * 3 / 4));
	auto menu = Menu::create(shoot, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 3);

	return true;
}

bool GameSence::onTouchBegan(Touch *touch, Event *unused_event) {
	// get touch position
	auto location = touch->getLocation();

	// create cheese sprite
	auto cheese = Sprite::create("cheese.png");
	cheese->setPosition(location);
	this->addChild(cheese, 1);

	// action of cheese sprite
	auto fadeIn = FadeIn::create(1.0f);
	cheese->runAction(fadeIn);
	auto fadeOut = FadeOut::create(4.0f);
	cheese->runAction(fadeOut);
	this->removeChildByName("cheese", true);

	// action of mouse sprite
	auto moveTo = MoveTo::create(1.5, location);
	mouse->runAction(moveTo);
	
	return true;
}

// click shoot label
void GameSence::shootMenuCallback(Ref* pSender) {
	// create a new sprite same as stone
	auto stone1 = Sprite::create("stone.png");
	stone1->setPosition(stone->getPosition());
	this->addChild(stone1);

	// action of stone1 sprite
	auto moveTo = MoveTo::create(0.5, mouse->getPosition());
	stone1->runAction(moveTo);
	auto fadeOut = FadeOut::create(1.5f);
	stone1->runAction(fadeOut);
	this->removeChildByName("stone1");

	// create diamond sprite
	auto diamond = Sprite::create("diamond.png");
	diamond->setPosition(mouse->getPosition());
	this->addChild(diamond, 1);

	// action of mouse sprite
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	auto moveToo = MoveTo::create(1.5, Vec2(origin.x + visibleSize.width / 5, origin.y + visibleSize.height * 3 / 4));
	mouse->runAction(moveToo);
}
