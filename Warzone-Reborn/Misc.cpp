#include "Misc.h"
#include "Main.hpp"
#include "Interface.h"
#include "Settings.h"

namespace Misc {
	void DrawCrosshair()
	{
		ImVec2 center = ImVec2((float)Interface::rRefDef->Width / 2, (float)Interface::rRefDef->Height / 2);

		Interface::DrawLine(ImVec2(center.x, center.y - (float)Settings::g_menuSettings.options["CrosshairSize"].get<double>()),
			ImVec2(center.x, center.y + (float)Settings::g_menuSettings.options["CrosshairSize"].get<double>()), 0xFFFFFFFF, 1.5f);

		Interface::DrawLine(ImVec2(center.x - (float)Settings::g_menuSettings.options["CrosshairSize"].get<double>(), center.y),
			ImVec2(center.x + (float)Settings::g_menuSettings.options["CrosshairSize"].get<double>(), center.y), 0xFFFFFFFF, 1.5f);
	}

	void DrawFOV()
	{
		ImVec2 center = ImVec2((float)Interface::rRefDef->Width / 2, (float)Interface::rRefDef->Height / 2);
		Interface::DrawCircle(center, (float)Settings::g_menuSettings.options["FOVSize"].get<double>(), 0xFFFFFFFF, 100.0f);
	}

	void MiscInit()
	{
		if (Settings::g_menuSettings.options["Crosshair"].get<bool>())
			DrawCrosshair();

		if (Settings::g_menuSettings.options["FOV"].get<bool>())
			DrawFOV();
	}
}