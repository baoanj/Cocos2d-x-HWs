#include "MenuSence.h"
#include "GameSence.h"
#include "SimpleAudioEngine.h"
#include <string>
using namespace std;

USING_NS_CC;

using namespace CocosDenshion;

Scene* MenuSence::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = MenuSence::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuSence::init() {

    if ( !Layer::init() ) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("bg.jpg");
	bg->setScale(0.8);
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(bg, 0);

	auto fire = ParticleFire::create();
	fire->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y - 150));
	this->addChild(fire, 1);
	
	auto title = Label::createWithTTF("Welcome to Circle Game !", "fonts/arial.ttf", 50);
	title->setColor(Color3B(0, 255, 0));
	title->setPosition(Vec2(origin.x + visibleSize.width / 2, visibleSize.height + origin.y - title->getHeight() - 50));
	this->addChild(title, 2);

	auto intro = Label::createWithTTF("How to play?\n1.Move your circle with direction key or letter 'WASD'.\n2.If meet bigger circle,your blood will reduce.\n3.If meet smaller circle,your blood will increase.\n4.If meet same-size circle, nothing will happen.\n5.In the beginning,you will have half-blood.\n6.If you have no blood,you lose.\n7.If you have full blood,you win.\n8.Click \"Start\" to start", "fonts/arial.ttf", 20);
	intro->setWidth(290);
	intro->setPosition(Vec2(origin.x + visibleSize.width - 150, visibleSize.height / 2 + origin.y));
	this->addChild(intro, 2);

	// start MenuItem
	auto start = MenuItemFont::create("Start", CC_CALLBACK_1(MenuSence::startMenuCallback, this));
	start->setColor(Color3B(0, 255, 0));
	start->setPosition(Vec2(origin.x + visibleSize.width / 2, visibleSize.height / 2 + origin.y));
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

    return true;
}

// Scene switch
void  MenuSence::startMenuCallback(Ref* pSender) {
	auto myScene = GameSence::createScene();
	Director::getInstance()->replaceScene(TransitionFlipX::create(1.5, myScene));
}