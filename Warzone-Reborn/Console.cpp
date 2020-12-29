#include "stdio.h"
#include "Windows.h"
#include "Console.h"
#include "Settings.h"

Console::Console()
{
	ClearLog();
	memset(InputBuf, 0, sizeof(InputBuf));
	HistoryPos = -1;

	AutoScroll = true;
	ScrollToBottom = false;
}

Console::~Console()
{
	ClearLog();
	for (int i = 0; i < History.Size; i++)
		free(History[i]);
}

void Console::ClearLog()
{
	for (int i = 0; i < Items.Size; i++)
		free(Items[i]);
	Items.clear();
}

void Console::AddLog(const char* fmt, ...) IM_FMTARGS(2)
{
	if (!Settings::g_menuSettings.options["DebugMode"].get<bool>())
		return;

	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	Items.push_back(_strdup(buf));
}