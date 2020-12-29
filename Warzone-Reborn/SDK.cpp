#include "SDK.h"
#include "Xor.hpp"
#include "LazyImporter.h"
#include "Memory.h"
#include "GUI.h"

namespace process
{
	HWND prehWnd;

	int _stdcall EnumWindowCallBack(HWND hWnd, LPARAM lParam)
	{
		DWORD dwPid = 0;
		GetWindowThreadProcessId(hWnd, &dwPid);

		if (dwPid == lParam)
		{
			prehWnd = hWnd;

			return 0;
		}

		return 1;
	}

	HWND GetProcessWindow()
	{
		if (prehWnd)
			return prehWnd;

		EnumWindows(EnumWindowCallBack, GetCurrentProcessId());

		if (prehWnd == NULL)
			Exit();

		return prehWnd;
	}
}

namespace g_Data
{
	QWORD base;
	DWORD peb_index;
	HWND hWind;

	void Init()
	{
		base = (QWORD)(iat(GetModuleHandleA).get()("ModernWarfare.exe"));
		hWind = process::GetProcessWindow();
		peb_index = (DWORD)(__readgsqword(0x60) >> 0xC) & 0xF;
	}
}

namespace g_SDK
{
	const DWORD nTickTime = 64;//64 ms
	bool bUpdateTick = false;
	std::map<DWORD, velocityInfo_t> velocityMap;

	QWORD cached_character_ptr;
	QWORD cached_base_ptr;
	QWORD cached_bone_ptr;
	QWORD current_visible_offset;
	QWORD last_visible_offset;
	QWORD cached_visible_base;
	
	bool IsInGame()
	{
		return *(int*)(g_Data::base + OFFSET_GAMEMODE + 0x4) > 0;
	}

	int GetGameMode()
	{
		return *(int*)(g_Data::base + OFFSET_GAMEMODE + 0x4);
	}

	int GetMaxPlayerCount()
	{
		return *(int*)(g_Data::base + OFFSET_GAMEMODE + 0x0);
	}

	QWORD DecryptCharacterInfoPtr()
	{
		if (cached_character_ptr != 0)
			return cached_character_ptr;

		QWORD encryptedPtr = *(QWORD*)(g_Data::base + OFFSET_CHARACTERINFO_ENCRYPTED_PTR);
		QWORD reversed_addr = *(QWORD*)(g_Data::base + OFFSET_CHARACTERINFO_REVERSED_ADDR);
		QWORD last_key = *(QWORD*)(~(reversed_addr) + OFFSET_CHARACTERINFO_DISPLACEMENT);
		
		auto not_peb = __readgsqword(0x60);
		if (encryptedPtr && last_key)
		{
			auto F_0000000003CCBADB = encryptedPtr >> 0x14;
			auto F_0000000003CCBADF = (encryptedPtr) ^ F_0000000003CCBADB;
			auto F_0000000003CCBAF6 = ((F_0000000003CCBADF >> 0x28) ^ F_0000000003CCBADF) * 0x70B1F0B1F564CDBD;
			auto F_0000000003CCBAFD = F_0000000003CCBAF6 >> 0x15;
			auto F_0000000003CCBB01 = (F_0000000003CCBAF6 ^ F_0000000003CCBAFD);
			auto F_0000000003CCBB20 = (F_0000000003CCBB01 >> 0x2A) ^ F_0000000003CCBB01;
			encryptedPtr = (last_key)*F_0000000003CCBB20;

			cached_character_ptr = encryptedPtr;
			return cached_character_ptr;
		}

		return 0;
	}

