#include "GameMainLayer.h"
#include "simpleAI.h"

GameMainLayer::GameMainLayer()
:m_mainNode(nullptr), m_curThinkPlayer(RoleType::Role_self)
{

}

GameMainLayer::~GameMainLayer()
{
	_eventDispatcher->removeEventListener(m_touchLister);
}

bool GameMainLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}
	m_mainNode = CSLoader::createNode("PokerGame/GameLayer.csb");
	this->addChild(m_mainNode);

	srand(time(nullptr));

	this->initBasicShow();
	this->initPoker();
	this->distributePoker();
	
	m_touchLister = EventListenerTouchOneByOne::create();
	m_touchLister->onTouchBegan = CC_CALLBACK_2(GameMainLayer::onTouchBegan, this);
	m_touchLister->onTouchMoved = CC_CALLBACK_2(GameMainLayer::onTouchMoved, this);
	m_touchLister->onTouchEnded = CC_CALLBACK_2(GameMainLayer::onTouchEnded, this);
	m_touchLister->onTouchCancelled = CC_CALLBACK_2(GameMainLayer::onTouchCancelled, this);
	_eventDispatcher->addEventListenerWithFixedPriority(m_touchLister, -1);
	return true;
}

bool GameMainLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
	for (auto it : m_selfPokers)
	{
		it->began(touch->getLocation());
	}
	return true;
}

void GameMainLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
	for (auto it:m_selfPokers)
	{
		it->move(touch->getLocation());
	}
}

void GameMainLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
	for (auto it : m_selfPokers)
	{
		it->end();
	}
}

void GameMainLayer::onTouchCancelled(Touch *touch, Event *unused_event)
{
	this->onTouchEnded(touch, unused_event);
}

void GameMainLayer::initBasicShow()
{
	auto btn = static_cast<Button *>(Helper::seekWidgetByName(m_mainNode, "Button_Back"));
	btn->addClickEventListener(CC_CALLBACK_0(GameMainLayer::onBackClicked, this));

	btn = static_cast<Button *>(Helper::seekWidgetByName(m_mainNode, "Button_Prompt"));
	btn->addClickEventListener(CC_CALLBACK_0(GameMainLayer::onPromptClicked, this));

	btn = static_cast<Button *>(Helper::seekWidgetByName(m_mainNode, "Button_Pass"));
	btn->addClickEventListener(CC_CALLBACK_0(GameMainLayer::onPassedClicked, this));

	btn = static_cast<Button *>(Helper::seekWidgetByName(m_mainNode, "Button_Sure"));
	btn->addClickEventListener(CC_CALLBACK_0(GameMainLayer::onSureClicked, this));
}

void GameMainLayer::initPoker()
{
	std::vector<int> typeVec = { 1, 2, 3, 4 };
	std::vector<int> valueVec;
	for (auto i = 3; i <15 ; i++){
		valueVec.push_back(i);
	}
	valueVec.push_back(POKER_VALUE_2);
	//--------
	PokerInfo info;
	for (auto type:typeVec)
	{
		for (auto value:valueVec)
		{
			info.color_type = type;
			info.value = value;
			m_initPokerVec.push_back(info);
		}
	}
	info.reset();
	info.value = POKER_VALUE_JOKER_SMALL;
	m_initPokerVec.push_back(info);

	info.reset();
	info.value = POKER_VALUE_JOKER_BIG;
	m_initPokerVec.push_back(info);
}

void GameMainLayer::onBackClicked()
{
	this->removeFromParent();
}

void GameMainLayer::onPassedClicked()
{
	for (auto it : m_selfPokers)
	{
		it->selectOrNot(false);
	}
	this->updatePlayerState(RoleType::Role_self);
}

