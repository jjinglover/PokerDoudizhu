#ifndef __GameMainLayer__
#define __GameMainLayer__

#include "publicHeader.h"
#include "PokerNode.h"

class GameMainLayer : public Layer
{
public:
	GameMainLayer();
	~GameMainLayer();

    CREATE_FUNC(GameMainLayer);
	virtual bool init();
	
	virtual bool onTouchBegan(Touch *touch, Event *unused_event);
	virtual void onTouchMoved(Touch *touch, Event *unused_event);
	virtual void onTouchEnded(Touch *touch, Event *unused_event);
	virtual void onTouchCancelled(Touch *touch, Event *unused_event);
protected:
	void initBasicShow();
	void initPoker();

	void onBackClicked();
	void onPassedClicked();
	void onSureClicked();
	void onPromptClicked();

	void distributePoker();
	void updateBottomPoker();
	void updatePlayerState(RoleType type);
	void cutDown(cocos2d::Node *);

	void updateKnockOutPokerShow();
	void updateSelfPokerShow();
private:
	cocos2d::Node *m_mainNode;
	EventListenerTouchOneByOne *m_touchLister;
	std::vector<PokerInfo> m_initPokerVec, m_knockOutPokersVec;
	std::map<RoleType, std::vector<PokerInfo>> m_pokersMap;
	std::vector<PokerNode *> m_selfPokers;

	int  m_curThinkPlayer;
	int  m_cd;
};

#endif // __GameMainLayer__
