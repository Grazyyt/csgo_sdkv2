#pragma once 

#include "IClientNetworkable.hpp"
#include "IClientRenderable.hpp"
#include "IClientUnknown.hpp"
#include "IClientThinkable.hpp"

struct SpatializationInfo_t;

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual void Release(void) = 0;
};

#pragma pack( push, 1 )
class CCSWeaponInfo
{
public:
	char pad_0000[4]; //0x0000
	char* consoleName; //0x0004
	char pad_0008[12]; //0x0008
	int32_t iMaxClip1; //0x0014
	char pad_0018[12]; //0x0018
	int32_t iMaxClip2; //0x0024
	char pad_0028[4]; //0x0028
	char* szWorldModel; //0x002C
	char* szViewModel; //0x0030
	char* szDroppedModel; //0x0034
	char pad_0038[64]; //0x0038
	char* szEmptySound; //0x0078
	char pad_007C[4]; //0x007C
	char* szBulletType; //0x0080
	char pad_0084[4]; //0x0084
	char* szHudName; //0x0088
	char* szWeaponName; //0x008C
	char pad_0090[12]; //0x0090
	int32_t iWeaponWeight; //0x009C
	char pad_00A0[40]; //0x00A0
	int32_t iWeaponType; //0x00C8
	int32_t iWeaponPad; //0x00CC
	int32_t iWeaponPrice; //0x00D0
	int32_t iKillAward; //0x00D4
	char* szAnimationPrefex; //0x00D8
	float flCycleTime; //0x00DC
	float flCycleTimeAlt; //0x00E0
	float flTimeToIdle; //0x00E4
	float flIdleInterval; //0x00E8
	bool bFullAuto; //0x00EC
	char pad_00ED[3]; //0x00ED
	int32_t iDamage; //0x00F0
	char pad_00F4[4]; //0x00F4
	float flArmorRatio; //0x00F8
	int32_t iBullets; //0x00FC
	float flPenetration; //0x0100
	float flFlinchVelocityModifierLarge; //0x0104
	float flFlinchVelocityModifierSmall; //0x0108
	float flWeaponRange; //0x010C
	float flRangeModifier; //0x0110
	float flThrowVelocity; //0x0114
	char pad_0118[12]; //0x0118
	bool bHasSilencer; //0x0124
	char pad_0125[7]; //0x0125
	int32_t iCrosshairMinDistance; //0x012C
	char pad_0130[4]; //0x0130
	float flMaxSpeed; //0x0134
	float flMaxSpeedAlt; //0x0138
	char pad_013C[4]; //0x013C
	float flSpread; //0x0140
	float flSpreadAlt; //0x0144
	float fInaccuracyCrouch; //0x0148
	float fInaccuracyCrouchAlt; //0x014C
	float fInaccuracyStand; //0x0150
	float fInaccuracyStandAlt; //0x0154
	float fInaccuracyJumpIntial; //0x0158
	float fInaccaurcyJumpApex; //0x015C
	float fInaccuracyJump; //0x0160
	float fInaccuracyJumpAlt; //0x0164
	float fInaccuracyLand; //0x0168
	float fInaccuracyLandAlt; //0x016C
	float fInaccuracyLadder; //0x0170
	float fInaccuracyLadderAlt; //0x0174
	float fInaccuracyFire; //0x0178
	float fInaccuracyFireAlt; //0x017C
	float fInaccuracyMove; //0x0180
	float fInaccuracyMoveAlt; //0x0184
	char pad_0188[4]; //0x0188
	int32_t iRecoilSeed; //0x018C
	float flRecoilAngle; //0x0190
	float flRecoilAngleAlt; //0x0194
	float flRecoilVariance; //0x0198
	float flRecoilAngleVarianceAlt; //0x019C
	float flRecoilMagnitude; //0x01A0
	float flRecoilMagnitudeAlt; //0x01A4
	float flRecoilMagnatiudeVeriance; //0x01A8
	float flRecoilMagnatiudeVerianceAlt; //0x01AC
	char pad_01B0[4]; //0x01B0
	float flRecoveryTimeCrouch; //0x01B4
	float flRecoveryTimeStand; //0x01B8
	float flRecoveryTimeCrouchFinal; //0x01BC
	float flRecoveryTimeStandFinal; //0x01C0
	int32_t iRecoveryTransititionStartBullet; //0x01C4
	int32_t iRecoveryTransititionEndBullet; //0x01C8
	bool bUnzoomAfterShot; //0x01CC
	char pad_01CD[27]; //0x01CD
	char* szWeaponClass; //0x01E8
	char pad_01EC[56]; //0x01EC
	float flInaccuracyPitchShift; //0x0224
	float flInaccuracySoundThreshold; //0x0228
	float flBotAudibleRange; //0x022C
	char pad_0230[12]; //0x0230
	bool bHasBurstMode; //0x023C
	char pad_023D[8942]; //0x023D
}; //Size: 0x252B
#pragma pack( pop )

