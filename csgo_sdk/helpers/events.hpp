#pragma once
#include <string>
#include <deque>
#include "../render/render.hpp"
#include "../sdk/sdk.hpp"
#include "../configurations.hpp"

class Events : public Singleton<Events>
{

	friend class Singleton<Events>;

public:
	void BEvent(IGameEvent* event);

	int bombsiteid;
	bool bomb_defusing_with_kits = false;

	bool IsKnifeString(const char* str)
	{
		return strstr(str, "knife");
	}
};

inline Events g_Events;