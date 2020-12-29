#include "Windows.h"
#include "Main.hpp"
#include "GUI.h"
#include "Settings.h"
#include "Console.h"
#include "Xor.hpp"
#include "SDK.h"

namespace GUI {
	Console console;

	void Log(const char* fmt, ...) {
		console.AddLog(fmt);
	}

	void RenderMenu() {
		if (GetAsyncKeyState(VK_INSERT) & 0x1)
			Settings::bMenu = !Settings::bMenu;

		if (Settings::bMenu) {
			if (ImGui::Begin(xorstr("Warzone Reloaded"), NULL, ImGuiWindowFlags_NoResize)) {
				ImGui::SetWindowSize(ImVec2(900, 425), ImGuiCond_Always);

				ImGui::BeginTabBar(xorstr("tab"));
				if (ImGui::BeginTabItem(xorstr("Main"))) {
					ImGui::Text(xorstr("In-Game: %s"), Settings::bIsInGame ? "Yes" : "No");
					if (Settings::iMaxPlayerCount > 0)
						ImGui::Text(xorstr("Max Players: %d"), Settings::iMaxPlayerCount);

					ImGui::Separator();

					ImGui::Text(xorstr("Idk why this is there.."));
					ImGui::SliderInt(xorstr("##MAXDISTANCE"), (int*)Settings::g_menuSettings.options["MaxDistance"].get<int64_t*>(), 50, 1000, xorstr("Distance: %d"));

					ImGui::Separator();

					ImGui::Text(xorstr("Settings"));
					if (ImGui::Button(xorstr("Save")))
						Settings::g_menuSettings.Save();
					ImGui::SameLine();
					if (ImGui::Button(xorstr("Load")))
						Settings::g_menuSettings.Load();

					ImGui::Separator();

					ImGui::Text(xorstr("Unload"));
					if (ImGui::Button(xorstr("Unload Cheat"))) {
						// TODO
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(xorstr("Aimbot"))) {
					if (ImGui::Checkbox(xorstr("Aim Lock"), Settings::g_menuSettings.options["AimLock"].get<bool*>()))
						Log(Settings::g_menuSettings.options["AimLock"].get<bool>() ? xorstr("[info] Aim Lock enabled !") : xorstr("[info] Aim Lock disabled !"));
					if (ImGui::Checkbox(xorstr("Aim Prediction"), Settings::g_menuSettings.options["AimPrediction"].get<bool*>()))
						Log(Settings::g_menuSettings.options["AimPrediction"].get<bool>() ? xorstr("[info] Aim Prediction enabled !") : xorstr("[info] Aim Prediction disabled !"));
					if (ImGui::Checkbox(xorstr("No Recoil"), Settings::g_menuSettings.options["NoRecoil"].get<bool*>()))
						Log(Settings::g_menuSettings.options["NoRecoil"].get<bool>() ? xorstr("[info] No Recoil enabled !") : xorstr("[info] No Recoil disabled !"));

					ImGui::Separator();

					const double aimSpeedMin = 0., aimSpeedMax = 10.;
					if (ImGui::SliderScalar(xorstr("##AIMSPEED"), ImGuiDataType_Double, Settings::g_menuSettings.options["AimSpeed"].get<double*>(), &aimSpeedMin, &aimSpeedMax, xorstr("Aim Speed: %.3lf")))
						Log(xorstr("[info] Aim Speed value chanted to %.3lf !", Settings::g_menuSettings.options["AimSpeed"].get<double>()));

					if (ImGui::SliderInt(xorstr("##AIMSMOOTH"), (int*)Settings::g_menuSettings.options["AimSmooth"].get<int64_t*>(), 1, 30, xorstr("Aim Smooth: %d")))
						Log(xorstr("[info] Aim Smooth value chanted to %d !", (int)Settings::g_menuSettings.options["AimSmooth"].get<int64_t>()));

					const double bulletSpeedMin = 1., bulletSpeedMax = 5000.;
					if (ImGui::SliderScalar(xorstr("##BULLETSPEED"), ImGuiDataType_Double, Settings::g_menuSettings.options["BulletSpeed"].get<double*>(), &bulletSpeedMin, &bulletSpeedMax, xorstr("Bulled Speed: %.1lf")))
						Log(xorstr("[info] Bullet Speed value chanted to %.1lf !", Settings::g_menuSettings.options["BulletSpeed"].get<double>()));

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(xorstr("ESP"))) {
					if (ImGui::Checkbox(xorstr("Aim Point"), Settings::g_menuSettings.options["AimPoint"].get<bool*>()))
						Log(Settings::g_menuSettings.options["AimPoint"].get<bool>() ? xorstr("[info] Aim Point enabled !") : xorstr("[info] Aim Point disabled !"));
					ImGui::SameLine();
					const double aimPointSizeMin = 1., aimPointSizeMax = 50.;
					if (ImGui::SliderScalar(xorstr("##AIMPOINTSIZE"), ImGuiDataType_Double, Settings::g_menuSettings.options["AimPointSize"].get<double*>(), &aimPointSizeMin, &aimPointSizeMax, xorstr("Aim Point Size: %.0lf")))
						Log(xorstr("[info] Aim Point Size value chanted to %.0lf !", Settings::g_menuSettings.options["AimPointSize"].get<double>()));

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(xorstr("Misc"))) {
					if (ImGui::Checkbox(xorstr("Debug Mode"), Settings::g_menuSettings.options["DebugMode"].get<bool*>()))
						Log(Settings::g_menuSettings.options["DebugMode"].get<bool>() ? xorstr("[info] Debug Mode enabled !") : xorstr("[info] Debug Mode disabled !"));
					if (ImGui::Checkbox(xorstr("Stance Check [WARZONE ONLY]"), Settings::g_menuSettings.options["StanceCheck"].get<bool*>()))
						Log(Settings::g_menuSettings.options["StanceCheck"].get<bool>() ? xorstr("[info] Stance Check enabled !") : xorstr("[info] Stance Check disabled !"));
					if (ImGui::Checkbox(xorstr("Visible Check"), Settings::g_menuSettings.options["VisibleCheck"].get<bool*>()))
						Log(Settings::g_menuSettings.options["VisibleCheck"].get<bool>() ? xorstr("[info] Visible Check enabled !") : xorstr("[info] Visible Check disabled !"));
					if (ImGui::Checkbox(xorstr("Allies Check"), Settings::g_menuSettings.options["AlliesCheck"].get<bool*>()))
						Log(Settings::g_menuSettings.options["AlliesCheck"].get<bool>() ? xorstr("[info] Allies Check enabled !") : xorstr("[info] Allies Check disabled !"));

					const double fovSizeMin = 10., fovSizeMax = 800.;
					if (ImGui::Checkbox(xorstr("FOV"), Settings::g_menuSettings.options["FOV"].get<bool*>()))
						Log(Settings::g_menuSettings.options["FOV"].get<bool>() ? xorstr("[info] FOV enabled !") : xorstr("[info] FOV disabled !"));
					ImGui::SameLine();
					if (ImGui::SliderScalar(xorstr("##FOVSIZE"), ImGuiDataType_Double, Settings::g_menuSettings.options["FOVSize"].get<double*>(), &fovSizeMin, &fovSizeMax, xorstr("FOV Size: %.0lf")))
						Log(xorstr("[info] FOV Size value chanted to %.0lf !", Settings::g_menuSettings.options["FOVSize"].get<double>()));

					const double crosshairSizeMin = 1., crosshairSizeMax = 100.;
					if (ImGui::Checkbox(xorstr("Crosshair"), Settings::g_menuSettings.options["Crosshair"].get<bool*>()))
						Log(Settings::g_menuSettings.options["Crosshair"].get<bool>() ? xorstr("[info] Crosshair enabled !") : xorstr("[info] Crosshair disabled !"));
					ImGui::SameLine();
					if (ImGui::SliderScalar(xorstr("##CROSSHAIRSIZE"), ImGuiDataType_Double, Settings::g_menuSettings.options["CrosshairSize"].get<double*>(), &crosshairSizeMin, &crosshairSizeMax, xorstr("Crosshair Size: %.0lf")))
						Log(xorstr("[info] Crosshair Size value chanted to %.0lf !", Settings::g_menuSettings.options["CrosshairSize"].get<double>()));

					if (ImGui::TreeNode("Color/Picker Test")) {
						static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
						ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_NoOptions;

						ImGui::Text("Color button with Custom Picker Popup:");

						// Generate a default palette. The palette will persist and can be edited.
						static bool saved_palette_init = true;
						static ImVec4 saved_palette[32] = {};
						if (saved_palette_init) {
							for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++) {
								ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
									saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
								saved_palette[n].w = 1.0f; // Alpha
							}
							saved_palette_init = false;
						}

						static ImVec4 backup_color;
						bool open_popup = ImGui::ColorButton("MyColor##3b", color, misc_flags);
						ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
						if (open_popup) {
							ImGui::OpenPopup("mypicker");
							backup_color = color;
						}
						if (ImGui::BeginPopup("mypicker")) {
							ImGui::ColorPicker4("##picker", (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
							ImGui::SameLine();

							ImGui::BeginGroup(); // Lock X position
							ImGui::Text("Current");
							ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
							ImGui::Text("Previous");
							if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
								color = backup_color;
							ImGui::Separator();
							ImGui::Text("Palette");
							for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
							{
								ImGui::PushID(n);
								if ((n % 8) != 0)
									ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

								ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
								if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
									color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

								// Allow user to drop colors into each palette entry. Note that ColorButton() is already a
								// drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
								if (ImGui::BeginDragDropTarget())
								{
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
										memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
									if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
										memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
									ImGui::EndDragDropTarget();
								}

								ImGui::PopID();
							}
							ImGui::EndGroup();
							ImGui::EndPopup();
						}

						ImGui::TreePop();
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem(xorstr("Console"))) {
					// Examples
					/*if (ImGui::Button("Add Debug Text")) { Log("Infinity Ward is GAYYYYYYYYYYY"); }
					ImGui::SameLine();
					if (ImGui::Button("Add Debug Info")) { Log("[info] Infinity Ward is GAYYYYYYYYYYY"); }
					ImGui::SameLine();
					if (ImGui::Button("Add Debug Error")) { Log("[error] something went wrong"); }
					ImGui::SameLine();*/

					// Options menu
					if (ImGui::BeginPopup(xorstr("Options"))) {
						ImGui::Checkbox(xorstr("Auto-scroll"), &console.AutoScroll);
						ImGui::EndPopup();
					}

					if (ImGui::Button(xorstr("Clear")))
						console.ClearLog();

					ImGui::SameLine();
					bool copy_to_clipboard = ImGui::Button(xorstr("Copy"));

					ImGui::SameLine();
					if (ImGui::Button(xorstr("Options")))
						ImGui::OpenPopup(xorstr("Options"));

					ImGui::SameLine();
					console.Filter.Draw(xorstr("Filter"), 180);

					ImGui::Separator();

					// Reserve enough left-over height for 1 separator + 1 input text
					const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
					ImGui::BeginChild(xorstr("ScrollingRegion"), ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					if (ImGui::BeginPopupContextWindow()) {
						if (ImGui::Selectable(xorstr("Clear")))
							console.ClearLog();
						ImGui::EndPopup();
					}

					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
					if (copy_to_clipboard)
						ImGui::LogToClipboard();

					for (int i = 0; i < console.Items.Size; i++) {
						const char* item = console.Items[i];
						if (!console.Filter.PassFilter(item))
							continue;

						// Normally you would store more information in your item than just a string.
						ImVec4 color;
						bool has_color = false;
						if (strstr(item, xorstr("[error]"))) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
						else if (strstr(item, xorstr("[info]"))) { color = ImVec4(1.00f, 0.54f, 0.01f, 0.71f); has_color = true; }
						else if (strncmp(item, xorstr("# "), 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
						if (has_color)
							ImGui::PushStyleColor(ImGuiCol_Text, color);
						ImGui::TextUnformatted(item);
						if (has_color)
							ImGui::PopStyleColor();
					}

					if (copy_to_clipboard)
						ImGui::LogFinish();

					if (console.ScrollToBottom || (console.AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
						ImGui::SetScrollHereY(1.0f);
					console.ScrollToBottom = false;

					ImGui::PopStyleVar();
					ImGui::EndChild();

					// Auto-focus on window apparition
					ImGui::SetItemDefaultFocus();

					ImGui::EndTabItem();
				}

				ImGui::EndTabBar();
			}
			ImGui::End();
		}
	}
}