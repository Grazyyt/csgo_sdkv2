#pragma once

#include <DirectXMath.h>

#include "../sdk/csgostructs.hpp"
#include "../sdk/utils/singleton.hpp"

#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

#define M_RADPI 57.295779513082f
#define PI 3.14159265358979323846f

class Misc : public Singleton<Misc>
{
public:
	void AutoStrafe(CUserCmd* cmd);
	void PrePrediction(CUserCmd* cmd, int pre_flags);
	void PostPrediction(CUserCmd* cmd, int pre_flags, int post_flags);
	void FakeBackwards(CUserCmd* cmd);
};