void GameMainLayer::onSureClicked()
{
	std::vector<PokerInfo> selectVec;
	auto &vec = m_pokersMap[RoleType::Role_self];
	for (auto it : vec)
	{
		if (it.selected)
		{
			selectVec.push_back(it);
		}
	}

	if (m_knockOutPokersVec.empty())
	{
		auto info = publicFunc::checkPokersType(selectVec);
		CCLOG("type=%d,minValue=%d,serialNum=%d", info.type, info.min_value, info.serial_num);
		if (info.type == PokerType::Type_none)
		{
			publicFunc::popRedTip(FileUtils::getInstance()->getStringFromFile("tip.json"));
			return;
		}
	}
	else{
		auto info = publicFunc::checkPokersType(m_knockOutPokersVec);
		auto vec = simpleAI::getInstance()->getMoreBigPokers(selectVec,info, false);
		if (vec.empty())
		{
			return;
		}
	}

	m_knockOutPokersVec = selectVec;
	for (auto it = vec.begin(); it != vec.end();)
	{
		if (publicFunc::isHaved(*it, selectVec))
		{
			it=vec.erase(it);
		}
		else{
			++it;
		}
	}

	this->updateSelfPokerShow();
	this->updateKnockOutPokerShow();
	this->updatePlayerState(RoleType::Role_self);
}

void GameMainLayer::onPromptClicked()
{
}

void GameMainLayer::distributePoker()
{
	//unorder
	std::vector<int> orderVec;
	while (true)
	{
		int	var = RandomHelper::random_int(0, 53);
		if (orderVec.size()<54)
		{
			if (!publicFunc::isHaved(var,orderVec))
			{
				orderVec.push_back(var);
			}
		}
		else{
			break;
		}
	}
	//
	int index = 0;
	for (auto i = 0; i < 3; i++)
	{
		for (auto j= 0; j < 17; j++)
		{
			index = i * 17 + j;
			auto &info = m_initPokerVec.at(orderVec.at(index));
			m_pokersMap[(RoleType)(i + 1)/*(RoleType)2*/].push_back(info);
		}
	}
	for (auto i = 51; i < 54; i++)
	{
		auto &info = m_initPokerVec.at(orderVec.at(i));
		m_pokersMap[RoleType::Role_bottom].push_back(info);
	}

	for (auto &it : m_pokersMap)
	{
		publicFunc::sortPoker(it.second);
	}
	
	//-----------
	this->updateBottomPoker();
	this->updateSelfPokerShow();
	this->updatePlayerState(RoleType::Role_none);
}

void GameMainLayer::updateBottomPoker()
{
	auto dibNode = Helper::seekWidgetByName(m_mainNode, "Node_DiB");
	auto difNode = Helper::seekWidgetByName(m_mainNode, "Node_DiF");
	const auto &vec = m_pokersMap[RoleType::Role_bottom];
	bool show = vec.size()>0;
	dibNode->setVisible(!show);
	difNode->setVisible(show);
	if (show)
	{
		char buff[BUFFER_SIZE] = { 0 };
		for (int i = 1; i <=3; i++)
		{
			sprintf(buff, "Sprite_D%d", i);
			auto sp = static_cast<Sprite *>(Helper::seekWidgetByName(difNode, buff));
			publicFunc::updatePokerIcon(sp, vec.at(i - 1));
		}
	}
}

void GameMainLayer::updatePlayerState(RoleType type)
{
	auto preSurT = static_cast<Text *>(Helper::seekWidgetByName(m_mainNode, "Text_SurplusPre"));
	auto nextSurT = static_cast<Text *>(Helper::seekWidgetByName(m_mainNode, "Text_SurplusNext"));
	preSurT->setString(publicFunc::intToString(m_pokersMap[RoleType::Role_pre].size()));
	nextSurT->setString(publicFunc::intToString(m_pokersMap[RoleType::Role_next].size()));
	//---
	if (type==RoleType::Role_pre
		||type==RoleType::Role_self
		||type==RoleType::Role_next){

		if (type == RoleType::Role_self)
		{
			if (m_curThinkPlayer == RoleType::Role_self)
			{
				++m_curThinkPlayer;
			}
			else{
				return;
			}
		}
		else{
			++m_curThinkPlayer;
		}

		if (m_curThinkPlayer>RoleType::Role_next){
			m_curThinkPlayer = RoleType::Role_pre;
		}
	}

	char buff[32] = { 0 };
	for (int i= 1; i<=3;i++)
	{
		sprintf(buff, "FileNode_Time%d", i);
		auto node = Helper::seekWidgetByName(m_mainNode, buff);
		if (node)
		{
			node->stopAllActions();
			if (i == m_curThinkPlayer)
			{
				node->setVisible(true);
				this->cutDown(node);
			}
			else{
				node->setVisible(false);
			}
		}
	}
}

