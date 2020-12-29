#pragma once

#define QWORD unsigned __int64
#define Exit() 0

#define D3DX_PI ((FLOAT) 3.141592654f)

#define _USE_MATH_DEFINES // for C++
#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <thread>
#include <fstream>
#include <iostream>
#include <sstream>
#include <mutex>
#include <random>
#include <map>
#include <cstdint>
#include <dxgi1_4.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "json.hpp"

#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")

#include <include/MinHook.h>
#pragma comment(lib, "minhook.lib")