
#include "Player.h"
Player::Player(void)
{
}


Player::~Player(void)
{
}



bool Player::initWithPlayerType( PlayerType pType )
{
	std::string spName = "";
	_type = pType;
	_speed = 100;
	_seq = nullptr;
	int animationFrameNum[5] = {4, 4, 4, 2, 4};
	int animationFrameNum2[5] = {3, 3, 3, 2, 0};
	_isShowBar = true;
	_isCanAttack = false;
	_health = 100;
	_maxHealth = 100;
	_attack = 24;


	
	switch (pType)
	{
	case PlayerType::PLAYER:
		spName = "player1-1-1.png";
		_name = "player1";
		_animationNum = 5;
		_animationFrameNum.assign(animationFrameNum, animationFrameNum + 5);
		_speed = 125;
		_isShowBar = false;
		break;
	case PlayerType::ENEMY1:
		spName = "enemy1-1-1.png";
		_name = "enemy1";
		_animationNum = 4;
		_animationFrameNum.assign(animationFrameNum2, animationFrameNum2 + 5);
		_isShowBar = true;
		break;
	case PlayerType::ENEMY2:
		spName = "enemy2-1-1.png";
		_name = "enemy2";
		_animationNum = 4;
		_animationFrameNum.assign(animationFrameNum2, animationFrameNum2 + 5);
		_isShowBar = true;
		break;
	default:
		break;
	}

	this->initWithSpriteFrameName(spName);
	std::string animationNames[] = {"walk", "attack", "dead", "hit", "skill" };
	_animationNames.assign(animationNames, animationNames + 5);
	
	this->addAnimation();
	auto size = this->getContentSize();
	auto body = PhysicsBody::createBox(Size(size.width/2, size.height));
	body->setCollisionBitmask(0);
	body->setContactTestBitmask(1);
	this->setPhysicsBody(body);

	this->initFSM();

	_progress = Progress::create("small-enemy-progress-bg.png", "small-enemy-progress-fill.png");
	_progress->setPosition( size.width*2/3, size.height + _progress->getContentSize().height/2);
	this->addChild(_progress);
	if (!_isShowBar)
	{
		_progress->setVisible(false);
	}

	_listener = EventListenerTouchOneByOne::create();
	_listener->setSwallowTouches(true);
	_listener->onTouchBegan = CC_CALLBACK_2(Player::onTouch, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

	return true;
}

Player* Player::create( PlayerType ptype )
{
	Player* player = new Player();
	if (player && player->initWithPlayerType(ptype))
	{
		player->autorelease();
		return player;
	} 
	else
	{
		delete player;
		player = nullptr;
		return nullptr;
	}
}

void Player::addAnimation()
{
	auto animation = AnimationCache::getInstance()->getAnimation(String::createWithFormat("%s-%s", _name.c_str(),
		_animationNames[0])->getCString());
	if (animation)
	{
		return;
	}
	for (int i = 0; i < _animationNum; i++)
	{
		auto animation = Animation::create();
		animation->setDelayPerUnit(0.2f);

		for (int j = 0; j < _animationFrameNum[i]; j++)
		{
			auto sfName = String::createWithFormat("%s-%d-%d.png", _name.c_str(), i+1, j+1)->getCString();
			animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(sfName));
		}
		AnimationCache::getInstance()->addAnimation(animation, 
			String::createWithFormat("%s-%s", _name.c_str(), _animationNames[i].c_str())->getCString());
	}
}

void Player::playAnimationForever(int index)
{
/*
	if (index < 0 || index >= _animationNum)
	{
		return;
	}
	auto str = String::createWithFormat("%s-%s", _name.c_str(), _animationNames[index].c_str())->getCString();
	auto animation = AnimationCache::getInstance()->getAnimation(str);
	auto animate = Animate::create(animation);
	this->runAction(RepeatForever::create(animate));*/
	auto act = this->getActionByTag(index);
	if(act)
		return;

	for(int i=0;i<5;i++)
	{
		this->stopActionByTag(i);
	}
	if(index <0 || index >= _animationNum)
	{
		log("illegal animation index!");
		return;
	}
	auto str = String::createWithFormat("%s-%s",_name.c_str(), _animationNames[index].c_str())->getCString();
	auto animation = AnimationCache::getInstance()->getAnimation(str);
	auto animate = RepeatForever::create(Animate::create(animation));
	animate->setTag(index);
	this->runAction(animate);
}