	QWORD DecryptBasePtr(QWORD decrypted_character_ptr)
	{
		if (cached_base_ptr != 0)
			return cached_base_ptr;

		if (decrypted_character_ptr)
		{
			QWORD encryptedPtr = *(QWORD*)(decrypted_character_ptr + OFFSET_CHARACTERINFO_BASE_ENCRYPTED_PTR);
			QWORD reversed_addr = *(QWORD*)(g_Data::base + OFFSET_CHARACTERINFO_BASE_REVERSED_ADDR);
			QWORD last_key = *(QWORD*)(~(reversed_addr) +OFFSET_CHARACTERINFO_BASE_DISPLACEMENT);
			
			auto not_peb = __readgsqword(0x60);
			if (encryptedPtr && last_key)
			{
				switch (_rotr64(~not_peb, 0xC) & 0xF)
				{
				case 0:
				{
					//11 sub-formulas
					auto F_00000000014706A5 = encryptedPtr;
					auto F_00000000014708E1 = ((g_Data::base) ^ F_00000000014706A5) * 0x4F42EF0FDEE5152F;
					auto F_000000000147092F = (encryptedPtr + 0x53D5E697);
					auto F_0000000001470936 = ((not_peb) ^ F_00000000014708E1) ^ F_000000000147092F;
					auto F_0000000001470955 = ((last_key)*F_0000000001470936) * 0x54768B16DAD8D377;
					auto F_0000000001470787 = F_0000000001470955 >> 0x1F;
					auto F_000000000147078B = (F_0000000001470955 ^ F_0000000001470787);
					auto F_00000000014707A2 = (F_000000000147078B >> 0x3E) ^ not_peb;
					g_Data::base = (F_000000000147078B ^ F_00000000014707A2);

					break;
				}
				case 1:
				{
					//19 sub-formulas
					auto F_0000000001470B2C = ((encryptedPtr + 0x77288420)) - not_peb;
					auto F_0000000001470B2F = (g_Data::base)+F_0000000001470B2C;
					auto F_0000000001470C50 = F_0000000001470B2F >> 0x03;
					auto F_0000000001470C54 = (F_0000000001470B2F ^ F_0000000001470C50);
					auto F_0000000001470C5A = F_0000000001470C54 >> 0x06;
					auto F_0000000001470C5E = (F_0000000001470C54 ^ F_0000000001470C5A);
					auto F_0000000001470C64 = F_0000000001470C5E >> 0x0C;
					auto F_0000000001470C68 = (F_0000000001470C5E ^ F_0000000001470C64);
					auto F_0000000001470C73 = F_0000000001470C68 >> 0x18;
					auto F_0000000001470C77 = (F_0000000001470C68 ^ F_0000000001470C73);
					auto F_0000000001470C7D = F_0000000001470C77 >> 0x30;
					auto F_0000000001470A59 = ((not_peb)+0x1) * (encryptedPtr + 0xCB98);
					auto F_0000000001470A5D = (F_0000000001470C77 ^ F_0000000001470C7D) + F_0000000001470A59;
					auto F_0000000001470B65 = F_0000000001470A5D >> 0x1C;
					auto F_0000000001470B69 = (F_0000000001470A5D ^ F_0000000001470B65);
					auto F_0000000001470B97 = ((F_0000000001470B69 >> 0x38) ^ F_0000000001470B69) - 0x3EB1F93269886BE5;
					g_Data::base = (((last_key)*F_0000000001470B97) * 0xE1FE2019769DDC57) - 0x3564EF03CDA4D62C;

					break;
				}
				case 2:
				{
					//17 sub-formulas
					auto F_0000000001470D3C = encryptedPtr;
					auto F_0000000001470ECA = (not_peb)+encryptedPtr;
					auto F_0000000001470ECE = ((g_Data::base) ^ F_0000000001470D3C) - F_0000000001470ECA;
					auto F_0000000001470E0C = F_0000000001470ECE >> 0x23;
					auto F_0000000001470E10 = (F_0000000001470ECE ^ F_0000000001470E0C);
					auto F_00000000014710A9 = F_0000000001470E10 >> 0x06;
					auto F_00000000014710AD = (F_0000000001470E10 ^ F_00000000014710A9);
					auto F_00000000014710C0 = F_00000000014710AD >> 0x0C;
					auto F_00000000014710C4 = (F_00000000014710AD ^ F_00000000014710C0);
					auto F_0000000001470F00 = F_00000000014710C4 >> 0x18;
					auto F_0000000001470F04 = (F_00000000014710C4 ^ F_0000000001470F00);
					auto F_0000000001470E35 = F_0000000001470F04 >> 0x30;
					auto F_0000000001470E3C = 0x785C91E3CA03EDB9;
					auto F_000000000147108F = last_key;
					g_Data::base = (((F_0000000001470F04 ^ F_0000000001470E35) ^ F_0000000001470E3C) * 0x30651B444C2329D9) * F_000000000147108F;

					break;
				}
				case 3:
				{
					//13 sub-formulas
					auto F_000000000147136F = 0xECA162E27046CE53;
					auto F_0000000001471379 = (g_Data::base) ^ F_000000000147136F;
					auto F_00000000014713A2 = (((last_key)*F_0000000001471379) * 0x67B89548AC96F923) + not_peb;
					auto F_00000000014712B5 = F_00000000014713A2 >> 0x18;
					auto F_00000000014712B9 = (F_00000000014713A2 ^ F_00000000014712B5);
					auto F_00000000014712EB = F_00000000014712B9 >> 0x30;
					auto F_00000000014711B6 = (encryptedPtr + 0x4996C912);
					auto F_00000000014711AC = (encryptedPtr + 0x78FBBE9D);
					auto F_00000000014711C0 = ((not_peb) ^ F_00000000014711AC) + not_peb;
					auto F_00000000014710DD = encryptedPtr;
					g_Data::base = (((F_00000000014712B9 ^ F_00000000014712EB) + F_00000000014711B6) + F_00000000014711C0) + F_00000000014710DD;

					break;
				}
				case 4:
				{
					//23 sub-formulas
					auto F_000000000147166A = g_Data::base >> 0x18;
					auto F_000000000147166E = (g_Data::base) ^ F_000000000147166A;
					auto F_0000000001471674 = F_000000000147166E >> 0x30;
					auto F_0000000001471678 = (F_000000000147166E ^ F_0000000001471674);
					auto F_000000000147168B = F_0000000001471678 >> 0x15;
					auto F_000000000147168F = (F_0000000001471678 ^ F_000000000147168B);
					auto F_000000000147145C = F_000000000147168F >> 0x2A;
					auto F_0000000001471460 = (F_000000000147168F ^ F_000000000147145C);
					auto F_0000000001471466 = F_0000000001471460 >> 0x05;
					auto F_000000000147146A = (F_0000000001471460 ^ F_0000000001471466);
					auto F_000000000147147D = F_000000000147146A >> 0x0A;
					auto F_0000000001471481 = (F_000000000147146A ^ F_000000000147147D);
					auto F_00000000014714E0 = F_0000000001471481 >> 0x14;
					auto F_00000000014714E4 = (F_0000000001471481 ^ F_00000000014714E0);
					auto F_00000000014716A7 = F_00000000014714E4 >> 0x28;
					auto F_00000000014716AE = 0x2DD464A5C36024E1;
					auto F_00000000014713BE = (encryptedPtr + 0x867);
					auto F_00000000014714D2 = (not_peb) ^ F_00000000014713BE;
					auto F_000000000147152E = last_key;
					auto F_0000000001471648 = 0x0CB7CD2575918BDF;
					g_Data::base = (((((F_00000000014714E4 ^ F_00000000014716A7) ^ F_00000000014716AE) - F_00000000014714D2) * F_000000000147152E) * 0x7D8C8458D4A1C7CD) ^ F_0000000001471648;

					break;
				}
				case 5:
				{
					//14 sub-formulas
					auto F_0000000001471938 = g_Data::base >> 0x1E;
					auto F_000000000147193C = (g_Data::base) ^ F_0000000001471938;
					auto F_0000000001471942 = F_000000000147193C >> 0x3C;
					auto F_0000000001471946 = (F_000000000147193C ^ F_0000000001471942);
					auto F_0000000001471959 = F_0000000001471946 >> 0x08;
					auto F_000000000147195D = (F_0000000001471946 ^ F_0000000001471959);
					auto F_0000000001471963 = F_000000000147195D >> 0x10;
					auto F_0000000001471967 = (F_000000000147195D ^ F_0000000001471963);
					auto F_00000000014718ED = F_0000000001471967 >> 0x20;
					auto F_00000000014718D3 = last_key;
					auto F_000000000147197C = 0xDDBF2B520F0F1B0E;
					g_Data::base = ((((((((F_0000000001471967 ^ F_00000000014718ED) * 0x74ACAC83969A9CDD) + not_peb) * F_00000000014718D3) ^ F_000000000147197C) - encryptedPtr) + 0xFFFFFFFFFFFF2457) + not_peb) * 0x034DDA5302CCC751;

					break;
				}
				case 6:
				{
					//10 sub-formulas
					auto F_0000000001471BAD = (0xFFFFFFFFBC178DBB) - encryptedPtr;
					auto F_0000000001471D48 = last_key;
					auto F_0000000001471B44 = 0xC0B1C44E7FDFFC9D;
					auto F_0000000001471B4E = ((((g_Data::base)+F_0000000001471BAD) * 0xD7A4BDB2C2930CA3) * F_0000000001471D48) ^ F_0000000001471B44;
					auto F_0000000001471B54 = F_0000000001471B4E >> 0x1A;
					auto F_0000000001471B58 = (F_0000000001471B4E ^ F_0000000001471B54);
					auto F_0000000001471DC4 = F_0000000001471B58 >> 0x34;
					g_Data::base = ((F_0000000001471B58 ^ F_0000000001471DC4) - not_peb) - 0x59E6E0A5F4C8DE16;

					break;
				}
				case 7:
				{
					//14 sub-formulas
					auto F_0000000001471F67 = ((g_Data::base) * 0x004D93880079EEA5) - 0x71D55F0902671B85;
					auto F_0000000001471FE9 = ((~(encryptedPtr + 0xD17F)) - not_peb) ^ F_0000000001471F67;
					auto F_00000000014720DF = (not_peb) * (encryptedPtr + 0x326D);
					auto F_00000000014720E3 = (((last_key)*F_0000000001471FE9) - not_peb) - F_00000000014720DF;
					auto F_00000000014720E9 = F_00000000014720E3 >> 0x25;
					auto F_00000000014720ED = (F_00000000014720E3 ^ F_00000000014720E9);
					auto F_0000000001472100 = F_00000000014720ED >> 0x0B;
					auto F_0000000001472104 = (F_00000000014720ED ^ F_0000000001472100);
					auto F_0000000001471F36 = F_0000000001472104 >> 0x16;
					auto F_0000000001471F3A = (F_0000000001472104 ^ F_0000000001471F36);
					auto F_00000000014720B6 = F_0000000001471F3A >> 0x2C;
					g_Data::base = (F_0000000001471F3A ^ F_00000000014720B6);

					break;
				}
				case 8:
				{
					//11 sub-formulas
					auto F_0000000001472371 = (~(encryptedPtr + 0xF55B)) ^ not_peb;
					auto F_0000000001472374 = (g_Data::base) ^ F_0000000001472371;
					auto F_00000000014723BA = ((~(encryptedPtr + 0x1585B8F4)) + 0x1) * not_peb;
					auto F_0000000001472442 = last_key;
					auto F_0000000001472524 = (((((0xF7B0B158EAEEFF57) ^ F_0000000001472374) + F_00000000014723BA) * F_0000000001472442) ^ not_peb) * 0xA02B5CE8B4DC9FE7;
					auto F_000000000147252B = F_0000000001472524 >> 0x1B;
					auto F_000000000147252F = (F_0000000001472524 ^ F_000000000147252B);
					auto F_0000000001472535 = F_000000000147252F >> 0x36;
					g_Data::base = (F_000000000147252F ^ F_0000000001472535);

					break;
				}
				case 9:
				{
					//13 sub-formulas
					auto F_0000000001472694 = (g_Data::base) * 0x78CEFB85FF7A6295;
					auto F_000000000147269B = (encryptedPtr + 0x4B09);
					auto F_00000000014726A8 = (not_peb) ^ F_000000000147269B;
					auto F_00000000014726AB = (((~(encryptedPtr + 0x41CCDFBC)) + not_peb) ^ F_0000000001472694) - F_00000000014726A8;
					auto F_00000000014726B1 = F_00000000014726AB >> 0x14;
					auto F_00000000014726B5 = (F_00000000014726AB ^ F_00000000014726B1);
					auto F_00000000014726BB = F_00000000014726B5 >> 0x28;
					auto F_000000000147270D = (last_key) * 0xEF4F7618B1604EF3;
					auto F_00000000014727A5 = (not_peb) * (encryptedPtr + 0x7B610EC0);
					auto F_00000000014727A9 = (0x8FA85E8164DD63A2) - F_00000000014727A5;
					g_Data::base = ((F_00000000014726B5 ^ F_00000000014726BB) * F_000000000147270D) + F_00000000014727A9;

					break;
				}
				case 10:
				{
					//23 sub-formulas
					auto F_0000000001472947 = (g_Data::base) ^ not_peb;
					auto F_000000000147295D = F_0000000001472947 >> 0x0F;
					auto F_0000000001472961 = (F_0000000001472947 ^ F_000000000147295D);
					auto F_0000000001472C9B = F_0000000001472961 >> 0x1E;
					auto F_0000000001472C9F = (F_0000000001472961 ^ F_0000000001472C9B);
					auto F_0000000001472CD1 = F_0000000001472C9F >> 0x3C;
					auto F_0000000001472CD5 = (F_0000000001472C9F ^ F_0000000001472CD1);
					auto F_0000000001472CDB = F_0000000001472CD5 >> 0x02;
					auto F_0000000001472CDF = (F_0000000001472CD5 ^ F_0000000001472CDB);
					auto F_00000000014728E6 = F_0000000001472CDF >> 0x04;
					auto F_00000000014728EA = (F_0000000001472CDF ^ F_00000000014728E6);
					auto F_0000000001472AB2 = F_00000000014728EA >> 0x08;
					auto F_0000000001472AB6 = (F_00000000014728EA ^ F_0000000001472AB2);
					auto F_00000000014729C9 = F_0000000001472AB6 >> 0x10;
					auto F_00000000014729CD = (F_0000000001472AB6 ^ F_00000000014729C9);
					auto F_00000000014729ED = F_00000000014729CD >> 0x20;
					auto F_00000000014729F4 = 0x53EF7FC5220D0607;
					auto F_0000000001472A0B = ((F_00000000014729CD ^ F_00000000014729ED) ^ F_00000000014729F4) * 0x11ADE0D5616B2A19;
					auto F_0000000001472BDB = ((~(encryptedPtr + 0x49A6B903)) - not_peb) + F_0000000001472A0B;
					auto F_00000000014728EF = 0x3B4BAA1C446A7F7E;
					g_Data::base = ((last_key)*F_0000000001472BDB) ^ F_00000000014728EF;

					break;
				}
				case 11:
				{
					//10 sub-formulas
					auto F_0000000001472D07 = encryptedPtr;
					auto F_0000000001472DF6 = ((g_Data::base) * 0xCC7ADD2DC78DCFF1) + F_0000000001472D07;
					auto F_0000000001472EE9 = F_0000000001472DF6 >> 0x1D;
					auto F_0000000001472EED = (F_0000000001472DF6 ^ F_0000000001472EE9);
					auto F_0000000001472F0D = F_0000000001472EED >> 0x3A;
					auto F_0000000001472F26 = 0x7F7FEC3C289B3536;
					auto F_0000000001472FC5 = last_key;
					g_Data::base = (((((F_0000000001472EED ^ F_0000000001472F0D) ^ F_0000000001472F26) + F_0000000001472D07) ^ not_peb) * F_0000000001472FC5) ^ not_peb;

					break;
				}
				case 12:
				{
					//11 sub-formulas
					auto F_00000000014733E0 = ((encryptedPtr)+0xED58) + not_peb;
					auto F_00000000014730BE = (encryptedPtr + 0x346CCDD3);
					auto F_00000000014730B2 = encryptedPtr;
					auto F_00000000014733A4 = ~(encryptedPtr + 0xD838);
					auto F_00000000014733AD = (~not_peb) + F_00000000014733A4;
					auto F_000000000147327D = ((((((g_Data::base) ^ F_00000000014733E0) ^ not_peb) ^ F_00000000014730BE) + F_00000000014730B2) + F_00000000014733AD) - not_peb;
					auto F_00000000014731C4 = ((last_key)*F_000000000147327D) * 0x3885854439D56E57;
					auto F_00000000014731E5 = F_00000000014731C4 >> 0x26;
					g_Data::base = (F_00000000014731C4 ^ F_00000000014731E5);

					break;
				}
				case 13:
				{
					//18 sub-formulas
					auto F_00000000014735CD = ~(encryptedPtr + 0x833F);
					auto F_00000000014735D0 = (~not_peb) * F_00000000014735CD;
					auto F_00000000014735D7 = ((last_key)*g_Data::base) ^ F_00000000014735D0;
					auto F_000000000147362D = F_00000000014735D7 >> 0x0C;
					auto F_0000000001473631 = (F_00000000014735D7 ^ F_000000000147362D);
					auto F_0000000001473644 = F_0000000001473631 >> 0x18;
					auto F_0000000001473648 = (F_0000000001473631 ^ F_0000000001473644);
					auto F_000000000147365B = F_0000000001473648 >> 0x30;
					auto F_000000000147352E = 0x9552AE0D0F52F5EC;
					auto F_000000000147375D = ((F_0000000001473648 ^ F_000000000147365B) ^ F_000000000147352E) * 0xAEE6B59327BB5DA3;
					auto F_0000000001473764 = F_000000000147375D >> 0x19;
					auto F_0000000001473768 = (F_000000000147375D ^ F_0000000001473764);
					auto F_0000000001473577 = F_0000000001473768 >> 0x32;
					auto F_0000000001473713 = (not_peb)-encryptedPtr;
					auto F_0000000001473727 = 0x23B348F5321496B1;
					g_Data::base = (((F_0000000001473768 ^ F_0000000001473577) + F_0000000001473713) * 0xDF8139F0D23339C7) + F_0000000001473727;

					break;
				}
				case 14:
				{
					//23 sub-formulas
					auto F_0000000001473922 = g_Data::base >> 0x21;
					auto F_0000000001473CB6 = ((g_Data::base) ^ F_0000000001473922) * 0x1E53CC7A2CC93F9D;
					auto F_0000000001473CBD = F_0000000001473CB6 >> 0x0C;
					auto F_0000000001473CC1 = (F_0000000001473CB6 ^ F_0000000001473CBD);
					auto F_0000000001473CD4 = F_0000000001473CC1 >> 0x18;
					auto F_0000000001473CD8 = (F_0000000001473CC1 ^ F_0000000001473CD4);
					auto F_0000000001473C89 = F_0000000001473CD8 >> 0x30;
					auto F_0000000001473C8D = (F_0000000001473CD8 ^ F_0000000001473C89);
					auto F_0000000001473C93 = F_0000000001473C8D >> 0x0B;
					auto F_0000000001473C97 = (F_0000000001473C8D ^ F_0000000001473C93);
					auto F_000000000147399D = F_0000000001473C97 >> 0x16;
					auto F_00000000014739A1 = (F_0000000001473C97 ^ F_000000000147399D);
					auto F_00000000014739C1 = F_00000000014739A1 >> 0x2C;
					auto F_00000000014739C8 = 0xA627A77DE2CFBC81;
					auto F_00000000014739D5 = 0x7F9DBAFB492871E3;
					auto F_0000000001473AB9 = last_key;
					auto F_0000000001473ABA = (((F_00000000014739A1 ^ F_00000000014739C1) ^ F_00000000014739C8) + F_00000000014739D5) * F_0000000001473AB9;
					auto F_0000000001473AC1 = F_0000000001473ABA >> 0x10;
					auto F_0000000001473AC5 = (F_0000000001473ABA ^ F_0000000001473AC1);
					auto F_000000000147398E = F_0000000001473AC5 >> 0x20;
					g_Data::base = (F_0000000001473AC5 ^ F_000000000147398E);

					break;
				}
				case 15:
				{
					//14 sub-formulas
					auto F_0000000001473E3A = (~not_peb) * (encryptedPtr + 0x4731D2A1);
					auto F_0000000001473F0A = 0x8181F4EA5120E5C1;
					auto F_0000000001473F54 = last_key;
					auto F_0000000001473F55 = ((((g_Data::base)+F_0000000001473E3A) + F_0000000001473F0A) + not_peb) * F_0000000001473F54;
					auto F_0000000001473FC2 = F_0000000001473F55 >> 0x0C;
					auto F_0000000001473FC6 = (F_0000000001473F55 ^ F_0000000001473FC2);
					auto F_0000000001473FCC = F_0000000001473FC6 >> 0x18;
					auto F_0000000001473FD0 = (F_0000000001473FC6 ^ F_0000000001473FCC);
					auto F_000000000147401B = F_0000000001473FD0 >> 0x30;
					auto F_0000000001473CFE = encryptedPtr;
					auto F_0000000001473D5D = 0x45A94F2FF7459CF6;
					g_Data::base = (((F_0000000001473FD0 ^ F_000000000147401B) * 0xD8C6CB1D3E3143DB) + F_0000000001473CFE) + F_0000000001473D5D;

					break;
				}
				}

				cached_base_ptr = encryptedPtr;
				return cached_base_ptr;
			}
		}

		return 0;
	}

