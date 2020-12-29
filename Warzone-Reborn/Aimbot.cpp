#include "Aimbot.h"
#include "Interface.h"
#include "Settings.h"
#include "GUI.h"
#include "SDK.h"

namespace Aimbot
{
	Aim aim;

	Aim::Aim()
	{
		fInitCenterDist = 10000.0f;
		fMinCenterDist = fInitCenterDist;
	}

	Aim::~Aim()
	{
		fInitCenterDist = 0.0f;
		fMinCenterDist = 0.0f;
	}

	void ResetLock()
	{
		aim.fMinCenterDist = aim.fInitCenterDist;
	}

	bool IsKeyDown(int vk_key)
	{
		return ((GetAsyncKeyState(vk_key) & 0x8000) ? 1 : 0);
	}

	void GetTargetInFOV(Vector2 pTargetPos)
	{
		const float x = pTargetPos.x - ((float)(Interface::rRefDef->Width) / 2);
		const float y = pTargetPos.y - ((float)(Interface::rRefDef->Height) / 2);
		float fCenterDist = sqrt(pow(y, 2) + pow(x, 2));
		if (fCenterDist < aim.fMinCenterDist && fCenterDist <= (float)Settings::g_menuSettings.options["FOVSize"].get<double>())
		{
			aim.fMinCenterDist = fCenterDist;
			aim.pLockPosition = pTargetPos;
		}
	}

	void AimLockOnTarget()
	{
		if (IsKeyDown(VK_RBUTTON))
		{
			if (aim.fMinCenterDist != aim.fInitCenterDist)
			{
				float target_x = 0;
				float target_y = 0;
				int center_x = Interface::rRefDef->Width / 2;
				int center_y = Interface::rRefDef->Height / 2;

				if (aim.pLockPosition.x != 0)
				{
					if (aim.pLockPosition.x > center_x)
					{
						target_x = -(center_x - aim.pLockPosition.x);
						target_x *= (float)Settings::g_menuSettings.options["AimSpeed"].get<double>();
						if (target_x + center_x > center_x * 2)
							target_x = 0;
					}
					if (aim.pLockPosition.x < center_x)
					{
						target_x = aim.pLockPosition.x - center_x;
						target_x *= (float)Settings::g_menuSettings.options["AimSpeed"].get<double>();
						if (target_x + center_x < 0)
							target_x = 0;
					}
				}
				if (aim.pLockPosition.y != 0)
				{
					if (aim.pLockPosition.y > center_y)
					{
						target_y = -(center_y - aim.pLockPosition.y);
						target_y *= (float)Settings::g_menuSettings.options["AimSpeed"].get<double>();
						if (target_y + center_y > center_y * 2)
							target_y = 0;
					}
					if (aim.pLockPosition.y < center_y)
					{
						target_y = aim.pLockPosition.y - center_y;
						target_y *= (float)Settings::g_menuSettings.options["AimSpeed"].get<double>();
						if (target_y + center_y < 0)
							target_y = 0;
					}
				}

				target_x /= (int)Settings::g_menuSettings.options["AimSmooth"].get<int64_t>();
				target_y /= (int)Settings::g_menuSettings.options["AimSmooth"].get<int64_t>();

				if (target_x != 0 && target_y != 0)
					mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(target_x), static_cast<DWORD>(target_y), NULL, NULL);
			}
		}
	}

	void AimbotInit(int playerId, float Distance, Vector3 LocalPlayerPos, Vector3 PlayerPos, int Stance)
	{
		Vector3 current_bone;
		Vector2 bone_screen_pos;

		if (Settings::g_menuSettings.options["AimPrediction"].get<bool>() && Distance > 4)
		{
			current_bone = g_SDK::PredictionSolver(LocalPlayerPos, PlayerPos, playerId, (float)Settings::g_menuSettings.options["BulletSpeed"].get<double>());
		}
		else
		{
			current_bone = PlayerPos;
		}

		if (!Interface::HeadToScreen(current_bone, &bone_screen_pos, Stance))
			return;

		//TODO: Add this to ESP.cpp
		if (Settings::g_menuSettings.options["AimPoint"].get<bool>())
			Interface::DrawCircle(ImVec2(bone_screen_pos.x, bone_screen_pos.y), (float)Settings::g_menuSettings.options["AimPointSize"].get<double>(), 0xFFFFFF00, 100.f);

		if (Settings::g_menuSettings.options["StanceCheck"].get<bool>())
			if (Stance == g_SDK::STANCE::KNOCK)
				return;

		GetTargetInFOV(bone_screen_pos);
	}
}