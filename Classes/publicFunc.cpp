 #include "publicFunc.h"

std::string publicFunc::getPokerFile(const PokerInfo &info)
{
	char buff[BUFFER_SIZE] = { 0 };
	std::string file = "PokerGame/poker/";
	if (info.value == POKER_VALUE_JOKER_SMALL
		|| info.value == POKER_VALUE_JOKER_BIG)
	{
		sprintf(buff, "poker_%d", info.value);
		file += buff;
	}
	else{
		sprintf(buff, "poker_%d_%d", info.color_type, info.value);
		file += buff;
	}
	file += ".png";
	return file;
}

void publicFunc::updatePokerIcon(Sprite *sp, const PokerInfo &info)
{
	if (sp){
		auto file = publicFunc::getPokerFile(info);
		auto cache = SpriteFrameCache::getInstance()->getSpriteFrameByName(file);
		if (cache)
		{
			sp->initWithSpriteFrame(cache);
		}
	}
}

void publicFunc::updatePokerIcon(ImageView *img, const PokerInfo &info)
{
	if (img){
		auto file = publicFunc::getPokerFile(info);
		img->loadTexture(file, Widget::TextureResType::PLIST);
	}
}

void publicFunc::sortPoker(std::vector<PokerInfo> &vec)
{
	auto func = [](PokerInfo &a, PokerInfo &b){
		if (a.value>b.value){
			return true;
		}
		else if (a.value==b.value)
		{
			if (a.color_type>b.color_type)
			{
				return true;
			}
		}
		return false;
	};
	std::sort(vec.begin(),vec.end(), func);
}

int fabs(const int value)
{
	int var = value;
	if (value<0)
	{
		var *= -1;
	}
	return var;
}

void handlePokers(const std::map<int, int> &valueNumMap, bool &allSerial, int &v3SerialNum, int &v3MinValue, int &v34SerialNum, int &v34MinValue)
{
	int preValue = 0;				              //辅助计算所有面值是否连续
	int v3Group = 1;                              //计算第几组连续
	std::map<int, int>  v3SerialNumMap;           //面值为3连续的个数（用来判断飞机牌型）
	std::map<int, int>  v3PreValueMap;		      //辅助计算面值为3连续的个数
	int v34Group = 1;                             //计算第几组连续
	std::map<int, int>  v34SerialNumMap;          //面值为3、4 连续的个数（用来判断飞机牌型）
	std::map<int, int>  v34PreValueMap;			  //辅助计算面值为3、4 连续的个数

	for (auto &it : valueNumMap)
	{
		//判断是否连续
		if (allSerial)
		{
			if (preValue == 0){
				preValue = it.first;
			}
			else{
				if (fabs(it.first - preValue)>1)
				{
					allSerial = false;
				}
				preValue = it.first;
			}
		}
		//面值数量为3连续的个数
		if (it.second == 3)
		{
			//判断是否连续
			if (v3PreValueMap[v3Group] == 0){
				v3PreValueMap[v3Group] = it.first;
				++v3SerialNumMap[v3Group];
			}
			else{
				if (fabs(it.first - v3PreValueMap[v3Group]) == 1)
				{
					++v3SerialNumMap[v3Group];
				}else{
					++v3Group;
				}
				v3PreValueMap[v3Group] = it.first;
			}
		}

		//面值数量为3、4 连续的个数
		if (it.second >= 3)
		{
			//判断是否连续
			if (v34PreValueMap[v34Group] == 0){
				v34PreValueMap[v34Group] = it.first;
				++v34SerialNumMap[v34Group];
			}
			else{
				if (fabs(it.first - v34PreValueMap[v34Group]) == 1)
				{
					++v34SerialNumMap[v34Group];
				}
				else{
					++v34Group;
				}
				v34PreValueMap[v34Group] = it.first;
			}
		}
	}
	//----
	int v3sNum = 0,v3Index;
	for (auto it:v3SerialNumMap)
	{
		if (v3sNum == 0 || it.second>v3sNum)
		{
			v3sNum = it.second;
			v3Index = it.first;
		}
	}
	v3SerialNum = v3sNum;
	v3MinValue = v3PreValueMap[v3Index] - v3sNum+1;
	//----
	int v34sNum = 0, v34Index;
	for (auto it : v34SerialNumMap)
	{
		if (v34sNum == 0 || it.second>v34sNum)
		{
			v34sNum = it.second;
			v34Index = it.first;
		}
	}
	v34SerialNum = v34sNum;
	v34MinValue = v34PreValueMap[v34Index] - v34sNum+1;
}