	// TO BE UPDATED
	QWORD DecryptBonePtr() {
		if (cached_bone_ptr != 0)
			return cached_bone_ptr;

		QWORD encryptedPtr = *(QWORD*)(g_Data::base + OFFSET_BONES_ENCRYPTED_PTR);
		QWORD reversed_addr = *(QWORD*)(g_Data::base + OFFSET_BONES_REVERSED_ADDR);
		QWORD last_key = *(QWORD*)(_byteswap_uint64(reversed_addr) + OFFSET_BONES_DISPLACEMENT);
		
		auto not_peb = __readgsqword(0x60);

		if (encryptedPtr && last_key) {
			auto r8 = encryptedPtr;
			QWORD rbx, rcx, rdi, rbp, rax, rdx, r10, r11, r12, r13, r14, r15 = 0;

			//switch (_rotr64(~not_peb, 0xC) & 0xF) {
			//case 0:
			//	//13 sub-formulas
			//	auto F_0000000001566335 = (g_Data::base + 0x7CAFA6AF);
			//	auto F_0000000001566657 = (~not_peb) ^ F_0000000001566335;
			//	auto F_00000000015664F0 = 0x1D63FB572DAC96BD;
			//	auto F_0000000001566500 = ~not_peb;
			//	auto F_0000000001566475 = last_key;
			//	auto F_0000000001566615 = ((((((((encryptedPtr)+F_0000000001566657) + not_peb) ^ F_00000000015664F0) + F_0000000001566500) - g_Data::base) - 0x69FEBAC7FA606302) * F_0000000001566475) * 0x8719DDE985FD4889;
			//	auto F_00000000015663E8 = F_0000000001566615 >> 0x09;
			//	auto F_00000000015663EC = (F_0000000001566615 ^ F_00000000015663E8);
			//	auto F_000000000156662E = F_00000000015663EC >> 0x12;
			//	auto F_0000000001566632 = (F_00000000015663EC ^ F_000000000156662E);
			//	auto F_0000000001566645 = F_0000000001566632 >> 0x24;
			//	encryptedPtr = (F_0000000001566632 ^ F_0000000001566645);

			//	break;
			//case 1:
			//	//11 sub-formulas
			//	auto F_00000000015668AC = ((g_Data::base + 0x6A15E614)) - not_peb;
			//	auto F_00000000015668B2 = 0xD6F0B6C553FAD828;
			//	auto F_0000000001566A31 = 0x64B0ED2651E569DB;
			//	auto F_000000000156684F = last_key;
			//	auto F_0000000001566850 = (((((encryptedPtr) ^ F_00000000015668AC) ^ F_00000000015668B2) * 0xCEFF5E7F782B739D) + F_0000000001566A31) * F_000000000156684F;
			//	auto F_00000000015668D4 = F_0000000001566850 >> 0x28;
			//	auto F_00000000015668DB = (g_Data::base + 0xA051);
			//	auto F_00000000015668E5 = ((F_0000000001566850 ^ F_00000000015668D4) + not_peb) + F_00000000015668DB;
			//	auto F_00000000015668EB = F_00000000015668E5 >> 0x22;
			//	encryptedPtr = (F_00000000015668E5 ^ F_00000000015668EB);

			//	break;
			//case 2:
			//	//8 sub-formulas
			//	auto F_0000000001566BAE = encryptedPtr >> 0x13;
			//	auto F_0000000001566BB2 = (encryptedPtr) ^ F_0000000001566BAE;
			//	auto F_0000000001566BB8 = F_0000000001566BB2 >> 0x26;
			//	auto F_0000000001566BBC = (F_0000000001566BB2 ^ F_0000000001566BB8);
			//	auto F_0000000001566C83 = F_0000000001566BBC >> 0x1D;
			//	auto F_0000000001566C87 = (F_0000000001566BBC ^ F_0000000001566C83);
			//	auto F_0000000001566DFA = g_Data::base;
			//	auto F_0000000001566E07 = (((F_0000000001566C87 >> 0x3A) ^ F_0000000001566C87) + not_peb) + F_0000000001566DFA;
			//	encryptedPtr = ((((last_key)*F_0000000001566E07) * 0xB9171C866C618EB9) - 0x2D2CF1162FD23971) * 0xB493C6998A037BCF;

			//	break;
			//case 3:
			//	//18 sub-formulas
			//	auto F_0000000001566E93 = (g_Data::base + 0xF8CB);
			//	auto F_0000000001566F63 = (not_peb) ^ F_0000000001566E93;
			//	auto F_00000000015671E7 = (((encryptedPtr)-F_0000000001566F63) * 0xDD42A94846BE5CF9) - 0x23DD68816620F860;
			//	auto F_00000000015671F8 = F_00000000015671E7 >> 0x1F;
			//	auto F_00000000015671FF = (F_00000000015671E7 ^ F_00000000015671F8);
			//	auto F_0000000001567223 = (F_00000000015671FF >> 0x3E) ^ F_00000000015671FF;
			//	auto F_0000000001567234 = (last_key)*F_0000000001567223;
			//	auto F_000000000156723B = F_0000000001567234 >> 0x1A;
			//	auto F_000000000156723F = (F_0000000001567234 ^ F_000000000156723B);
			//	auto F_00000000015670ED = F_000000000156723F >> 0x34;
			//	auto F_00000000015670F1 = (F_000000000156723F ^ F_00000000015670ED);
			//	auto F_0000000001566FC6 = F_00000000015670F1 >> 0x05;
			//	auto F_0000000001566FCA = (F_00000000015670F1 ^ F_0000000001566FC6);
			//	auto F_0000000001567299 = F_0000000001566FCA >> 0x0A;
			//	auto F_000000000156729D = (F_0000000001566FCA ^ F_0000000001567299);
			//	auto F_00000000015672A3 = F_000000000156729D >> 0x14;
			//	auto F_00000000015672A7 = (F_000000000156729D ^ F_00000000015672A3);
			//	auto F_00000000015672AD = F_00000000015672A7 >> 0x28;
			//	encryptedPtr = (F_00000000015672A7 ^ F_00000000015672AD) - not_peb;

			//	break;
			//case 4:
			//	//11 sub-formulas
			//	auto F_00000000015675CD = ~not_peb;
			//	auto F_00000000015675D6 = ((~(g_Data::base + 0x71FA92D3)) + not_peb) ^ F_00000000015675CD;
			//	auto F_0000000001567395 = (g_Data::base + 0xE615);
			//	auto F_0000000001567457 = 0x341438EDA73EF51F;
			//	auto F_000000000156759C = (((((encryptedPtr) ^ F_00000000015675D6) ^ F_0000000001567395) - not_peb) ^ F_0000000001567457) * 0x14114CF0012C77DD;
			//	auto F_000000000156761D = (g_Data::base + 0x56E6F4CF);
			//	auto F_0000000001567624 = ((~not_peb) + F_000000000156759C) + F_000000000156761D;
			//	auto F_0000000001567631 = (last_key)*F_0000000001567624;
			//	auto F_0000000001567652 = F_0000000001567631 >> 0x1E;
			//	auto F_0000000001567656 = (F_0000000001567631 ^ F_0000000001567652);
			//	auto F_000000000156765C = F_0000000001567656 >> 0x3C;
			//	encryptedPtr = (F_0000000001567656 ^ F_000000000156765C);

			//	break;
			//case 5:
			//	//11 sub-formulas
			//	auto F_000000000156798C = encryptedPtr >> 0x0E;
			//	auto F_0000000001567990 = (encryptedPtr) ^ F_000000000156798C;
			//	auto F_0000000001567806 = F_0000000001567990 >> 0x1C;
			//	auto F_000000000156780A = (F_0000000001567990 ^ F_0000000001567806);
			//	auto F_0000000001567810 = F_000000000156780A >> 0x38;
			//	auto F_0000000001567814 = (F_000000000156780A ^ F_0000000001567810);
			//	auto F_00000000015677B1 = F_0000000001567814 >> 0x1E;
			//	auto F_00000000015677B5 = (F_0000000001567814 ^ F_00000000015677B1);
			//	auto F_00000000015677BF = (F_00000000015677B5 >> 0x3C) ^ F_00000000015677B5;
			//	auto F_0000000001567932 = last_key;
			//	auto F_0000000001567948 = 0x7E4493B94BFB5A65;
			//	encryptedPtr = (((((((not_peb) * (g_Data::base + 0x62E85476)) + F_00000000015677BF) - not_peb) * F_0000000001567932) * 0x4558DB96CDC57E85) ^ F_0000000001567948) * 0x9F4C946F17D26CC9;

			//	break;
			//case 6:
			//	//12 sub-formulas
			//	auto F_0000000001567CE1 = 0xE2C21B2DBAEAFF8D;
			//	//auto F_0000000001567A39 = [BASE_MEM + 0x0000000004D96196];
			//	//auto F_0000000001567BA0 = ((__ROL8__(rax, 0x10)) ^ F_0000000001567A39)BSWAP[BASE_MEM + 0x0000000004D96196];
			//	auto F_0000000001567BA3 = last_key;
			//	auto F_0000000001567BA4 = ((encryptedPtr) ^ F_0000000001567CE1) * F_0000000001567BA3;
			//	auto F_0000000001567D4C = F_0000000001567BA4 >> 0x1B;
			//	auto F_0000000001567D50 = (F_0000000001567BA4 ^ F_0000000001567D4C);
			//	auto F_0000000001567D67 = F_0000000001567D50 >> 0x36;
			//	auto F_0000000001567D6B = ((not_peb) * (g_Data::base + 0x2B67)) ^ F_0000000001567D67;
			//	auto F_0000000001567D2A = (g_Data::base + 0x4AC7F13D);
			//	auto F_0000000001567D34 = 0x5CA637B6D1176705;
			//	auto F_0000000001567BE2 = (((((F_0000000001567BA4 ^ F_0000000001567D4C) ^ F_0000000001567D6B) ^ not_peb) ^ F_0000000001567D2A) ^ F_0000000001567D34) * 0x14D8FEFE4440BE25;
			//	encryptedPtr = ((not_peb) * (g_Data::base + 0x9610)) ^ F_0000000001567BE2;

			//	break;
			//case 7:
			//	//8 sub-formulas
			//	auto F_000000000156813B = (encryptedPtr >> 0x21) ^ encryptedPtr;
			//	auto F_0000000001567EA4 = last_key;
			//	auto F_00000000015680E6 = 0xA8BEABA458AB936E;
			//	auto F_0000000001568175 = ((not_peb)+0x1) * (g_Data::base + 0x48E7);
			//	auto F_0000000001568181 = 0x3DF526680D4C7658;
			//	auto F_000000000156818E = g_Data::base;
			//	encryptedPtr = (((((((((g_Data::base + 0x1DB4)) ^ not_peb) + F_000000000156813B) * F_0000000001567EA4) * 0x98A6383A12B300ED) ^ F_00000000015680E6) + F_0000000001568175) ^ F_0000000001568181) + F_000000000156818E;

			//	break;
			//case 8:
			//	//8 sub-formulas
			//	auto F_000000000156836E = encryptedPtr >> 0x16;
			//	auto F_0000000001568372 = (encryptedPtr) ^ F_000000000156836E;
			//	auto F_0000000001568378 = F_0000000001568372 >> 0x2C;
			//	auto F_000000000156837C = (F_0000000001568372 ^ F_0000000001568378);
			//	auto F_00000000015682A6 = (not_peb) * (g_Data::base + 0x82CF);
			//	auto F_00000000015682C5 = ((0xF9A4CF28E20EB42D) * F_000000000156837C) + F_00000000015682A6;
			//	auto F_000000000156831A = (((last_key)*F_00000000015682C5) - not_peb) ^ not_peb;
			//	auto F_000000000156830D = 0x704D32D7BF7A8741;
			//	encryptedPtr = ((F_000000000156831A >> 0x27) ^ F_000000000156831A) ^ F_000000000156830D;

			//	break;
			//case 9:
			//	//15 sub-formulas
			//	auto F_0000000001568826 = last_key;
			//	auto F_0000000001568827 = ((encryptedPtr) * 0x8751C74A06104EA1) * F_0000000001568826;
			//	auto F_000000000156882E = F_0000000001568827 >> 0x14;
			//	auto F_0000000001568832 = (F_0000000001568827 ^ F_000000000156882E);
			//	auto F_0000000001568838 = F_0000000001568832 >> 0x28;
			//	auto F_000000000156883C = (F_0000000001568832 ^ F_0000000001568838);
			//	auto F_000000000156884F = F_000000000156883C >> 0x18;
			//	auto F_0000000001568853 = (F_000000000156883C ^ F_000000000156884F);
			//	auto F_000000000156866C = F_0000000001568853 >> 0x30;
			//	auto F_000000000156879C = 0x416F05917E813F3C;
			//	auto F_0000000001568624 = 0xE3266DC241623581;
			//	auto F_0000000001568634 = ~not_peb;
			//	auto F_000000000156855B = (g_Data::base + 0x741E);
			//	encryptedPtr = (((((F_0000000001568853 ^ F_000000000156866C) ^ F_000000000156879C) ^ not_peb) ^ F_0000000001568624) + F_0000000001568634) + F_000000000156855B;

			//	break;
			//case 10:
			//	//7 sub-formulas
			//	auto F_0000000001568A2F = encryptedPtr >> 0x14;
			//	auto F_0000000001568A33 = (encryptedPtr) ^ F_0000000001568A2F;
			//	auto F_0000000001568945 = g_Data::base;
			//	auto F_000000000156894C = 2 * F_0000000001568945;
			//	auto F_0000000001568AC4 = last_key;
			//	encryptedPtr = (((((((F_0000000001568A33 >> 0x28) ^ F_0000000001568A33) + F_000000000156894C) * 0x85BA1A7F58EF4447) ^ not_peb) * 0xD3CEB6C5763022F9) * 0xFC6547DA0E476F0F) * F_0000000001568AC4;

			//	break;
			//case 11:
			//	//19 sub-formulas
			//	auto F_0000000001568C8A = last_key;
			//	auto F_0000000001568CD1 = (~(g_Data::base + 0x63315392)) - not_peb;
			//	auto F_0000000001568CD4 = (((encryptedPtr) * 0xD518D665A8814691) * F_0000000001568C8A) + F_0000000001568CD1;
			//	auto F_0000000001568DF9 = F_0000000001568CD4 >> 0x25;
			//	auto F_0000000001568DFD = (F_0000000001568CD4 ^ F_0000000001568DF9);
			//	auto F_0000000001568E03 = F_0000000001568DFD >> 0x02;
			//	auto F_0000000001568E07 = (F_0000000001568DFD ^ F_0000000001568E03);
			//	auto F_0000000001568CA4 = F_0000000001568E07 >> 0x04;
			//	auto F_0000000001568CA8 = (F_0000000001568E07 ^ F_0000000001568CA4);
			//	auto F_0000000001568CBB = F_0000000001568CA8 >> 0x08;
			//	auto F_0000000001568CBF = (F_0000000001568CA8 ^ F_0000000001568CBB);
			//	auto F_0000000001568D6C = F_0000000001568CBF >> 0x10;
			//	auto F_0000000001568D70 = (F_0000000001568CBF ^ F_0000000001568D6C);
			//	auto F_0000000001568D90 = (~not_peb) * (g_Data::base + 0x47DC);
			//	auto F_0000000000000000 = not_peb;
			//	auto F_0000000001568D9E = 2 * F_0000000000000000;
			//	auto F_0000000001568D97 = (g_Data::base + 0x3C17F084);
			//	encryptedPtr = ((((F_0000000001568D70 >> 0x20) ^ F_0000000001568D70) + F_0000000001568D90) + F_0000000001568D9E) + F_0000000001568D97;

			//	break;
			//case 12:
			//	//8 sub-formulas
			//	auto F_00000000015690B4 = encryptedPtr >> 0x08;
			//	auto F_00000000015690BB = (encryptedPtr) ^ F_00000000015690B4;
			//	auto F_00000000015690DC = F_00000000015690BB >> 0x10;
			//	auto F_00000000015690E0 = (F_00000000015690BB ^ F_00000000015690DC);
			//	auto F_00000000015690FA = (F_00000000015690E0 >> 0x20) ^ F_00000000015690E0;
			//	auto F_00000000015691FC = ((last_key)*F_00000000015690FA) * 0x77AB7F6BC54E95B9;
			//	auto F_000000000156921D = 0x614AE48A6F6D5CF5;
			//	auto F_0000000001569296 = ((g_Data::base)+0x4E76D85B) + not_peb;
			//	encryptedPtr = ((((((((~(g_Data::base + 0x5717699A)) - not_peb) ^ F_00000000015691FC) - not_peb) - g_Data::base) - 0x7432) ^ F_000000000156921D) * 0x2443BEFDD354E759) ^ F_0000000001569296;

			//	break;
			//case 13:
			//	//14 sub-formulas
			//	auto F_00000000015694D1 = (encryptedPtr)+not_peb;
			//	auto F_000000000156943D = F_00000000015694D1 >> 0x03;
			//	auto F_0000000001569441 = (F_00000000015694D1 ^ F_000000000156943D);
			//	auto F_0000000001569634 = F_0000000001569441 >> 0x06;
			//	auto F_0000000001569638 = (F_0000000001569441 ^ F_0000000001569634);
			//	auto F_000000000156963E = F_0000000001569638 >> 0x0C;
			//	auto F_0000000001569642 = (F_0000000001569638 ^ F_000000000156963E);
			//	auto F_0000000001569662 = F_0000000001569642 >> 0x18;
			//	auto F_0000000001569666 = (F_0000000001569642 ^ F_0000000001569662);
			//	auto F_000000000156966C = F_0000000001569666 >> 0x30;
			//	auto F_0000000001569670 = (F_0000000001569666 ^ F_000000000156966C);
			//	auto F_00000000015695F1 = 0x2CC90D1D81AF3364;
			//	auto F_00000000015695FB = ((0x35C3701ED3899B33) * F_0000000001569670) + F_00000000015695F1;
			//	auto F_0000000001569595 = ((g_Data::base + 0x661F)) - not_peb;
			//	encryptedPtr = ((((last_key)*F_00000000015695FB) ^ F_0000000001569595) - not_peb) * 0xD273001CA03268E7;

			//	break;
			//case 14:
			//	//21 sub-formulas
			//	auto F_00000000015698D7 = encryptedPtr >> 0x09;
			//	auto F_00000000015698DB = (encryptedPtr) ^ F_00000000015698D7;
			//	auto F_00000000015698E1 = F_00000000015698DB >> 0x12;
			//	auto F_00000000015698E5 = (F_00000000015698DB ^ F_00000000015698E1);
			//	auto F_0000000001569A13 = F_00000000015698E5 >> 0x24;
			//	auto F_0000000001569A39 = 0x3ABA76649EC1E83E;
			//	auto F_0000000001569A43 = (F_00000000015698E5 ^ F_0000000001569A13) + F_0000000001569A39;
			//	auto F_0000000001569A49 = F_0000000001569A43 >> 0x06;
			//	auto F_0000000001569A4D = (F_0000000001569A43 ^ F_0000000001569A49);
			//	auto F_0000000001569A60 = F_0000000001569A4D >> 0x0C;
			//	auto F_0000000001569A64 = (F_0000000001569A4D ^ F_0000000001569A60);
			//	auto F_00000000015698FD = F_0000000001569A64 >> 0x18;
			//	auto F_0000000001569901 = (F_0000000001569A64 ^ F_00000000015698FD);
			//	auto F_0000000001569914 = F_0000000001569901 >> 0x30;
			//	auto F_0000000001569784 = last_key;
			//	auto F_0000000001569785 = (F_0000000001569901 ^ F_0000000001569914) * F_0000000001569784;
			//	auto F_0000000001569799 = F_0000000001569785 >> 0x21;
			//	auto F_0000000001569A9E = ((not_peb)-0x0000000800000003) - 0x7BADC062;
			//	auto F_0000000001569947 = 0x06F91A134C70876A;
			//	encryptedPtr = (((F_0000000001569785 ^ F_0000000001569799) * 0xC6DFA27AD0E4552F) ^ F_0000000001569A9E) + F_0000000001569947;

			//	break;
			//case 15:
			//	//17 sub-formulas
			//	auto F_0000000001569E94 = g_Data::base;
			//	auto F_0000000001569E9B = (encryptedPtr)+F_0000000001569E94;
			//	auto F_0000000001569EA1 = F_0000000001569E9B >> 0x01;
			//	auto F_0000000001569EA4 = (F_0000000001569E9B ^ F_0000000001569EA1);
			//	auto F_0000000001569BC1 = F_0000000001569EA4 >> 0x02;
			//	auto F_0000000001569BC5 = (F_0000000001569EA4 ^ F_0000000001569BC1);
			//	auto F_0000000001569DDA = F_0000000001569BC5 >> 0x04;
			//	auto F_0000000001569DDE = (F_0000000001569BC5 ^ F_0000000001569DDA);
			//	auto F_0000000001569E48 = F_0000000001569DDE >> 0x08;
			//	auto F_0000000001569E4C = (F_0000000001569DDE ^ F_0000000001569E48);
			//	auto F_0000000001569D44 = F_0000000001569E4C >> 0x10;
			//	auto F_0000000001569D48 = (F_0000000001569E4C ^ F_0000000001569D44);
			//	auto F_0000000001569D68 = F_0000000001569D48 >> 0x20;
			//	auto F_0000000001569CC6 = last_key;
			//	auto F_0000000001569CC2 = (not_peb) * (g_Data::base + 0x24F38A04);
			//	auto F_0000000001569CD8 = ((((F_0000000001569D48 ^ F_0000000001569D68) * 0x3233210024041DC1) + not_peb) * F_0000000001569CC6) ^ F_0000000001569CC2;
			//	auto F_0000000001569DE9 = F_0000000001569CD8 >> 0x27;
			//	encryptedPtr = (F_0000000001569CD8 ^ F_0000000001569DE9) * 0x19639FED92664C77;

			//	break;
			//}

			cached_bone_ptr = r8;
			return cached_bone_ptr;
		}

		return 0;
	}

