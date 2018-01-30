
#ifndef __publicData__
#define __publicData__

struct PokerInfo
{
	int value = 0;			//3-14：（3-A） 32:(2) 64:(小王) 128:（大王）
	int color_type = 0;		//1-4  黑桃 梅花 红桃 方片
	bool selected = false;	//是否选中
	
	void reset(){
		value = 0;
		color_type = 0;
		selected = false;
	}

	bool operator ==(const PokerInfo &info){
		if (color_type==info.color_type
			&&value==info.value)
		{
			return true;
		}
		return false;
	}
};

enum ColorType{
	Color_Black_1=1,
	Color_Black_2,
	Color_Red_1,
	Color_Red_2,
};

enum PokerType
{
	Type_none,                  //
	Type_sigle,				    //min 1   
	Type_double,				//min 2
	Type_three,					//min 3
	Type_tree_with_s,			//min 4
	Type_tree_with_d,			//min 5
	Type_straight,				//min 5
	Type_straight_d,			//min 6
	Type_straight_three,		//min 6
	Type_straight_three_with_s,	//8
	Type_straight_three_with_d,	//min 10
	Type_four_two_s,			//min 6
	Type_four_two_d,			//min 10
	Type_boom,					//min 4
	Type_king_boom,             //min 2

	Type_last,
};

struct CheckInfo
{
	PokerType type = PokerType::Type_none;
	int       min_value = 0;
	int       serial_num = 1;
	int       sum_num = 1;
};

enum RoleType
{
	Role_none=0,
	Role_pre,
	Role_self,
	Role_next,
	Role_bottom,//底牌
};
#endif

#define POKER_VALUE_Q             0x000c
#define POKER_VALUE_2             0x0020
#define POKER_VALUE_JOKER_SMALL   0x0040
#define POKER_VALUE_JOKER_BIG     0x0080
#define BUFFER_SIZE               0x0020

#define POKER_PILE_UP_VIEW_WIDTH  30

#define CAN_THINK_TIME_ADD_ONE    21
#define AI_THINK_TIME_THRESHOLD   18