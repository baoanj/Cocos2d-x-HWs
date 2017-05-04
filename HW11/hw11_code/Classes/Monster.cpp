#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;

Factory::Factory() {
	initSpriteFrame();
}

Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}

//初始化怪物帧动画
void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

//生成一个怪物，并存储到容器中管理
Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

//移除怪物
void Factory::removeMonster(Sprite* sp) {
	Animation* anim = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	Animate* ani = Animate::create(anim);
	Sequence* seq = Sequence::create(ani, CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, sp)), NULL);
	sp->runAction(seq);
	monster.eraseObject(sp);
}

//让容器中的所有怪物都往角色移动，通过容器管理所有的怪物很方便
void Factory::moveMonster(Vec2 playerPos, float time) {
	for (auto sp : monster) {
		auto moveTo = MoveTo::create(time, playerPos);
		if (sp == nullptr) continue;
		sp->runAction(moveTo);
	}
}

//判断碰撞
Sprite* Factory::collider(Rect rect) {
	for (auto sp : monster) {
		if (sp == nullptr) continue;
		if (rect.containsPoint(sp->getPosition())) {
			return sp;
		}
	}
	return NULL;
}