void GameMainLayer::cutDown(cocos2d::Node *node)
{
	auto cutT = static_cast<Text *>(node->getChildByName("Text_Cut"));
	auto func = [=]{
		cutT->setString(publicFunc::intToString(--m_cd));
		if (m_curThinkPlayer == RoleType::Role_self)
		{
			if (m_cd == 0){
				this->onPassedClicked();
			}
		}
		else{
			if (m_cd == AI_THINK_TIME_THRESHOLD){
				auto &goalVec = m_pokersMap[(RoleType)m_curThinkPlayer];
				auto checkInfo = publicFunc::checkPokersType(m_knockOutPokersVec);
				auto vec = simpleAI::getInstance()->getMoreBigPokers(goalVec, checkInfo, false);
				if (!vec.empty())
				{
					for (auto aa : vec)
					{
						for (auto it = goalVec.begin(); it != goalVec.end();)
						{
							if (aa==*it)
							{
								it = goalVec.erase(it);
							}
							else{
								++it;
							}
						}
					}

					m_knockOutPokersVec = vec;
					this->updateKnockOutPokerShow();
				}
				this->updatePlayerState(RoleType::Role_pre);
			}
		}
	};
	m_cd = CAN_THINK_TIME_ADD_ONE;
	func();
	node->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1.f), CallFunc::create(func), nullptr)));
}

void GameMainLayer::updateKnockOutPokerShow()
{
	this->updatePlayerState(RoleType::Role_none);
	auto outNode = Helper::seekWidgetByName(m_mainNode, "Node_KnockOut");
	publicFunc::sortPoker(m_knockOutPokersVec);

	int num = m_knockOutPokersVec.size();
	int intval_x = POKER_PILE_UP_VIEW_WIDTH;
	int posX = -0.5*((num - 1) * intval_x);
	for (int i = 0; i < num; i++)
	{
		auto pt = dynamic_cast<PokerNode *>(outNode->getChildByTag(i));
		if (!pt)
		{
			pt = PokerNode::create(&m_knockOutPokersVec[i]);
			outNode->addChild(pt);
			pt->setTag(i);
		}
		else{
			pt->refresh(&m_knockOutPokersVec[i]);
			pt->setVisible(true);
		}
		pt->setPositionX(posX + i*intval_x);
	}

	while (true)
	{
		auto pt = dynamic_cast<PokerNode *>(outNode->getChildByTag(num++));
		if (pt)
		{
			pt->setVisible(false);
		}
		else{
			break;
		}
	}
}

void GameMainLayer::updateSelfPokerShow()
{
	auto pokerNode = Helper::seekWidgetByName(m_mainNode, "Node_Poker");
	pokerNode->removeAllChildren();
	auto &vec = m_pokersMap[RoleType::Role_self];
	
	m_selfPokers.clear();
	int num = vec.size();
	int intval_x = POKER_PILE_UP_VIEW_WIDTH;
	int posX = -0.5*((num - 1) * intval_x);
	for (int i = 0; i < num; i++)
	{
		auto pt = PokerNode::create(&vec[i]);
		pokerNode->addChild(pt);
		pt->setPositionX(posX + i*intval_x);
		m_selfPokers.push_back(pt);
	}
}