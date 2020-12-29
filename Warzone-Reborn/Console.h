#pragma once
#include "imgui.h"

struct Console
{
	Console();
	~Console();

	char                  InputBuf[256];
	ImVector<char*>       Items;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImGuiTextFilter       Filter;
	bool                  AutoScroll;
	bool                  ScrollToBottom;

	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
};
