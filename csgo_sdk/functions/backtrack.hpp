#pragma once

#include "../configurations.hpp"
#include "../sdk/csgostructs.hpp"

#include <map>
#include <deque>

struct BacktrackData 
{
	float simTime;
	Vector hitboxPos;
	matrix3x4_t boneMatrix[128];
};

class Backtrack : public Singleton<Backtrack>
{
public:
	void OnMove(CUserCmd* pCmd);

	float correct_time = 0.0f;
	float latency = 0.0f;
	float lerp_time = 0.0f;

	std::map<int, std::deque<BacktrackData>> data;
};