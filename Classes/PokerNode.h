#ifndef __PokerNode__
#define __PokerNode__

#include "publicHeader.h"

class PokerNode:public Node
{
public:
	PokerNode();
	~PokerNode();

	static PokerNode *create(PokerInfo *info);
	void refresh(PokerInfo *info);
	void selectOrNot(bool select);

	void began(const Vec2 &pos);
	void move(const Vec2 &pos);
	void end();
protected:
	bool init(PokerInfo *info);
	void onImgClicked(Ref *ref);
	void clickEnabled(bool enabled);
private:
	PokerInfo	*m_info;
	ImageView   *m_img;
	int          m_startPosX;
	bool         m_moveSelected;
	bool         m_canClicked;
};

#endif