	// TO BE UPDATED
	unsigned short DecBoneIndex(uint32_t i)
	{
		QWORD rax, rbx, rcx, rdx, r8, r9, r10, r11, r15d = 0;

		rcx = i * 0x13C8;
		rax = 0xB351CEF3C056B94D;
		r11 = g_Data::base;
		rax = _umul128(rax, rcx, (QWORD*)&rdx);
		rax = rcx;
		r10 = 0xD5C1530099A2F3A9;
		rax -= rdx;
		rax >>= 0x1;
		rax += rdx;
		rax >>= 0xD;
		rax = rax * 0x25A3;
		rcx -= rax;
		rax = 0x904BD65E1CD47641;
		r8 = rcx * 0x25A3;
		rax = _umul128(rax, r8, (QWORD*)&rdx);
		rax = r8;
		rax -= rdx;
		rax >>= 0x1;
		rax += rdx;
		rax >>= 0xD;
		rax = rax * 0x28EE;
		r8 -= rax;
		rax = 0xB70FBB5A19BE3659;
		rax = _umul128(rax, r8, (QWORD*)&rdx);
		rax = 0x5C9882B931057263;
		rdx >>= 0x8;
		rcx = rdx * 0x166;
		rax = _umul128(rax, r8, (QWORD*)&rdx);
		rax = r8;
		rax -= rdx;
		rax >>= 0x1;
		rax += rdx;
		rax >>= 0x5;
		rcx += rax;
		rax = rcx * 0x5E;
		rcx = r8 + r8 * 2;
		rcx <<= 0x5;
		rcx -= rax;
		rax = *(uint16_t*)(rcx + r11 * 1 + 0x4DF2FE0);
		r8 = rax * 0x13C8;
		rax = r10;
		rax = _umul128(rax, r8, (QWORD*)&rdx);
		rax = r10;
		rdx >>= 0xD;
		rcx = rdx * 0x2653;
		r8 -= rcx;
		r9 = r8 * 0x3087;
		rax = _umul128(rax, r9, (QWORD*)&rdx);
		rdx >>= 0xD;
		rax = rdx * 0x2653;
		r9 -= rax;
		rax = 0x97657E1ED980B69;
		rax = _umul128(rax, r9, (QWORD*)&rdx);
		rax = r9;
		rax -= rdx;
		rax >>= 0x1;
		rax += rdx;
		rax >>= 0xA;
		rcx = rax * 0x7B7;
		rax = 0xAAAAAAAAAAAAAAAB;
		rax = _umul128(rax, r9, (QWORD*)&rdx);
		rdx >>= 0x2;
		rcx += rdx;
		rax = rcx + rcx * 2;
		rcx = r9 * 0xE;
		rax <<= 0x2;
		rcx -= rax;
		r15d = *(uint16_t*)(rcx + r11 * 1 + 0x4DFB360);

		return (QWORD)r15d;
	}

