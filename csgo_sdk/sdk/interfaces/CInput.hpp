#pragma once

#include "../Misc/CUserCmd.hpp"

#define MULTIPLAYER_BACKUP 150

class bf_write;
class bf_read;

class CInput
{
public:
	char                pad_0x00[0x0C];				//0x0
	bool                m_trackir_available;		//0xC
	bool                m_mouse_initiated;			//0xD
	bool                m_mouse_active;				//0xE
	bool                m_fJoystickAdvancedInit;    //0xF   
	char                pad_0x08[0x28];             //0x10  
	void*               m_pKeys;					//0x38   
	char                pad_0x38[0x6C];             //0x3C 
	bool                m_fCameraInterceptingMouse; //0xA8
	bool                m_fCameraInThirdPerson;     //0xA9

	bool                m_fCameraMovingWithMouse;   //0xAA
	Vector				m_vecCameraOffset;          //0xAC  
	bool                m_fCameraDistanceMove;      //0xB8   
	int                 m_nCameraOldX;              //0xBC    
	int                 m_nCameraOldY;				//0xC0    
	int                 m_nCameraX;                 //0xC4
	int                 m_nCameraY;                 //0xC8  
	bool                m_CameraIsOrthographic;     //0xC9   

	Vector              m_angPreviousViewAngles;    //0xD0   
	Vector              m_angPreviousViewAnglesTilt;//0xD8 

	float               m_flLastForwardMove;		//0xE4          
	int                 m_nClearInputState;			//0xE8

	char                pad_0xE4[0x4];              //0xEC   
	CUserCmd*           m_pCommands;                //0xF0   
	CVerifiedUserCmd*   m_pVerifiedCommands;		//0xF4

	inline CUserCmd* GetUserCmd(int sequence_number);
	inline CUserCmd * GetUserCmd(int nSlot, int sequence_number);
	inline CVerifiedUserCmd* GetVerifiedCmd(int sequence_number);
};

CUserCmd* CInput::GetUserCmd(int sequence_number)
{
	using OriginalFn = CUserCmd * (__thiscall *)(void *, int, int);
	return CallVFunction<OriginalFn>(this, 8)(this, 0, sequence_number);
}

CUserCmd *CInput::GetUserCmd(int nSlot, int sequence_number)
{
	typedef CUserCmd*(__thiscall *GetUserCmd_t)(void*, int, int);
	return CallVFunction<GetUserCmd_t>(this, 8)(this, nSlot, sequence_number);
}

CVerifiedUserCmd* CInput::GetVerifiedCmd(int sequence_number)
{
	auto verifiedCommands = *(CVerifiedUserCmd **)(reinterpret_cast<uint32_t>(this) + 0xF4);
	return &verifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}
