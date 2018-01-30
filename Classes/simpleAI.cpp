 #include "simpleAI.h"

static simpleAI *instance = nullptr;
simpleAI* simpleAI::getInstance()
{
	if (!instance){
		instance = new simpleAI();
	}
	return instance;
}

void simpleAI::resetData()
{
	m_typeNumMap.clear();
	m_valueNumMap.clear();
	m_searchResult.clear();
	
	m_checkInfo.type = PokerType::Type_none;
	m_isFriend = false;

	m_boomMinValue = 0;
	m_boomMaxValue = 0;
	m_haveKingBoom = false;
	m_selfSurplusNum = 0;

	m_pair.first = 0;
	m_pair.second = 0;
}

std::vector<PokerInfo> simpleAI::getMoreBigPokers(const std::vector<PokerInfo> &goalVec, const CheckInfo &checkInfo, bool isFriend)
{
	std::vector<PokerInfo> vec;
	if (checkInfo.type == PokerType::Type_king_boom)
	{
		//最大的牌无须考虑
		return vec;
	}
	this->resetData();
	m_isFriend = isFriend;
	m_checkInfo = checkInfo;
	m_selfSurplusNum = goalVec.size();

	int kingNum = 0;
	for (auto &it : goalVec)
	{
		if (it.value == POKER_VALUE_JOKER_SMALL
			|| it.value == POKER_VALUE_JOKER_BIG)
		{
			++kingNum;
		}
		
		m_valueNumMap[it.value] += 1;
	}
	m_haveKingBoom = kingNum >= 2;
	if (m_haveKingBoom){
		//王炸排除王炸
		m_valueNumMap.erase(POKER_VALUE_JOKER_SMALL);
		m_valueNumMap.erase(POKER_VALUE_JOKER_BIG);
	}

	//-------------
	for (auto &it : m_valueNumMap)
	{
		if (it.second==4){
			if (m_boomMinValue==0){
				m_boomMinValue = it.first;
			}
			m_boomMaxValue = it.first;
		}
		m_typeNumMap[it.second] += 1;
	}
	//-------------------------------
	if (!m_haveKingBoom&&m_boomMaxValue == 0){
		if (m_selfSurplusNum<checkInfo.sum_num)
		{
			return vec;
		}
	}

	switch (checkInfo.type)
	{
	case Type_sigle:
		searchSingle();
		break;
	case Type_double:
		searchDouble();
		break;
	case Type_three:
		searchThree();
		break;
	case Type_tree_with_s:
		searchThreeWithS();
		break;
	case Type_tree_with_d:
		searchThreeWithD();
		break;
	case Type_straight:
		searchStraight();
		break;
	case Type_straight_d:
		searchStraightD();
		break;
	case Type_straight_three:
		searchStraightThree();
		break;
	case Type_straight_three_with_s:
		searchStraightThreeWithS();
		break;
	case Type_straight_three_with_d:
		searchStraightThreeWithD();
		break;
	case Type_four_two_s:
		searchFourTwoS();
		break;
	case Type_four_two_d:
		searchFourTwoD();
		break;
	case Type_boom:
		searchBoom();
		break;
	case Type_king_boom:
	case Type_last:
		break;
	default:
		break;
	}

	PokerInfo info;
	if (m_searchResult.empty()){
		//未找到合适的牌
		if (!isFriend){
			//非好友考虑使用炸弹/王炸
			if (checkInfo.type == PokerType::Type_boom){
				//如果有 使用王炸
				if (m_haveKingBoom){
					info.value = POKER_VALUE_JOKER_SMALL;
					vec.push_back(info);
					info.value = POKER_VALUE_JOKER_BIG;
					vec.push_back(info);
				}
			}
			else{
				if (m_boomMinValue)
				{
					for (int i = Color_Black_1; i <= Color_Red_2;++i){
						info.reset();
						info.color_type = i;
						info.value = m_boomMinValue;
						vec.push_back(info);
					}
				}
			}
		}
	}
	else{
		auto tmpVec = goalVec;
		int count = 0;
		for (auto aa:m_searchResult)
		{
			count = 0;
			for (auto it = tmpVec.begin(); it != tmpVec.end();)
			{
				if (aa.first==it->value)
				{
					vec.push_back(*it);
					++count;
					it = tmpVec.erase(it);
				}
				else{
					++it;
				}
				if (count==aa.second){
					break;
				}
			}
		}
		
	}
	return vec;
}