	int DecryptVisibleFlag(int i, QWORD valid_list)
	{
		auto ptr = valid_list + ((i + i * 8) * 8) + OFFSET_VISIBLE_OFFSET;
		DWORD dw1 = (*(DWORD*)(ptr + 4) ^ (DWORD)ptr);
		DWORD dw2 = ((dw1 + 2) * dw1);
		BYTE dec_visible_flag = *(BYTE*)(ptr) ^ BYTE1(dw2) ^ (BYTE)dw2;

		return (int)dec_visible_flag;
	}

	QWORD GetVisibleBase()
	{
		if (cached_visible_base != 0)
			return cached_visible_base;

		for (int j = 4000; j >= 0; j--)
		{
			QWORD nIndex = (j + (j << 2)) << 0x6;
			QWORD vis_base_ptr = g_Data::base + OFFSET_VISIBLE_FUNCTION_DISTRIBUTE + nIndex;
			QWORD cmp_function = *(QWORD*)(vis_base_ptr + 0x90);

			if (!cmp_function)
				continue;

			QWORD about_visible = g_Data::base + OFFSET_VISIBLE_ABOUT_FUNCTION;
			if (cmp_function == about_visible)
			{
				cached_visible_base = vis_base_ptr;
				break;
			}
		}

		return cached_visible_base;
	}

