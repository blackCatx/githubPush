#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "Progress.h"
USING_NS_CC;

class MainScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(MainScene);

	bool onTouchBegan(Touch *touch, Event * pevent);
	void onTouchPause(Ref* sender);
	void onTouchResume();
	void setPhysicsWorld(PhysicsWorld *pPhysicsWorld){ _world = pPhysicsWorld;}
	void toggleDebug(Ref* pSender);
	void onEnter();
	bool onContactBegin(const PhysicsContact& contact);
	void onContactSeperate(const PhysicsContact& contact);
	void clickEnemy(Ref* obj);

private:
	Player *_player;
	Player *_enemy1;
	EventListenerTouchOneByOne * _listener_touch;
	EventListenerPhysicsContact *_listener_contact;
	Progress *_progress;
	Player *_enemy2;
	Menu *_menu;
	PhysicsWorld * _world;
};

#endif // __HELLOWORLD_SCENE_H__