void simpleAI::searchSingle()
{
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second==1){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}
	if (m_pair.first == 0
		&&!m_isFriend){
		//未压住且非好友 拆牌（不拆炸弹）
		for (auto it : m_valueNumMap){
			if (it.second<4){
				if (it.first>m_checkInfo.min_value){
					m_pair.first = it.first;
					break;
				}
			}
		}
	}

	if (m_pair.first == 0){
		return;
	}

	m_pair.second = 1;
	bool need = false;
	if (!m_isFriend){
		need = true;
	}
	else{
		if (m_selfSurplusNum <= 2){
			need = true;
		}
		else{
			if (m_checkInfo.min_value < POKER_VALUE_2){
				need = true;
			}
		}
	}

	if (need){
		m_searchResult.push_back(m_pair);
	}
}

void simpleAI::searchDouble()
{
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 2){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}
	if (m_pair.first == 0
		&& !m_isFriend){
		//未压住且非好友 拆牌（不拆炸弹）
		for (auto it : m_valueNumMap){
			if (it.second==3){
				if (it.first>m_checkInfo.min_value){
					m_pair.first = it.first;
					break;
				}
			}
		}
	}

	if (m_pair.first == 0){
		return;
	}

	m_pair.second = 2;
	bool need = false;
	if (!m_isFriend){
		need = true;
	}
	else{
		if (m_selfSurplusNum <= 4){
			need = true;
		}
		else{
			if (m_checkInfo.min_value < POKER_VALUE_Q){
				need = true;
			}
		}
	}

	if (need){
		m_searchResult.push_back(m_pair);
	}
}

void simpleAI::searchThree()
{
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 3){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}
	//由于不拆炸弹 不需要拆牌 
	if (m_pair.first == 0){
		return;
	}

	m_pair.second = 3;
	bool need = false;
	if (!m_isFriend){
		need = true;
	}
	else{
		if (m_selfSurplusNum <= 5){
			need = true;
		}
	}

	if (need){
		m_searchResult.push_back(m_pair);
	}
}

void simpleAI::searchThreeWithS()
{
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 3){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}

	if (m_pair.first == 0){
		return;
	}

	if (m_isFriend){
		if (m_checkInfo.min_value >= POKER_VALUE_Q
			||m_typeNumMap[1]<1){
			return;
		}
	}

	m_pair.second = 3;
	m_searchResult.push_back(m_pair);

	m_pair.second = 1;
	if (m_typeNumMap[1] >= 1){
		for (auto it : m_valueNumMap){
			if (it.second == 1){
				if (m_searchResult.size()<2){
					m_pair.first = it.first;
					m_searchResult.push_back(m_pair);
				}
			}
		}
	}
	else{
		for (auto it : m_valueNumMap){
			if (it.second == 2){
				if (m_searchResult.size()<2){
					m_pair.first = it.first;
					m_searchResult.push_back(m_pair);
				}
			}
		}
	}
}

void simpleAI::searchThreeWithD()
{
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 3){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}

	if (m_pair.first == 0){
		return;
	}

	if (m_isFriend){
		if (m_checkInfo.min_value >= POKER_VALUE_Q){
			return;
		}
	}

	if (m_typeNumMap[2] < 1){
		return;
	}

	m_pair.second = 3;
	m_searchResult.push_back(m_pair);

	m_pair.second = 2;
	for (auto it : m_valueNumMap){
		if (it.second == 2){
			if (m_searchResult.size()<2){
				m_pair.first = it.first;
				m_searchResult.push_back(m_pair);
			}
		}
	}
}

void simpleAI::searchStraight()
{
	if (m_valueNumMap.size()<m_checkInfo.serial_num){
		//没有更大的顺子
		return;
	}
	int preValue = 0;
	int seaialNum = 1;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second<4){
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (it.first-preValue==1){
					++seaialNum;
					if (seaialNum == m_checkInfo.serial_num){
						preValue = it.first;
						break;
					}
				}
				else{
					seaialNum = 1;
				}
				preValue = it.first;
			}
		}
	}

	if (seaialNum == m_checkInfo.serial_num){
		m_pair.second = 1;
		for (int i = 0; i < seaialNum; ++i){
			m_pair.first = preValue--;
			m_searchResult.push_back(m_pair);
		}
	}
}

void simpleAI::searchStraightD()
{
	if (m_valueNumMap.size()<m_checkInfo.serial_num){
		//没有更大的顺子
		return;
	}
	int preValue = 0;
	int seaialNum = 1;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second>=2
			&&it.second<4){
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (it.first - preValue == 1){
					++seaialNum;
					if (seaialNum == m_checkInfo.serial_num){
						preValue = it.first;
						break;
					}
				}
				else{
					seaialNum = 1;
				}
				preValue = it.first;
			}
		}
	}

	if (seaialNum == m_checkInfo.serial_num){
		m_pair.second = 2;
		for (int i = 0; i < seaialNum; ++i){
			m_pair.first = preValue--;
			m_searchResult.push_back(m_pair);
		}
	}
}

