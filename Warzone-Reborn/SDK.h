#pragma once
#include "Main.hpp"
#include "Vec.h"

#define _PTR_MAX_VALUE	((PVOID)0x000F000000000000)
#define BYTEn(x, n)		(*((BYTE*)&(x)+n))
#define BYTE1(x)		BYTEn(x,  1)

//auto padding
#define	STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

template <typename T>
bool IsValidPtr(PVOID ptr)
{
	return (ptr >= (PVOID)0x10000) && (ptr < _PTR_MAX_VALUE);
}

//Gamemode
#define OFFSET_GAMEMODE 0x13D74AA8 //74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?

//CharacterInfo
#define OFFSET_CHARACTERINFO_ENCRYPTED_PTR 0x13FE1E68
#define OFFSET_CHARACTERINFO_REVERSED_ADDR 0x4D960DD
#define OFFSET_CHARACTERINFO_DISPLACEMENT 0x11

#define OFFSET_CHARACTERINFO_BASE_ENCRYPTED_PTR 0x97BC8 //48 8B 83 ? ? ? ? C6 44 24 ? ? 0F B6 4C 24 ?
#define OFFSET_CHARACTERINFO_BASE_REVERSED_ADDR 0x4D96103
#define OFFSET_CHARACTERINFO_BASE_DISPLACEMENT 0x19

//E8 ?? ?? ?? ?? 48 83 C7 04 48 83 ED 01 75 E6 48 8B D3 48 8B CE
#define OFFSET_CHARACTERINFO_INDEX 0x151C
#define OFFSET_CHARACTERINFO_POSITION_PTR 0x78
#define OFFSET_CHARACTERINFO_VALID 0x540

#define OFFSET_CHARACTERINFO_DEAD 0x664 //C7 83 ? ? ? ? ? ? ? ? E8 ? ? ? ? ? 0F B6 ? 48 8B
#define OFFSET_CHARACTERINFO_STANCE 0xD4 //83 BF ? ? ? ? ? 75 0A F3

//39 81 ? ? ? ? 0F 84 ? ? ? ? B8 ? ? ? ?
#define OFFSET_CHARACTERINFO_SIZE 0x3A98
#define OFFSET_CHARACTERINFO_TEAM 0x63C

#define OFFSET_CHARACTERINFO_LOCAL_BASE 0x219F0 //49 83 BF ? ? ? 00 00 0F 84 22 or 48 8D 15 47 17 E5 01 or post_game_state
#define OFFSET_CHARACTERINFO_LOCAL_INDEX 0x1F4 //8D 90 ? ? ? ? 8D 88 ? ? ? ? 48 8B 43 10 39 90 ? ? ? ?

#define OFFSET_CHARACTERINFO_RECOIL 0x0F68 //0F 85 ? ? ? ? 0F 2E 80 ? ? ? ? 0F 85 ? ? ? ? 4C 8D 96 ? ? ? ?

//Client
#define OFFSET_CLIENT_ARRAY 0x13FEF588 //NAME_ARRAY_OFFSET
#define OFFSET_CLIENT_HANDLE 0x4C70
#define OFFSET_CLIENT_HEALTH 0x8C

//Bones
#define OFFSET_BONES_ENCRYPTED_PTR 0x1223C548
#define OFFSET_BONES_REVERSED_ADDR 0x4D96196
#define OFFSET_BONES_DISPLACEMENT 0xB

#define OFFSET_BONES_INDEX_ARRAY 0x4DFB360 //INDEX_ARRAY_OFFSET //TO UPDATE
#define OFFSET_BONES_BASE_POS 0x1C61C //TO UPDATE
#define OFFSET_BONES_SIZE 0x150

//Viewport
#define OFFSET_VIEWPORT_REFDEF 0x13FE3BC0 //F3 0F 11 47 ? E8 ? ? ? ? 48 8D 0D ? ? ? ? 33 0D ? ? ? ?
#define OFFSET_VIEWPORT_CAMERA 0x113F7980 // 48 89 05 ? ? ? ? E8 ? ? ? ? 8B D6 49

