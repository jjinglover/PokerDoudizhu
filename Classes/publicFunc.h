#ifndef __publicFunc__
#define __publicFunc__

#include "publicHeader.h"

class publicFunc
{
	static std::string getPokerFile(const PokerInfo &info);
public:
	static void updatePokerIcon(Sprite *sp, const PokerInfo &info);
	static void updatePokerIcon(ImageView *img, const PokerInfo &info);
	static void sortPoker(std::vector<PokerInfo> &vec);
	
	static CheckInfo checkPokersType(const std::vector<PokerInfo> &vec);

	static void popRedTip(const std::string &tip);
	static std::string intToString(const int &v);

	template <typename ST>
	static bool isHaved(const ST &var, std::vector<ST> &vec){
		for (auto &value : vec)
		{
			if (value == var){
				return true;
			}
		}
		return false;
	}
};

#endif