void simpleAI::searchStraightThree()
{
	if (m_isFriend){
		//暂时不出
		return;
	}

	if (m_valueNumMap.size()<m_checkInfo.serial_num){
		//没有更大的顺子
		return;
	}
	int preValue = 0;
	int seaialNum = 1;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second ==3){
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (it.first - preValue == 1){
					++seaialNum;
					if (seaialNum == m_checkInfo.serial_num){
						preValue = it.first;
						break;
					}
				}
				else{
					seaialNum = 1;
				}
				preValue = it.first;
			}
		}
	}

	if (seaialNum == m_checkInfo.serial_num){
		m_pair.second = 3;
		for (int i = 0; i < seaialNum; ++i){
			m_pair.first = preValue--;
			m_searchResult.push_back(m_pair);
		}
	}
}

void simpleAI::searchStraightThreeWithS()
{
	if (m_isFriend){
		//暂时不出
		return;
	}

	int preValue = 0;
	int seaialNum = 1;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 3){
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (it.first - preValue == 1){
					++seaialNum;
					if (seaialNum == m_checkInfo.serial_num){
						preValue = it.first;
						break;
					}
				}
				else{
					seaialNum = 1;
				}
				preValue = it.first;
			}
		}
	}

	if (seaialNum == m_checkInfo.serial_num){
		if (m_typeNumMap[1] >= seaialNum)
		{
			m_pair.second = 3;
			for (int i = 0; i < seaialNum; ++i){
				m_pair.first = preValue--;
				m_searchResult.push_back(m_pair);
			}

			m_pair.second = 1;
			for (auto it : m_valueNumMap){
				if (it.second == 1){
					if (m_searchResult.size()<2 * seaialNum){
						m_pair.first = it.first;
						m_searchResult.push_back(m_pair);
					}
				}
			}
		}
	}
}

void simpleAI::searchStraightThreeWithD()
{
	if (m_isFriend){
		//暂时不出
		return;
	}

	int preValue = 0;
	int seaialNum = 1;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 3){
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (it.first - preValue == 1){
					++seaialNum;
					if (seaialNum == m_checkInfo.serial_num){
						preValue = it.first;
						break;
					}
				}
				else{
					seaialNum = 1;
				}
				preValue = it.first;
			}
		}
	}

	if (seaialNum == m_checkInfo.serial_num){
		if (m_typeNumMap[2] >= seaialNum)
		{
			m_pair.second = 3;
			for (int i = 0; i < seaialNum; ++i){
				m_pair.first = preValue--;
				m_searchResult.push_back(m_pair);
			}

			m_pair.second = 2;
			for (auto it : m_valueNumMap){
				if (it.second == 2){
					if (m_searchResult.size()<2 * seaialNum){
						m_pair.first = it.first;
						m_searchResult.push_back(m_pair);
					}
				}
			}
		}
	}
}

void simpleAI::searchFourTwoS()
{
	if (m_isFriend){
		//暂时不出
		return;
	}
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 4){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}

	if (m_pair.first == 0){
		return;
	}

	if (m_typeNumMap[2]<1
		|| m_typeNumMap[1]<2){
		//找不到两单（不拆大于两张的牌）
		return;
	}
	m_pair.second = 4;
	m_searchResult.push_back(m_pair);

	if (m_typeNumMap[1]>=2){
		m_pair.second = 1;
		for (auto it : m_valueNumMap){
			if (it.second == 1){
				if (m_searchResult.size()<3){
					m_pair.first = it.first;
					m_searchResult.push_back(m_pair);
				}
			}
		}
	}
	else{
		m_pair.second = 2;
		for (auto it : m_valueNumMap){
			if (it.second == 2){
				if (m_searchResult.size()<2){
					m_pair.first = it.first;
					m_searchResult.push_back(m_pair);
				}
			}
		}
	}
}

void simpleAI::searchFourTwoD()
{
	if (m_isFriend){
		//暂时不出
		return;
	}

	std::pair<int, int> m_pair;
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 4){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}

	if (m_pair.first==0){
		return;
	}

	if (m_typeNumMap[2]<2){
		//找不到两对（不拆大于两张的牌）
		return;
	}
	m_pair.second = 4;
	m_searchResult.push_back(m_pair);

	m_pair.second = 2;
	for (auto it : m_valueNumMap){
		if (it.second==2){
			if (m_searchResult.size()<3){
				//后续优化：判断选择的牌是否可组成连对
				m_pair.first = it.first;
				m_searchResult.push_back(m_pair);
			}
		}
	}
}

void simpleAI::searchBoom()
{
	if (m_isFriend){
		//暂时不出
		return;
	}
	for (auto it : m_valueNumMap){
		if (it.first>m_checkInfo.min_value
			&&it.second == 4){
			//查找可以压住的牌
			m_pair.first = it.first;
			break;
		}
	}

	if (m_pair.first == 0){
		return;
	}

	m_pair.second = 4;
	m_searchResult.push_back(m_pair);
}