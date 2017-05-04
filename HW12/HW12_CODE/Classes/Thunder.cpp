#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();

	// player移动
	move = 0;

	// 用于判断“按住空格不放，一颗子弹消失后会继续发射子弹”
	firing = false;

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
    // bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, \
    //     visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

    addEnemy(5);

    preloadMusic();
    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

    // TODO
    // add schedule
	schedule(schedule_selector(Thunder::update), 0.1f, kRepeatForever, 0);
    
    return true;
}

void Thunder::preloadMusic() {
    // 预加载资源
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

void Thunder::playBgm() {
    // 播放背景音乐
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * (i + 1));
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::addKeyboardListener() {
    // 添加键盘事件监听器
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, player);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener->clone(), player);
}

void Thunder::update(float f) {
	// 检查player是否出边界
	Vec2 newPosition = player->getPosition() + Vec2(move, 0);
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (newPosition.x >= origin.x && newPosition.x <= origin.x + visibleSize.width) {
		player->setPosition(newPosition);
	}

	// 按住空格不放，一颗子弹消失后会继续发射子弹
	if (firing == true) {
		fire();
	}

	static double count = 0;
	static int dir = 5;
	count += f;
	if (count > 2) { count = 0.0; dir = -dir; }

	if (bullet != NULL) {
		bullet->setPosition(bullet->getPositionX(), bullet->getPositionY() + 30);
		if (bullet->getPositionY() > visibleSize.height - 10) {
			bullet->removeFromParentAndCleanup(true);
			bullet = NULL;
		}
	}

	for (unsigned i = 0; i < enemys.size(); i++) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			if (bullet != NULL && bullet->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
				EventCustom e("meet");
				e.setUserData(&i);
				_eventDispatcher->dispatchEvent(&e);
			}
		}
    }
}

void Thunder::fire() {
    if (bullet != NULL) return;
    bullet = Sprite::create("bullet.png");
    bullet->setPosition(player->getPosition());
	addChild(bullet);
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::meet(EventCustom* event) {
	bullet->removeFromParentAndCleanup(true);
	bullet = NULL;
	int x = *(int*)(event->getUserData());
	enemys[x]->removeFromParentAndCleanup(true);
	enemys[x] = NULL;
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::addCustomListener() {
    // 添加自定义事件监听器
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 1);
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
			player->setPosition(player->getPositionX() - 1, player->getPositionY());
			move -= 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
			player->setPosition(player->getPositionX() + 1, player->getPositionY());
			move += 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
			firing = true;
            break;
        default:
            break;
    }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		player->setPosition(player->getPositionX() - 1, player->getPositionY());
		move += 5;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		player->setPosition(player->getPositionX() + 1, player->getPositionY());
		move -= 5;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		firing = false;
		break;
	default:
		break;
	}
}