void Player::walkTo( Vec2 dest)
{
	
//	if (_seq)
// 		this->stopAction(_seq);
// 
// 	auto curPos = this->getPosition();
// 	if (curPos.x > dest.x)
// 	{
// 		this->setFlippedX(true);
// 	}
// 	else
// 	{
// 		this->setFlippedX(false);
// 	}
// 	auto diff = dest - curPos;
// 	auto time = diff.getLength() / _speed;
// 	auto move = MoveTo::create(time,dest);
// 	auto func = [&](){
// 		this->stopAllActions();
// 		_seq = nullptr;
// 	};
// 	auto callback = CallFunc::create(func);
// 	_seq = Sequence::create(move, callback, nullptr);
// 	
// 	this->runAction(_seq);
// 	this->playAnimationForever(0);
	std::function<void()> onWalk = CC_CALLBACK_0(Player::onWalk, this, dest);
	_fsm->setOnEnter("walking", onWalk);
	_fsm->doEvent("walk");

}

Animate* Player::getAnimateByType(AnimationType type)
{
	if (type < 0 || type >= _animationNum)
	{
		log("illegal animation index!");
		return nullptr;
	}
	auto str = String::createWithFormat("%s-%s", _name.c_str(), _animationNames[type].c_str())->getCString();
	auto animation = AnimationCache::getInstance()->getAnimation(str);
	auto animate = Animate::create(animation);
	animate->setTag(type);
	return animate;
}

void Player::initFSM()
{
	_fsm = FSM::create("idle");
	_fsm->retain();
	auto onIdel =[&]()
	{
		log("onIdle: Enter idle");
		this->stopActionByTag(WALKING);
		auto sfName = String::createWithFormat("%s-1-1.png", _name.c_str());
		auto spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(sfName->getCString());
		this->setSpriteFrame(spriteFrame);


	};
	_fsm->setOnEnter("idle", onIdel);


	auto onAttacking =[&]()
	{
		log("onAttacking: Enter Attacking");
		auto animate = getAnimateByType(ATTACKING);
		auto func = [&]()
		{
			this->_fsm->doEvent("stop");
		};
		auto callback = CallFunc::create(func);
		auto seq = Sequence::create(animate, callback, nullptr);
		this->runAction(seq);
	};
	_fsm->setOnEnter("attacking",onAttacking);


	auto onBeingHit = [&]()
	{
		log("onBeingHit: Enter BeingHit");
		auto animate = getAnimateByType(BEINGHIT);
		auto func = [&]()
		{
			this->_fsm->doEvent("stop");
		};
		auto wait = DelayTime::create(0.6f);
		auto callback = CallFunc::create(func);
		auto seq = Sequence::create(wait,animate, callback, nullptr);
		this->runAction(seq);
	};
	_fsm->setOnEnter("beingHit",onBeingHit);

	auto onDead = [&]()
	{
		log("onDead: Enter Dead");
		auto animate = getAnimateByType(DEAD);
		auto func = [&]()
		{
			log("A charactor died!");
			NotificationCenter::getInstance()->postNotification("ENEMY_DEAD",nullptr);
			this->removeFromParentAndCleanup(true);
		};
		auto blink = Blink::create(3,5);
		auto callback = CallFunc::create(func);
		auto seq = Sequence::create(animate, blink, callback, nullptr);
		this->runAction(seq);
		_progress->setVisible(false);
	};
	_fsm->setOnEnter("dead",onDead);
}
void Player::onWalk(Vec2 dest)
{
	log("onIdle: Enter walk");
	this->stopActionByTag(WALKTO_TAG);
	auto curPos = this->getPosition();

	if (curPos.x > dest.x)
	{
		this->setFlippedX(true);
	} 
	else
	{
		this->setFlippedX(false);
	}
	auto diff = dest - curPos;
	auto time = diff.getLength()/_speed;
	auto move = MoveTo::create(time, dest);
	auto func = [&]()
	{
		this->_fsm->doEvent("stop");
	};
	auto callback = CallFunc::create(func);
	auto seq = Sequence::create(move, callback, nullptr);
	seq->setTag(WALKTO_TAG);
	this->runAction(seq);
//	this->playAnimationForever(0);
	this->playAnimationForever(WALKING);
}

void Player::onExit()
{
	Sprite::onExit();
	_fsm->release();
}

bool Player::onTouch(Touch* touch, Event* event)
{
	if(_type == PLAYER)
		return false;
	log("Player: touch detected!");
	auto pos = this->convertToNodeSpace(touch->getLocation());
	auto size = this->getContentSize();
	auto rect = Rect(size.width/2, 0, size.width, size.height);
	if(rect.containsPoint(pos))
	{

		NotificationCenter::getInstance()->postNotification("clickEnemy", this);
		log("enemy touched!");
		return true;

	}
	log("enemy not touched!");
	return false;

}

void Player::attack()
{
	_fsm->doEvent("attack");

}

void Player::beHit(int attack)
{

	_health -= attack;
	if (_health <= 0)
	{
		_health = 0;
		this->_progress->setProgress((float)_health/_maxHealth*100);
		_fsm->doEvent("die");
		return;
	}
	else
	{
		this->_progress->setProgress((float)_health/_maxHealth*100);
		_fsm->doEvent("beHit");
	}
}
