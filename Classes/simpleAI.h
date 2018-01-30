#ifndef __simpleAI__
#define __simpleAI__

#include "publicHeader.h"

class simpleAI
{
public:
	static simpleAI* getInstance();
	std::vector<PokerInfo> getMoreBigPokers(const std::vector<PokerInfo> &goalVec, const CheckInfo &info,bool isFriend);

protected:
	void searchSingle();
	void searchDouble();
	void searchThree();
	void searchThreeWithS();
	void searchThreeWithD();

	void searchStraight();
	void searchStraightD();
	void searchStraightThree();
	void searchStraightThreeWithS();
	void searchStraightThreeWithD();

	void searchFourTwoS();
	void searchFourTwoD();
	void searchBoom();

	void resetData();
private:
	std::map<int, int> m_typeNumMap; //单/双/三/四 的数量
	std::map<int, int> m_valueNumMap;//每种数值的牌有几张
	std::vector<std::pair<int, int>> m_searchResult;
	
	CheckInfo          m_checkInfo;
	bool               m_isFriend;

	int                m_boomMinValue, m_boomMaxValue;
	bool               m_haveKingBoom;
	int                m_selfSurplusNum;
	std::pair<int, int> m_pair;
};

#endif