#define OFFSET_VIEWPORT_CAMERA_VIEW_X 0xF8
#define OFFSET_VIEWPORT_CAMERA_VIEW_Y 0x108
#define OFFSET_VIEWPORT_CAMERA_VIEW_Z 0x118

//Visible
#define OFFSET_VISIBLE_FUNCTION_DISTRIBUTE 0x7D001A0 //66 3B C1 73 EF 0F B7 C8 4C 8D 05 ? ? ? ?
#define OFFSET_VISIBLE_ABOUT_FUNCTION 0x3E6F9F0 //48 8B CB 66 0F 6E C0 48 8D 05 ? ? ? ? 0F 5B C0
#define OFFSET_VISIBLE_OFFSET 0xA70 //80 BF ? ? 00 00 01 74 17 3B 87

//DirectX
#define OFFSET_DIRECTX_COMMAND_QUEUE 0x159A9BA8 //48 8B 0D ? ? ? ? 4C 8B C3 48 8B 15 ? ? ? ? 48 8B 01 FF 50 78

/*
	//sub     eax, dword ptr cs:qword_7FF6EE42EAC8+4
	#define game_mode 0x13E1FA28 //74 1D 48 8B 8B ? ? ? ? 48 2B 8E ? ? ? ? 2B 05 ? ? ? ?
	
	//movsx   esi, rva word_7FF6EC9B1240[r15+rdi*2]
	#define INDEX_ARRAY_OFFSET 0xF4D1240 //8B D7 8B CB E8 ? ? ? ? 41 0F BF B4 7F ? ? ? ? 48 8B E8
	
	//lea     r12, qword_7FF6EE6ADFD8
	#define NAME_ARRAY_OFFSET 0x111CDFD8 //4C 8D 25 ? ? ? ? 4C 8B FA 4D 8B 24 C4 66 41 83 F8 ?
	#define NAME_LIST_OFFSET 0x4C70
	
	//lea     rcx, dword_7FF67D4A2610
	#define REFDEF 0x112C2610 //F3 0F 11 47 ? E8 ? ? ? ? 48 8D 0D ? ? ? ? 33 0D ? ? ? ?

	//mov     rbx, cs:qword_7FF67A9F6830
	#define CAMER_OBJ 0xE816830 //48 8B 1D ? ? ? ? 48 8B F1 48 63 49 30 4C 8B FA

	//mov     rcx, [r13+1D8h]
	#define CAMER_POS 0x1D8 //49 8B 8D ? ? ? ? 48 89 44 24 ? 49 89 85 ? ? ? ?

	//mov     [rbx+50h], eax
	//mov     eax, [rdi+1147Ch]
	#define bone_basepos 0x1147C //89 73 4C E8 ? ? ? ? 48 8B 74 24 ? 89 43 50 8B 87 ? ? ? ?

	//#define client_BASE_OFFSET 0x97B18 //48 8B 83 ? ? ? ? C6 44 24 ? ? 0F B6 4C 24 ?
	#define client_LOCAL_INDEX_OFFSET 0x2D168 //48 83 BB ? ? ? ? ? 0F 84 ? ? ? ? 48 89 AC 24 ? ? ? ?
	#define client_local_index_data_offset 0x1F4 //8D 90 ? ? ? ? 8D 88 ? ? ? ? 48 8B 43 10 39 90 ? ? ? ?
	#define client_NORECOIL_OFFSET 0x8E24 //4C 8D 96 ? ? ? ? B3 01 41 8B 52 0C 4D 8D 4A 04 4D 8D 42 08 4C 89 95 ? ? ? ?
	// mov     [rbx+14Ch], r14d
	#define client_entitynum 0x604 //E8 ? ? ? ? 48 83 C7 04 48 83 ED 01 75 E6 48 8B D3 48 8B CE
	#define client_PosInfo 0x15D0 //
	#define client_isValid 0x4C0 //
	#define client_TeamId 0x1CC //39 81 ? ? ? ? 0F 84 ? ? ? ? B8 ? ? ? ?
	#define CharacterInfo_size 0x3A78//

	//cmp     dword ptr [rdi+690h], 1
	#define client_stance 0x3164 //0F 48 F0 83 BF ? ? ? ? ? 75 0A F3 0F 10 35 ? ? ? ? EB 08

	//C7 83 ? ? ? ? ? ? ? ? E8 ? ? ? ? 44 0F B6 C6 48 8B D5 48 8B CF E8 ? ? ? ?
	#define client_death_status 0x1D4 //>0 dead
	#define client_death_status 0x1630 //>0 dead

	//mov     rcx, cs:qword_7FF67EE5EAA8
	#define COMMANDQUEUE 0x12B7EAA8 //48 8B 0D ? ? ? ? 4C 8B C3 48 8B 15 ? ? ? ? 48 8B 01 FF 50 78
	#define WEAPONDEFSCOMPLETE_OFFSET 0xe97fef0; //

	movzx   ecx, ax
	lea     r8, FunctionDisTribute
	#define FunctionDisTribute 0x7FF73B9B0DA0 //66 3B C1 73 EF 0F B7 C8 4C 8D 05 ? ? ? ?

	mov     [rdi+88h], rax
	lea     rax, AboutVisibleFunction
	#define AboutVisibleFunction 0x7FF737B15DE0 //48 8B CB 66 0F 6E C0 48 8D 05 ? ? ? ? 0F 5B C0
*/

