#pragma once
#include "imgui.h"
#include "SDK.h"

namespace Interface
{
	extern g_SDK::refdef_t* rRefDef;

	float	GetDistance(const Vector3& lhs, const Vector3& rhs);
	bool	HeadToScreen(Vector3 pos, Vector2* pos_out, int stance);
	void	DrawLine(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness);
	void	DrawCircle(const ImVec2& position, float radius, uint32_t color, float thickness);

	void	Header();
	void	Init(ImFont* font);
	void	End();

	void	InterfaceInit(ImFont* font);
}