CheckInfo publicFunc::checkPokersType(const std::vector<PokerInfo> &vec)
{
	std::map<int, int> valueNumMap;//记录每种面值有几个
	for (auto &it:vec)
	{
		valueNumMap[it.value] += 1;
	}
	//-----------------------------
	bool allSerial = true;			//所有面值是否连续
	int v3SerialNum = 0;
	int	v3MinValue = 0;
	int v34SerialNum = 0;
	int v34MinValue = 0;
	handlePokers(valueNumMap, allSerial, v3SerialNum, v3MinValue, v34SerialNum, v34MinValue);

	int maxNum = 0;					//面值相同的最大数量
	bool isEverySameCount = true;	//每种面值数量是否一样
	std::map<int, int> typeNumMap;  //记录（单、双、三、四）的数量
	for (auto &it : valueNumMap)
	{
		//判断面值最大数量是否一致
		if (isEverySameCount)
		{
			if (maxNum != 0 && maxNum != it.second){
				isEverySameCount = false;
			}
		}
		//计算面值相同最大的数量
		if (maxNum<it.second)
		{
			maxNum = it.second;
		}
		//记录类型的数量
		typeNumMap[it.second] += 1;
	}

	//-----------------------------
	int min_value = 0;
	auto it = valueNumMap.begin();
	if (it != valueNumMap.end())
	{
		min_value = it->first;
	}
	//-----------------------------
	CheckInfo info;
	int pokerNum = vec.size();
	if (maxNum==1)
	{
		//单、顺子、王炸
		if (pokerNum==1)
		{
			info.type = PokerType::Type_sigle;
			info.min_value = min_value;
		}
		else if (pokerNum==2)
		{
			if (vec.at(0).value + vec.at(1).value == POKER_VALUE_JOKER_SMALL + POKER_VALUE_JOKER_BIG)
			{
				info.type=PokerType::Type_king_boom;
			}
		}
		else if (pokerNum>=5)
		{
			if (allSerial)
			{
				info.type = PokerType::Type_straight;
				info.min_value = min_value;
				info.serial_num = pokerNum;
			}
		}
	}
	else if (maxNum==2)
	{
		//双、连对
		if (pokerNum==2)
		{
			info.type = PokerType::Type_double;
			info.min_value = min_value;
		}
		else if (pokerNum>=6)
		{
			if (isEverySameCount&&allSerial)
			{
				info.type = PokerType::Type_straight_d;
				info.min_value = min_value;
				info.serial_num = pokerNum / 2;
			}
		}
	}
	else if (maxNum==3)
	{
		//三张、三带一、三带二、飞机、飞机单翅膀、飞机双翅膀
		if (allSerial&&isEverySameCount)
		{
			if (pokerNum == 3)
			{
				info.type = PokerType::Type_three;
				info.min_value = min_value;
			}
			else{
				info.type = PokerType::Type_straight_three;
				info.min_value = min_value;
				info.serial_num = pokerNum / 3;
			}
		}
		else if (pokerNum==4)
		{
			info.type = PokerType::Type_tree_with_s;
			info.min_value = v3MinValue;
		}
		else if (pokerNum==5)
		{
			if (typeNumMap[2]==1)
			{
				info.type = PokerType::Type_tree_with_d;
				info.min_value = v3MinValue;
			}
		}
		else
		{
			if (v34SerialNum >= 2)
			{
				if (pokerNum - v34SerialNum * 3 == v34SerialNum)
				{
					info.type = PokerType::Type_straight_three_with_s;
					info.min_value = v34MinValue;
					info.serial_num = v34SerialNum;
				}
				else if (v3SerialNum>=2)
				{
					if (pokerNum - v3SerialNum * 3 == typeNumMap[2] * 2 + typeNumMap[4] * 4){
						info.type = PokerType::Type_straight_three_with_d;
						info.min_value = v3MinValue;
						info.serial_num = v3SerialNum;
					}
				}
			}
		}
	}else
	{
		//炸弹、炸弹双单、炸弹两对
		if (pokerNum==4)
		{
			info.type = PokerType::Type_boom;
			info.min_value = min_value;
		}
		else if (pokerNum==6)
		{
			info.type = PokerType::Type_four_two_s;
			info.min_value = v34MinValue;
		}
		else if (pokerNum>=8)
		{
			bool is = false;
			if (typeNumMap[2]==2
				||typeNumMap[4]==2)
			{
				info.type = PokerType::Type_four_two_d;
				info.min_value = v34MinValue;
				is = true;
			}

			if (!is&&v34SerialNum >= 2)
			{
				if (pokerNum - v34SerialNum * 3 == v34SerialNum)
				{
					info.type = PokerType::Type_straight_three_with_s;
					info.min_value = v34MinValue;
					info.serial_num = v34SerialNum;
				}

				if (v3SerialNum >= 2)
				{
					if (pokerNum - v3SerialNum * 3 == typeNumMap[2] * 2 + typeNumMap[4] * 4){
						info.type = PokerType::Type_straight_three_with_d;
						info.min_value = v34MinValue;
						info.serial_num = v3SerialNum;
					}
				}
			}
		}
	}
	info.sum_num = pokerNum;
	return info;
}

void publicFunc::popRedTip(const std::string &tip)
{
	auto sz = Director::getInstance()->getWinSize();

	auto tipT = ui::Text::create();
	tipT->setString(tip);
	tipT->setTextColor(Color4B::RED);
	tipT->setPosition(sz*0.5);
	tipT->runAction(Sequence::create(MoveBy::create(1.0, Vec2(0, 150)), CallFunc::create(std::bind(&Node::removeFromParent, tipT)), nullptr));
	Director::getInstance()->getRunningScene()->addChild(tipT, 0xff);
}

std::string publicFunc::intToString(const int &v)
{
	std::stringstream ss;
	ss << v;
	return ss.str();
}