namespace process
{
	BOOL	CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam);
	HWND	GetProcessWindow();
}

namespace g_Data
{
	extern	HWND hWind;
	extern	uint64_t base;
	extern	DWORD peb_index;
	
	void	Init();
}

namespace g_SDK
{
	enum BONE_INDEX : unsigned long
	{
		BONE_POS_HELMET			= 8,

		BONE_POS_HEAD			= 7,
		BONE_POS_NECK			= 6,
		BONE_POS_CHEST			= 5,
		BONE_POS_MID			= 4,
		BONE_POS_TUMMY			= 3,
		BONE_POS_PELVIS			= 2,

		BONE_POS_RIGHT_FOOT_1	= 21,
		BONE_POS_RIGHT_FOOT_2	= 22,
		BONE_POS_RIGHT_FOOT_3	= 23,
		BONE_POS_RIGHT_FOOT_4	= 24,

		BONE_POS_LEFT_FOOT_1	= 17,
		BONE_POS_LEFT_FOOT_2	= 18,
		BONE_POS_LEFT_FOOT_3	= 19,
		BONE_POS_LEFT_FOOT_4	= 20,

		BONE_POS_LEFT_HAND_1	= 13,
		BONE_POS_LEFT_HAND_2	= 14,
		BONE_POS_LEFT_HAND_3	= 15,
		BONE_POS_LEFT_HAND_4	= 16,

		BONE_POS_RIGHT_HAND_1	= 9,
		BONE_POS_RIGHT_HAND_2	= 10,
		BONE_POS_RIGHT_HAND_3	= 11,
		BONE_POS_RIGHT_HAND_4	= 12
	};

	enum TYPE_TAG
	{
		ET_GENERAL			= 0x0,
		ET_PLAYER			= 0x1,
		ET_PLAYER_CORPSE	= 0x2,
		ET_ITEM				= 0x3,
		ET_MISSILE			= 0x4,
		ET_INVISIBLE		= 0x5,
		ET_SCRIPTMOVER		= 0x6,
		ET_SOUND_BLEND		= 0x7,
		ET_FX				= 0x8,
		ET_LOOP_FX			= 0x9,
		ET_PRIMARY_LIGHT	= 0xA,
		ET_TURRET			= 0xB,
		ET_HELICOPTER		= 0xC,
		ET_PLANE			= 0xD,
		ET_VEHICLE			= 0xE,
		ET_VEHICLE_COLLMAP	= 0xF,
		ET_VEHICLE_CORPSE	= 0x10,
		ET_VEHICLE_SPAWNER	= 0x11,
		ET_AGENT			= 0x12,
		ET_AGENT_CORPSE		= 0x13,
		ET_EVENTS			= 0x14,
	};

	enum STANCE : int
	{
		IDLE	= 0,
		CROUCH	= 1,
		PRONE	= 2,
		KNOCK	= 3
	};