class IWeaponSystem
{
	virtual void unused0() = 0;
	virtual void unused1() = 0;
public:
	virtual CCSWeaponInfo* GetWpnData(unsigned ItemDefinitionIndex) = 0;
};

template<class T> struct CUtlReference {
	CUtlReference* m_pNext;
	CUtlReference* m_pPrev;
	T* m_pObject;
};
template<class T> struct CUtlIntrusiveList {
	T* m_pHead;
};
template<class T> struct CUtlIntrusiveDList : public CUtlIntrusiveList<T> {};
template<class T> struct CUtlReferenceList : public CUtlIntrusiveDList< CUtlReference<T> > {};

enum EAttributeDataType {
	ATTRDATATYPE_NONE = -1,
	ATTRDATATYPE_FLOAT = 0,
	ATTRDATATYPE_4V,
	ATTRDATATYPE_INT,
	ATTRDATATYPE_POINTER,

	ATTRDATATYPE_COUNT,
};

#define MAX_PARTICLE_ATTRIBUTES 24

#define DEFPARTICLE_ATTRIBUTE( name, bit, datatype )			\
	const int PARTICLE_ATTRIBUTE_##name##_MASK = (1 << bit);	\
	const int PARTICLE_ATTRIBUTE_##name = bit;					\
	const EAttributeDataType PARTICLE_ATTRIBUTE_##name##_DATATYPE = datatype;

DEFPARTICLE_ATTRIBUTE(TINT_RGB, 6, ATTRDATATYPE_4V);

DEFPARTICLE_ATTRIBUTE(ALPHA, 7, ATTRDATATYPE_FLOAT);

struct CParticleAttributeAddressTable {
	float* m_pAttributes[MAX_PARTICLE_ATTRIBUTES];
	size_t m_nFloatStrides[MAX_PARTICLE_ATTRIBUTES];

	FORCEINLINE float* FloatAttributePtr(int nAttribute, int nParticleNumber) const {
		int block_ofs = nParticleNumber / 4;
		return m_pAttributes[nAttribute] +
			m_nFloatStrides[nAttribute] * block_ofs +
			(nParticleNumber & 3);
	}

};

struct CUtlString_simple {
	char* buffer;
	int capacity;
	int grow_size;
	int length;
};

class CParticleSystemDefinition {
	BYTE pad_0[308];
public:
	CUtlString_simple m_Name;
};

class CParticleCollection {
	BYTE pad_0[48];//0
public:
	int m_nActiveParticles;//48
private:
	BYTE pad_1[12];//52
public:
	CUtlReference<CParticleSystemDefinition> m_pDef;//64
private:
	BYTE pad_2[60];//80
public:
	CParticleCollection* m_pParent;//136
private:
	BYTE pad_3[84];//140
public:
	CParticleAttributeAddressTable m_ParticleAttributes;//224
};