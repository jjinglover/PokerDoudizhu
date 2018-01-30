#include "PokerNode.h"

PokerNode::PokerNode()
:m_info(nullptr)
, m_img(nullptr)
, m_startPosX(0)
, m_moveSelected(false)
, m_canClicked(true)
{
}

PokerNode::~PokerNode()
{
	
}

PokerNode *PokerNode::create(PokerInfo *info)
{
	auto pRet = new PokerNode();
	if (pRet&&pRet->init(info))
	{
		pRet->autorelease();
		return pRet;
	}
	delete pRet;
	return nullptr;
}

bool PokerNode::init(PokerInfo *info)
{
	if (!Node::init())
	{
		return false;
	}
	m_info = info;

	m_img = ImageView::create();
	this->addChild(m_img);
	m_img->setTouchEnabled(true);
	m_img->addClickEventListener(CC_CALLBACK_1(PokerNode::onImgClicked, this));

	this->refresh(info);
	return true;
}

void PokerNode::refresh(PokerInfo *info)
{
	m_info = info;
	publicFunc::updatePokerIcon(m_img, *info);
}

void PokerNode::onImgClicked(Ref *ref)
{
	if (m_canClicked)
	{
		this->selectOrNot(!m_info->selected);
	}
}

void PokerNode::clickEnabled(bool enabled)
{
	m_canClicked=enabled;
}

void PokerNode::selectOrNot(bool select)
{
	if (select)
	{
		this->setPositionY(20);
	}
	else{
		this->setPositionY(0);
	}
	m_info->selected = select;
}

void PokerNode::began(const Vec2 &pos)
{
	m_startPosX = this->convertToNodeSpace(pos).x;
	m_moveSelected = false;
	this->clickEnabled(true);
}

void PokerNode::move(const Vec2 &pos)
{
	auto nPos = this->convertToNodeSpace(pos);
	auto contentSz = m_img->getContentSize();
	int left_x = -contentSz.width*0.5;
	auto x = left_x+POKER_PILE_UP_VIEW_WIDTH*0.5;
	m_moveSelected = false;
	if (fabs(m_startPosX - nPos.x) >= POKER_PILE_UP_VIEW_WIDTH*0.5)
	{
		if (m_startPosX > nPos.x)
		{
			if (m_startPosX >= left_x + POKER_PILE_UP_VIEW_WIDTH)
			{
				m_moveSelected = nPos.x < x;
			}
		}
		else{
			if (m_startPosX <= x)
			{
				m_moveSelected = nPos.x > x;
			}
		}
		this->clickEnabled(false);
	}
	else{
		this->clickEnabled(true);
	}

	
	m_img->setOpacity(255 - 50 * m_moveSelected);
}

void PokerNode::end()
{
	if (m_moveSelected)
	{
		m_img->setOpacity(255);
		this->selectOrNot(!m_info->selected);
	}	
}