	class WeaponDef
	{
		char	pad0000[0x404];
		int		iProjectileSpeed;
	};

	class WeaponCompleteDef
	{
	public:
		char*		szInternalName;		//0x0000
		WeaponDef*  weapDef;			//0x0008
		char*		szDisplayName;		//0x0010
		char*		hideTags;			//0x0018
		char		pad_0020[552];		//0x0020
		char*		szAltWeaponName;	//0x0248
		char		pad_0250[32];	    //0x0250
	}; //Size: 0x0270

	class clientInfo_t
	{
	public:
		DWORD	m_entity_index;
		char	m_szName[36]; // 0x4    
		
		int32_t GetHealth()
		{
			if (!IsValidPtr<clientInfo_t>(this))
				return 0;

			return *reinterpret_cast<int32_t*>((uintptr_t)this + OFFSET_CLIENT_HEALTH);
		}

	}; // size == 0x90

	class characterInfo_t
	{
	public:
		int32_t GetIndex()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<int32_t*>((uintptr_t)this + OFFSET_CHARACTERINFO_INDEX);
		}

		int IsValid()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<int*>((uintptr_t)this + OFFSET_CHARACTERINFO_VALID);
		}

		int32_t GetTeamId()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<int32_t*>((uintptr_t)this + OFFSET_CHARACTERINFO_TEAM);
		}

		QWORD GetPositionPtr()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<QWORD*>((uintptr_t)this + OFFSET_CHARACTERINFO_POSITION_PTR);
		}

		int32_t GetStance()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<int32_t*>((uintptr_t)this + OFFSET_CHARACTERINFO_STANCE);
		}

		int32_t GetDeathStatus()
		{
			if (!IsValidPtr<characterInfo_t>(this))
				return 0;

			return *reinterpret_cast<int32_t*>((uintptr_t)this + OFFSET_CHARACTERINFO_DEAD);
		}
	}; //size == 0x3A48

	class posInfo_t
	{
	public:
		float GetRotation() const;
	public:
		char	pad_0x0[0xC];
		DWORD	nIsSoldierEntity;		//this+0xC
		char	pad_0x10[0x8];
		void*	m_next_member;			//this+0x18
		char	pad_0x20[0x18];
		char	flags[8];               //this+0x38
		Vector3 m_position;				//this+0x40
		char	pad_0x4C[0xC];
		float	m_rotation;				//this+0x58
	};

	class refdef_t
	{
	public:
		char	pad_0000[8];				//0x0000
		__int32 Width;						//0x0008
		__int32 Height;						//0x000C
		float	FovX;						//0x0010
		float	FovY;						//0x0014
		float	Unk;						//0x0018
		char	pad_001C[8];				//0x001C
		Vector3 ViewAxis[3];				//0x0024
	};

	class Result
	{
	public:
		bool	hasResult;
		float	a;
		float	b;
	};

	struct visible_t
	{
		UCHAR	 data[0x10];
	};

	struct velocityInfo_t
	{
		Vector3		lastPos;
		Vector3		delta;
	};

	bool				IsInGame();
	int					GetGameMode();
	int					GetMaxPlayerCount();
	characterInfo_t*	GetCharacterInfo(QWORD decryptedPtr, int i);
	characterInfo_t*	GetLocalCharacterInfo(int* local_index);
	clientInfo_t*		GetClientInfo(int i);
	refdef_t*			GetRefdef();
	bool				UpdateVisibleAddr(int i);
	bool				IsVisible(int i);
	std::string			GetPlayerName(int i);
	void				UpdateLastVisible();
	bool				GetBonesByIndex(int i, int bone_index, Vector3* Out_bone_pos);
	void				NoRecoil();
	int					GetPlayerHealth(int i);
	void				StartTick();
	void				UpdateVelMap(int index, Vector3 vPos);
	void				ClearMap();
	void				ClearCachedPtr();
	Vector3				GetSpeed(int index);
	Vector3				PredictionSolver(Vector3 local_pos, Vector3 position, int index, float bullet_speed);
	Vector3				GetPrediction(int local_index, int i, float fdistance, Vector3 position, float bullet_speed);
	int					GetClientCount();
}