	int GetClientCount()
	{
		QWORD decryptPtr = DecryptCharacterInfoPtr();
		if (decryptPtr)
		{
			auto client_ptr = *reinterpret_cast<uint64_t*>(decryptPtr + OFFSET_CHARACTERINFO_LOCAL_BASE);
			if (client_ptr)
			{
				return *(unsigned int*)(client_ptr + 0x1C);
			}
		}

		return 0;
	}

	float posInfo_t::GetRotation() const
	{
		float rotation = m_rotation;
		if (rotation < 0)
			rotation = 360.0f - (rotation * -1);

		rotation += 90.0f;

		if (rotation >= 360.0f)
			rotation = rotation - 360.0f;

		return rotation;
	}

	characterInfo_t* GetCharacterInfo(QWORD decryptedPtr, int i)
	{
		if (!decryptedPtr)
			decryptedPtr = DecryptCharacterInfoPtr();

		QWORD BaseAddress = DecryptBasePtr(decryptedPtr);
		if (BaseAddress) 
			return (characterInfo_t*)(BaseAddress + (i * OFFSET_CHARACTERINFO_SIZE));

		return nullptr;
	}

	characterInfo_t* GetLocalCharacterInfo(int* local_index)
	{
		QWORD decryptedPtr = DecryptCharacterInfoPtr();
		if (decryptedPtr)
		{
			unsigned __int64 BaseAddress = *(unsigned __int64*)(decryptedPtr + OFFSET_CHARACTERINFO_LOCAL_BASE);
			if (BaseAddress)
			{
				*local_index = *(DWORD*)(BaseAddress + OFFSET_CHARACTERINFO_LOCAL_INDEX);

				return GetCharacterInfo(decryptedPtr, *local_index);
			}
		}

		*local_index = 0;

		return nullptr;
	}

