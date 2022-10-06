#pragma once
#include "../sdk/misc/Recv.hpp"
#include "../sdk/sdk.hpp"
#include "../configurations.hpp"
#include "../sdk/utils/utils.hpp"
#include "../sdk/csgostructs.hpp"

class C_BaseCombatWeapon;
class attributable_item_t;

namespace Sequence
{
	namespace general
	{
		void FullUpdate();
	}
	int GetNewAnimation(const char* model, const int sequence);
	void DoSequenceRemapping(CRecvProxyData* data, C_BaseViewModel* entity);
}