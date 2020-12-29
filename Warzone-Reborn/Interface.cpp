#include "Interface.h"
#include "Aimbot.h"
#include "Settings.h"
#include "ESP.h"
#include "GUI.h"
#include "Misc.h"

namespace Interface
{
	g_SDK::refdef_t* rRefDef;

	float GetDistance(const Vector3& lhs, const Vector3& rhs)
	{
		const float distanceX = lhs.x - rhs.x;
		const float distanceY = lhs.y - rhs.y;
		const float distanceZ = lhs.z - rhs.z;

		return std::sqrt((distanceX * distanceX) + (distanceY * distanceY) + (distanceZ * distanceZ));
	}

	Vector3 GetCameraLocation()
	{
		float x = *(float*)(*(uint64_t*)(g_Data::base + OFFSET_VIEWPORT_CAMERA) + OFFSET_VIEWPORT_CAMERA_VIEW_X);
		float y = *(float*)(*(uint64_t*)(g_Data::base + OFFSET_VIEWPORT_CAMERA) + OFFSET_VIEWPORT_CAMERA_VIEW_Y);
		float z = *(float*)(*(uint64_t*)(g_Data::base + OFFSET_VIEWPORT_CAMERA) + OFFSET_VIEWPORT_CAMERA_VIEW_Z);

		return Vector3{ x, y, z };
	}

	bool WorldToScreen(const Vector3& WorldPos, Vector2* ScreenPos)
	{
		Vector3 ViewOrig = GetCameraLocation();
		auto refdef = rRefDef;

		Vector3 vLocal, vTransform;

		vLocal = WorldPos - ViewOrig;

		// get our dot products from viewAxis
		vTransform.x = vLocal.Dot(refdef->ViewAxis[1]);
		vTransform.y = vLocal.Dot(refdef->ViewAxis[2]);
		vTransform.z = vLocal.Dot(refdef->ViewAxis[0]);

		// make sure it is in front of us
		if (vTransform.z < 0.01f)
			return false;

		ScreenPos->x = ((refdef->Width / 2) * (1 - (vTransform.x / refdef->FovX / vTransform.z)));
		ScreenPos->y = ((refdef->Height / 2) * (1 - (vTransform.y / refdef->FovY / vTransform.z)));

		return true;
	}

	bool HeadToScreen(Vector3 pos, Vector2* pos_out, int stance)
	{
		Vector2 w2s_head;
		pos.z += 55.f;

		if (!WorldToScreen(pos, &w2s_head))
		{
			return false;
		}
		else if (stance == g_SDK::CROUCH)
		{
			pos.z -= 20.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == g_SDK::KNOCK)
		{
			pos.z -= 28.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}
		else if (stance == g_SDK::PRONE)
		{
			pos.z -= 50.f;
			if (!WorldToScreen(pos, &w2s_head))
				return false;
		}

		pos_out->x = w2s_head.x;
		pos_out->y = w2s_head.y;

		return true;
	}

	void DrawLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		float a = (float)((color >> 24) & 0xff);
		float r = (float)((color >> 16) & 0xff);
		float g = (float)((color >> 8) & 0xff);
		float b = (float)((color) & 0xff);

		window->DrawList->AddLine(from, to, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thickness);
	}

	void DrawCircle(const ImVec2& position, float radius, uint32_t color, float thickness)
	{
		float step = (float)M_PI * 2.0f / thickness;

		for (float a = 0; a < (M_PI * 2.0f); a += step)
		{
			DrawLine(ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y), ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y), color, 1.5f);
		}
	}

	void Header()
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
	}

	void Init(ImFont* font)
	{
		Header();
		InterfaceInit(font);
		End();
	}

	void End()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	int cLocalIndex = 0;
	void InterfaceInit(ImFont* font)
	{
		Settings::bIsInGame = g_SDK::IsInGame();

		if (!Settings::bIsInGame)
		{
			rRefDef = 0;
			cLocalIndex = 0;
			Settings::iMaxPlayerCount = -1;

			g_SDK::ClearCachedPtr();
			g_SDK::ClearMap();

			return;
		}

		Misc::MiscInit();

		g_SDK::StartTick();

		if (Settings::iMaxPlayerCount == -1)
			Settings::iMaxPlayerCount = g_SDK::GetMaxPlayerCount();

		if (rRefDef == 0)
			rRefDef = g_SDK::GetRefdef();

		if (Settings::g_menuSettings.options["AimLock"].get<bool>())
			Aimbot::ResetLock();

		// Local Player
		g_SDK::characterInfo_t* cLocalCharacterInfo = g_SDK::GetLocalCharacterInfo(&cLocalIndex);
		if (!cLocalCharacterInfo)
			return;

		g_SDK::posInfo_t* cLocalPosInfo = (g_SDK::posInfo_t*)(cLocalCharacterInfo->GetPositionPtr());
		if (!cLocalPosInfo)
			return;

		DWORD dLocalTeamId = cLocalCharacterInfo->GetTeamId();

		if (Settings::g_menuSettings.options["NoRecoil"].get<bool>())
			g_SDK::NoRecoil();

		// Online Players
		for (int i = 0; i < Settings::iMaxPlayerCount; ++i)
		{
			if (cLocalIndex == i)
				continue;

			g_SDK::characterInfo_t* cPlayerInfo = g_SDK::GetCharacterInfo(NULL, i);
			if (!cPlayerInfo && cPlayerInfo->IsValid() != 1)
				continue;

			g_SDK::posInfo_t* cPlayerPosInfo = (g_SDK::posInfo_t*)cPlayerInfo->GetPositionPtr();
			if (!cPlayerPosInfo && cPlayerPosInfo->m_position.IsZero())
				continue;

			if (Settings::g_menuSettings.options["AlliesCheck"].get<bool>())
			{
				DWORD dTeamId = cPlayerInfo->GetTeamId();
				bool bIsFriendly = dTeamId == dLocalTeamId;
				if (bIsFriendly)
					continue;
			}

			g_SDK::UpdateVisibleAddr(i);

			if (Settings::g_menuSettings.options["VisibleCheck"].get<bool>())
			{
				bool bIsVisible = !g_SDK::IsVisible(i);
				if (bIsVisible)
					continue;
			}

			int iPlayerHealth = g_SDK::GetPlayerHealth(i);
			bool bIsAlive = !(cPlayerInfo->GetDeathStatus() == 0 && iPlayerHealth > 0);
			if (bIsAlive)
				continue;

			float fDistance = GetDistance(cLocalPosInfo->m_position, cPlayerPosInfo->m_position) / 50;
			if (fDistance > (int)Settings::g_menuSettings.options["MaxDistance"].get<int64_t>())
				continue;

			g_SDK::UpdateVelMap(i, cLocalPosInfo->m_position);

			if (Settings::g_menuSettings.options["AimLock"].get<bool>())
			{
				int iPlayerStance = cPlayerInfo->GetStance();
				Aimbot::AimbotInit(i, fDistance, cLocalPosInfo->m_position, cPlayerPosInfo->m_position, iPlayerStance);
			}
		}

		g_SDK::UpdateLastVisible();

		if (Settings::g_menuSettings.options["AimLock"].get<bool>())
			Aimbot::AimLockOnTarget();
	};
}