	clientInfo_t* GetClientInfo(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_Data::base + OFFSET_CLIENT_ARRAY);
		if (bgs)
		{
			clientInfo_t* pClientInfo = (clientInfo_t*)(bgs + OFFSET_CLIENT_HANDLE + ((i + i * 8) << 4));
			if (pClientInfo)
				return pClientInfo;
		}

		return 0;
	}

	refdef_t* GetRefdef()
	{
		uint32_t crypt_0 = *(uint32_t*)(g_Data::base + OFFSET_VIEWPORT_REFDEF);
		uint32_t crypt_1 = *(uint32_t*)(g_Data::base + OFFSET_VIEWPORT_REFDEF + 0x4);
		uint32_t crypt_2 = *(uint32_t*)(g_Data::base + OFFSET_VIEWPORT_REFDEF + 0x8);

		// lower 32 bits
		uint32_t entry_1 = (uint32_t)(g_Data::base + OFFSET_VIEWPORT_REFDEF);
		uint32_t entry_2 = (uint32_t)(g_Data::base + OFFSET_VIEWPORT_REFDEF + 0x4);

		// decryption
		uint32_t _low = entry_1 ^ crypt_2;
		uint32_t _high = entry_2 ^ crypt_2;

		uint32_t low_bit = crypt_0 ^ _low * (_low + 2);
		uint32_t high_bit = crypt_1 ^ _high * (_high + 2);

		return (refdef_t*)(((QWORD)high_bit << 32) + low_bit);
	}

	std::string GetPlayerName(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_Data::base + OFFSET_CLIENT_ARRAY);
		if (bgs)
		{
			clientInfo_t* clientInfo_ptr = (clientInfo_t*)(bgs + OFFSET_CLIENT_HANDLE + (i * 0xD0));
			int length = (int)strlen(clientInfo_ptr->m_szName);
			for (int j = 0; j < length; ++j)
			{
				char ch = clientInfo_ptr->m_szName[j];
				bool is_english = ch >= 0 && ch <= 127;
				if (!is_english)
					return xorstr("Player");
			}

			return clientInfo_ptr->m_szName;
		}

		return xorstr("Player");
	}

	bool UpdateVisibleAddr(int i)
	{
		auto vis_base_ptr = GetVisibleBase();
		if (vis_base_ptr)
		{
			QWORD visible_head_offset = *(QWORD*)(vis_base_ptr + 0x108);
			if (visible_head_offset)
			{
				auto visible_flag = DecryptVisibleFlag(i, visible_head_offset);
				if (visible_flag == 3)
				{
					current_visible_offset = vis_base_ptr;

					return true;
				}
			}
		}

		return false;
	}

	bool IsVisible(int i)
	{
		if (last_visible_offset) {
			QWORD visible_list_ptr = *(QWORD*)(last_visible_offset + 0x108);
			if (visible_list_ptr) {
				QWORD index = visible_list_ptr + ((i + i * 8) * 8) + OFFSET_VISIBLE_OFFSET;
				visible_t* visible = (visible_t*)(index);
				if (visible) {
					DWORD v_data1 = *(PDWORD)&visible->data[4];
					v_data1 ^= (DWORD)index;

					DWORD v_data2 = v_data1 + 2;
					v_data2 *= v_data1;
					v_data1 = v_data2;
					v_data1 = v_data1 >> 8;

					UCHAR visible_flag = (UCHAR)v_data1 ^ (UCHAR)v_data2;
					visible_flag = visible_flag ^ *(UCHAR*)&visible->data[0];
					
					return visible_flag == 3;
				}
			}
		}

		return false;
	}

	void UpdateLastVisible()
	{
		last_visible_offset = current_visible_offset;
	}

	bool GetBonesByIndex(int i, int bone_id, Vector3* Out_bone_pos)
	{
		uint64_t encryptedPtr = *(uint64_t*)(g_Data::base + OFFSET_BONES_ENCRYPTED_PTR);
		if (encryptedPtr)
		{
			uint64_t decrypted_ptr = DecryptBonePtr();
			if (decrypted_ptr)
			{
				unsigned short index = DecBoneIndex(i);
				if (index != 0)
				{
					uint64_t bone_ptr = *(uint64_t*)(decrypted_ptr + (index * OFFSET_BONES_SIZE) + 0xC0);
					if (bone_ptr)
					{
						Vector3 bone_pos = *(Vector3*)(bone_ptr + (bone_id * 0x20) + 0x10);
						uint64_t characterInfo_ptr = DecryptCharacterInfoPtr();
						if (characterInfo_ptr)
						{
							Vector3 BasePos = *(Vector3*)(characterInfo_ptr + OFFSET_BONES_BASE_POS);

							bone_pos.x += BasePos.x;
							bone_pos.y += BasePos.y;
							bone_pos.z += BasePos.z;
							*Out_bone_pos = bone_pos;

							return true;
						}
					}
				}
			}
		}

		return false;
	}

	void NoRecoil()
	{
		uint64_t characterInfo_ptr = DecryptCharacterInfoPtr();
		if (characterInfo_ptr)
		{
			// up, down
			QWORD r12 = characterInfo_ptr;
			r12 += OFFSET_CHARACTERINFO_RECOIL;
			QWORD rsi = r12 + 0x4;
			DWORD edx = *(QWORD*)(r12 + 0xC);
			DWORD ecx = (DWORD)r12;
			ecx ^= edx;
			DWORD eax = (DWORD)((QWORD)ecx + 0x2);
			eax *= ecx;
			ecx = (DWORD)rsi;
			ecx ^= edx;
			DWORD udZero = eax;
			//left, right
			eax = (DWORD)((QWORD)ecx + 0x2);
			eax *= ecx;
			DWORD lrZero = eax;
			*(DWORD*)(r12) = udZero;
			*(DWORD*)(rsi) = lrZero;
		}
	}

	int GetPlayerHealth(int i)
	{
		uint64_t bgs = *(uint64_t*)(g_Data::base + OFFSET_CLIENT_ARRAY);
		if (bgs)
		{
			clientInfo_t* pClientInfo = (clientInfo_t*)(bgs + OFFSET_CLIENT_HANDLE + +(i * 0xD0));
			if (pClientInfo)
			{
				return pClientInfo->GetHealth();
			}
		}

		return 0;
	}

	void StartTick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount64();
		bUpdateTick = lastTick < t;
		if (bUpdateTick)
			lastTick = t + nTickTime;
	}

	void UpdateVelMap(int index, Vector3 vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void ClearMap()
	{
		if (!velocityMap.empty())
			velocityMap.clear();
	}

	void ClearCachedPtr()
	{
		cached_character_ptr = 0;
		cached_base_ptr = 0;
		cached_bone_ptr = 0;
		cached_visible_base = 0;
	}

	Vector3 GetSpeed(int index)
	{
		return velocityMap[index].delta;
	}

	Vector3 GetPrediction(int local_index, int index, float fdistance, Vector3 position, float bullet_speed)
	{
		auto local_speed = GetSpeed(local_index);
		auto target_speed = GetSpeed(index);
		auto final_speed = target_speed - local_speed;
		auto travel_time = fdistance / bullet_speed;

		position.x += travel_time * final_speed.x;
		position.y += travel_time * final_speed.y;

		return position;
	}

	Result MidnightSolver(float a, float b, float c)
	{
		Result res;
		double subsquare = b * b - 4 * a * c;
		if (subsquare < 0)
		{
			res.hasResult = false;
		}
		else
		{
			res.hasResult = true,
			res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}

		return res;
	}

	Vector3 PredictionSolver(Vector3 local_pos, Vector3 position, int index, float bullet_speed)
	{
		Vector3 aimPosition = Vector3().Zero();
		auto target_speed = GetSpeed(index);

		local_pos -= position; 

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position; 

		Result r = MidnightSolver(a, b, c);
		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;
	}
}