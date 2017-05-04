#include "GameSence.h"
#include "MenuSence.h"
#include <string>
using namespace std;
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

void GameSence::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }

Scene* GameSence::createScene() {
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setGravity(Point(0, 0));

	auto layer = GameSence::create(scene->getPhysicsWorld());

	scene->addChild(layer);
	return scene;
}

bool GameSence::init(PhysicsWorld* world) {
	if (!Layer::init()) {
		return false;
	}

	this->setPhysicsWorld(world);
	visibleSize = Director::getInstance()->getVisibleSize();
	origin = Director::getInstance()->getVisibleOrigin();

	// preload music
	preloadMusic();
	// play background music
	playBgm();

	// add edge for whole game
	addEdge();
	// add player
	addPlayer();

	// the blood of player
	hp();

	addContactListener();
	addKeyboardListener();

	// a label of game's name
	time = Label::createWithTTF("Circle Game", "fonts/arial.ttf", 30);
	time->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - time->getHeight() - 20));
	addChild(time);

	this->schedule(schedule_selector(GameSence::update), 1);

	return true;
}

GameSence * GameSence::create(PhysicsWorld * world) {
	GameSence* pRet = new(std::nothrow) GameSence();
	if (pRet && pRet->init(world)) {
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	pRet = NULL;
	return NULL;
}

void GameSence::preloadMusic() {
	SimpleAudioEngine::getInstance()->preloadBackgroundMusic("music/bgm.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("music/meet_stone.wav");
}

void GameSence::playBgm() {
	SimpleAudioEngine::getInstance()->playBackgroundMusic("music/bgm.mp3", true);
}

void GameSence::addEdge() {
	auto edgeSp = Sprite::create();
	auto boundBody = PhysicsBody::createEdgeBox(visibleSize);
	boundBody->setDynamic(false);
	boundBody->setTag(0);
	edgeSp->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
	edgeSp->setPhysicsBody(boundBody);
	this->addChild(edgeSp);
}

void GameSence::addPlayer() {
	player = Sprite::create();
	player->setAnchorPoint(Vec2(0.5, 0.5));
	player->setPhysicsBody(PhysicsBody::createCircle(35));
	auto iden = Label::createWithTTF("Player", "fonts/arial.ttf", 18);
	player->addChild(iden);
	player->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);
	player->setPosition(visibleSize / 2);
	player->getPhysicsBody()->setTag(1);
	player->getPhysicsBody()->setAngularVelocityLimit(0);
	addChild(player);
}

void GameSence::hp() {
	Sprite* sp0 = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(0, 320, 420, 47)));
	Sprite* sp = Sprite::create("hp.png", CC_RECT_PIXELS_TO_POINTS(Rect(610, 362, 4, 16)));

	pT = ProgressTimer::create(sp);
	pT->setScaleX(90);
	pT->setAnchorPoint(Vec2(0, 0));
	pT->setType(ProgressTimerType::BAR);
	pT->setBarChangeRate(Point(1, 0));
	pT->setMidpoint(Point(0, 1));
	pT->setPercentage(50);
	pT->setPosition(Vec2(origin.x + 14 * pT->getContentSize().width, origin.y + visibleSize.height - 2 * pT->getContentSize().height));
	addChild(pT, 2);
	sp0->setAnchorPoint(Vec2(0, 0));
	sp0->setPosition(Vec2(origin.x + pT->getContentSize().width, origin.y + visibleSize.height - sp0->getContentSize().height));
	addChild(sp0, 1);
}

void GameSence::addContactListener() {
	auto touchListener = EventListenerPhysicsContact::create();
	touchListener->onContactBegin = CC_CALLBACK_1(GameSence::onConcactBegan, this);
	_eventDispatcher->addEventListenerWithFixedPriority(touchListener, 1);
}

void GameSence::addKeyboardListener() {
	auto keboardListener = EventListenerKeyboard::create();
	keboardListener->onKeyPressed = CC_CALLBACK_2(GameSence::onKeyPressed, this);
	keboardListener->onKeyReleased = CC_CALLBACK_2(GameSence::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keboardListener, this);
}

