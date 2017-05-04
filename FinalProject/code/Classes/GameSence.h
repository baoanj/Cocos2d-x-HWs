#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class GameSence :public Layer {
public:
	void setPhysicsWorld(PhysicsWorld * world);
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init(PhysicsWorld* world);

	// implement the "static create()" method manually
	static GameSence* create(PhysicsWorld* world);

private:
	Sprite* player;
	PhysicsWorld* m_world;
	Size visibleSize;
	cocos2d::Vec2 origin;
	Vector<PhysicsBody*> enemys;

	cocos2d::Label* time;
	cocos2d::ProgressTimer* pT;

	void preloadMusic();
	void playBgm();

	void addEdge();
	void addPlayer();

	void hp();

	void addContactListener();
	void addKeyboardListener();

	void update(float f);

	bool onConcactBegan(PhysicsContact& contact);
	void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	void newEnemys();
	void addEnemy(int type, Point p);

	void menu();
	void startMenuCallback(Ref* pSender);
	void reStartCallback(Ref* pSender);
};