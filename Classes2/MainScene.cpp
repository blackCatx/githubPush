#include "MainScene.h"
#include "PauseLayer.h"
#include "Player.h"
#include "FSM.h"
#include "VisibleRect.h"
#include "CustomTool.h"
Scene* MainScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	    
    // 'layer' is an autorelease object
    auto layer = MainScene::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    auto background = Sprite::create("image/background.png");
	background->setPosition( origin + visibleSize / 2);
	this->addChild(background);

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("image/role.plist", "image/role.pvr.ccz");
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("image/ui.plist","image/ui.pvr.ccz");

	_player = Player::create(Player::PlayerType::PLAYER);
	_player->setPosition(origin.x + _player->getContentSize().width/2, origin.y + visibleSize.height/2);
	this->addChild(_player);

	_enemy1 = Player::create(Player::PlayerType::ENEMY1);
	_enemy1->setPosition(origin.x + visibleSize.width - _player->getContentSize().width/2, origin.y + visibleSize.height/2);
	this->addChild(_enemy1);

	_enemy2 = Player::create(Player::PlayerType::ENEMY2);
	_enemy2->setPosition(VisibleRect::right().x*2/3 - _player->getContentSize().width/2, VisibleRect::top().y/2);
	this->addChild(_enemy2);
	
	_listener_contact = EventListenerPhysicsContact::create();
	_listener_contact->onContactBegin = CC_CALLBACK_1(MainScene::onContactBegin, this);
	_listener_contact->onContactSeperate = CC_CALLBACK_1(MainScene::onContactSeperate, this);
	_eventDispatcher->addEventListenerWithFixedPriority(_listener_contact, 10);

	//_player->playAnimationForever(1);
//	_enemy1->playAnimationForever(1);
	//_enemy2->playAnimationForever(1);

	_listener_touch = EventListenerTouchOneByOne::create();
	_listener_touch->onTouchBegan = CC_CALLBACK_2(MainScene::onTouchBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener_touch, this);
	auto fsm = FSM::create("idle",[](){cocos2d::log("Enter idle");});

	NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(MainScene::clickEnemy),"clickEnemy",nullptr);

	_progress = Progress::create("player-progress-bg.png", "player-progress-fill.png");
	_progress->setPosition(VisibleRect::left().x + _progress->getContentSize().width/2, VisibleRect::top().y - _progress->getContentSize().height/2);
	this->addChild(_progress);

	
	auto pauseItem = CustomTool::createMenuItemImage("pause1.png", "pause2.png", CC_CALLBACK_1(MainScene::onTouchPause, this));
	pauseItem->setTag(1);
	pauseItem->setPosition(VisibleRect::right().x - pauseItem->getContentSize().width/2, VisibleRect::top().y - pauseItem->getContentSize().height/2);

	auto debugItem = MenuItemImage::create("CloseNormal.png","CloseSelected.png", 
		CC_CALLBACK_1(MainScene::toggleDebug, this));

	debugItem->setScale(2.0);
	debugItem->setPosition(Vec2(VisibleRect::right().x - debugItem->getContentSize().width - pauseItem->getContentSize().width ,
		VisibleRect::top().y - debugItem->getContentSize().height));
	_menu = Menu::create(pauseItem, debugItem,  NULL);
	_menu->setPosition(0,0);
	this->addChild(_menu);




    return true;
}

void MainScene::onTouchPause(Ref* sender)
{

	_player->pause();
	_enemy1->pause();
	_enemy2->pause();
	 auto layer = PauseLayer::create();
	 this->addChild(layer,100);
}

void MainScene::onTouchResume()
{
		_player->resume();
		_enemy1->resume();
		_enemy2->resume();
}
void MainScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

bool MainScene::onTouchBegan( Touch *touch, Event * pevent )
{
	Vec2 pos = this->convertToNodeSpace(touch->getLocation());
	_player->walkTo(pos);
	return true;
}

void MainScene::onEnter()
{
	Layer::onEnter();
	_world->setGravity(Vec2(0,0));

}

bool MainScene::onContactBegin( const PhysicsContact& contact )
{
	auto playerA = (Player*)contact.getShapeA()->getBody()->getNode();
	auto playerB = (Player*)contact.getShapeB()->getBody()->getNode();
	auto typeA = playerA->getPlayerType();
	auto typeB = playerB->getPlayerType();
	if (typeA == Player::PlayerType::PLAYER)
	{
		CCLOG("contact enemy!");
		playerB->setCanAttack(true);
	} 
	if (typeB == Player::PlayerType::PLAYER)
	{
		CCLOG("contact enemy!B");
		playerA->setCanAttack(true);
	}
	return true;
	
}

void MainScene::onContactSeperate( const PhysicsContact& contact )
{

	auto playerA = (Player*) contact.getShapeA()->getBody()->getNode();
	auto playerB = (Player*) contact.getShapeB()->getBody()->getNode();
	auto typeA = playerA->getPlayerType();
	auto typeB = playerB->getPlayerType();
	if (typeA == Player::PlayerType::PLAYER)
	{
		CCLOG("leave enemy!");
		playerB->setCanAttack(false);
	}
	if (typeB == Player::PlayerType::PLAYER)
	{
		CCLOG("leave enemy!B");
		playerA->setCanAttack(false);
	}
	
	
}

void MainScene::clickEnemy( Ref* obj )
{
	log("click enemy message received!");
	auto enemy = (Player*)obj;
	if (enemy == nullptr)
	{
		log("enemy null");
		return;
	}
	if (enemy->isCanAttack())
	{
		_player->attack();
		enemy->beHit(_player->getAttack());

	}
	else
	{
		_player->walkTo(enemy->getPosition());

	}
	
}

void MainScene::toggleDebug( Ref* pSender )
{
	if (_world->getDebugDrawMask() != PhysicsWorld::DEBUGDRAW_NONE)
	{
		_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
	} 
	else
	{
		_world->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	}
	
}