bool GameSence::onConcactBegan(PhysicsContact& contact) {
	auto bodyA = contact.getShapeA()->getBody();
	auto bodyB = contact.getShapeB()->getBody();
	auto A = bodyA;
	auto B = bodyB;
	if (bodyA && bodyB) {
		if (bodyA->getTag() == 1 || bodyB->getTag() == 1) {
			if (bodyA->getTag() == 1) {
				A = bodyA;
				B = bodyB;
			}
			if (bodyB->getTag() == 1) {
				A = bodyB;
				B = bodyA;
			}

			// if player meet enemy,enemy explode and remove it
			auto pos = B->getNode()->getPosition();
			B->getNode()->removeFromParentAndCleanup(true);
			enemys.eraseObject(B);
			SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav", false);
			auto explode = ParticleSystemQuad::create("explode.plist");
			explode->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
			explode->setPosition(pos);
			this->addChild(explode);

			// if meet bigger circle,reduce blood; if meet smaller circle,increase blood
			if (B->getTag() == 10) {
				pT->setPercentage(pT->getPercentage() + 10.0);
			}
			if (B->getTag() == 30) {
				pT->setPercentage(pT->getPercentage() - 10.0);
			}

			// if have no blood,player explode and remove it
			if (pT->getPercentage() <= 0) {
				time->setString("You Lose !");

				auto playerExplode = ParticleSystemQuad::create("explode.plist");
				playerExplode->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
				playerExplode->setPosition(A->getNode()->getPosition());
				this->addChild(playerExplode);

				player->removeFromParentAndCleanup(true);

				unschedule(schedule_selector(GameSence::update));
				_eventDispatcher->removeAllEventListeners();

				// display menu with "return" and "restart"
				menu();
			}

			// if have full blood,all enemys explode and remove them
			if (pT->getPercentage() >= 100) {
				time->setString("You Win !");

				unschedule(schedule_selector(GameSence::update));
				_eventDispatcher->removeAllEventListeners();

				for (int i = 0; i < enemys.size(); i++) {
					if (enemys.at(i)) {
						auto enemyPos = enemys.at(i)->getNode()->getPosition();
						enemys.at(i)->getNode()->removeFromParentAndCleanup(true);
						SimpleAudioEngine::getInstance()->playEffect("music/meet_stone.wav", false);
						auto enemyExplode = ParticleSystemQuad::create("explode.plist");
						enemyExplode->setPositionType(ParticleSystemQuad::PositionType::RELATIVE);
						enemyExplode->setPosition(enemyPos);
						this->addChild(enemyExplode);
					}
				}
				enemys.clear();

				menu();
			}
		}
	}
	return true;
}

void GameSence::menu() {
	// return to MenuSence
	auto start = MenuItemFont::create("Return", CC_CALLBACK_1(GameSence::startMenuCallback, this));
	start->setPosition(Vec2(origin.x + visibleSize.width / 2, visibleSize.height / 2 + origin.y));
	auto menu = Menu::create(start, NULL);
	menu->setPosition(Vec2::ZERO);
	addChild(menu);

	// restart
	auto start1 = MenuItemFont::create("Restart", CC_CALLBACK_1(GameSence::reStartCallback, this));
	start1->setPosition(Vec2(origin.x + visibleSize.width / 2, visibleSize.height / 2 + origin.y - 40));
	auto menu1 = Menu::create(start1, NULL);
	menu1->setPosition(Vec2::ZERO);
	addChild(menu1);
}

void GameSence::update(float f) {
	newEnemys();
}

void GameSence::newEnemys() {
	if (enemys.size() > 20) return;
	int newNum = 2;
	while (newNum--) {
		// add one of three kinds of circles
		int type = 0;
		if (CCRANDOM_0_1() > 0.70) { type = 2; }
		else if (CCRANDOM_0_1() > 0.35) { type = 1; }

		// new circles will appear from four different places
		Point location = Vec2(0, 0);
		switch (rand() % 4)
		{
		case 0:
			location.y = visibleSize.height;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 1:
			location.x = visibleSize.width;
			location.y = rand() % (int)(visibleSize.height);
			break;
		case 2:
			location.y = 0;
			location.x = rand() % (int)(visibleSize.width);
			break;
		case 3:
			location.x = 0;
			location.y = rand() % (int)(visibleSize.height);
			break;
		default:
			break;
		}
		addEnemy(type, location);
	}
}

void GameSence::addEnemy(int type, Point p) {
	int tag, size;
	switch (type)
	{
	case 0:
		size = 20;
		tag = 10;
		break;
	case 1:
		size = 35;
		tag = 20;
		break;
	case 2:
		size = 50;
		tag = 30;
		break;
	default:
		size = 15;
		tag = 10;
		break;
	}
	auto re = Sprite::create();
	re->setPhysicsBody(PhysicsBody::createCircle(size));
	re->setAnchorPoint(Vec2(0.5, 0.5));
	re->setPosition(p);
	re->getPhysicsBody()->setContactTestBitmask(0xFFFFFFFF);

	re->getPhysicsBody()->setTag(tag);
	if (size < 35) {
		re->getPhysicsBody()->setVelocity((player->getPosition() - p) * (0.25));
	}
	else {
		re->getPhysicsBody()->setVelocity((Point(rand() % (int)(visibleSize.width - 100) + 50, rand() % (int)(visibleSize.height - 100) + 50) - p) * (0.25));
	}
	re->getPhysicsBody()->setAngularVelocity(CCRANDOM_0_1() * 10);
	enemys.pushBack(re->getPhysicsBody());
	addChild(re);
}

void GameSence::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->getPhysicsBody()->setVelocity(Point(-200, player->getPhysicsBody()->getVelocity().y));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->getPhysicsBody()->setVelocity(Point(200, player->getPhysicsBody()->getVelocity().y));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, 200));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		player->getPhysicsBody()->setVelocity(Point(player->getPhysicsBody()->getVelocity().x, -200));
		break;
	default:
		break;
	}
}

void GameSence::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(-200, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(200, 0));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, 200));
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		player->getPhysicsBody()->setVelocity(player->getPhysicsBody()->getVelocity() - Point(0, -200));
		break;
	default:
		break;
	}
}

void GameSence::startMenuCallback(Ref* pSender) {
	auto myScene = MenuSence::createScene();
	Director::getInstance()->replaceScene(TransitionFlipX::create(1.5, myScene));
}

void GameSence::reStartCallback(Ref* pSender) {
	auto myScene = GameSence::createScene();
	Director::getInstance()->replaceScene(TransitionFlipX::create(1.5, myScene));
}
