#ifdef WITH_IMGUI

#include "Engine/Engine.h"
#include "ImGuiSubsystem.h"
#include "Styling/AppStyle.h"
#include "Styling/SlateTypes.h"

// demo for adding widget to the main menu bar 
namespace ImGuiMenuBar_RootWidget
{
	static void Initialize()
	{
	}

	static void Tick(FImGuiTickContext* Context)
	{
		FImGuiTickScope Scope{ Context };

		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		// the widget slot is never activated so we should not receive window tick callbacks!
		check(Context->bIsTickingMainMenuBar);

		const bool bIsHorizontalMenu = ImGui::GetCurrentWindow()->DC.LayoutType == ImGuiLayoutType_Horizontal;

		// settings menu with icon
		{
			FImGuiImageBindingParams SettingsIcon = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON_BRUSH("CoreStyle", "Icons.Settings"), 16.f * ImGui::GetStyle().FontScaleMain/*ImGui::GetTextLineHeightWithSpacing()*/);
			if (bIsHorizontalMenu)
			{
				FImGui::SubMenu("    ###Settings",
					[]()
					{
						ImGui::MenuItem("Setting#1");
						ImGui::MenuItem("Setting#2");
						ImGui::MenuItem("Setting#3");
					}, SettingsIcon, /*IconOffset=*/ImVec2(0.f, 2.5f));
			}
			else
			{
				FImGui::SubMenu("Settings",
					[]()
					{
						ImGui::MenuItem("Setting#1");
						ImGui::MenuItem("Setting#2");
						ImGui::MenuItem("Setting#3");
					}, SettingsIcon);
			}
		}

		// input text widget for on screen print
		{
			static char TextBuffer[32] = { 0 };
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f);
			if (bIsHorizontalMenu)
			{
				ImGui::SetNextItemWidth(80.f * ImGui::GetStyle().FontScaleMain);
			}
			else
			{
				ImGui::SetNextItemWidth(100.f * ImGui::GetStyle().FontScaleMain);
			}
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.f, 1.f));
			ImGui::PushStyleColor(ImGuiCol_NavCursor, 0); // disable nav highlight
			bool bProcessText = ImGui::InputTextWithHint("##Print", "Print text...", TextBuffer, sizeof(TextBuffer), ImGuiInputTextFlags_EnterReturnsTrue);
			ImGui::PopStyleColor();
			if (bProcessText)
			{
				static const int32 PrintKey = ImGui::GetID("PrintID");
				if (FCStringAnsi::Strlen(TextBuffer) > 0)
				{
					GEngine->AddOnScreenDebugMessage(PrintKey, 2.f, FColor::Green, UTF8_TO_TCHAR(TextBuffer));
				}

				// reset and keep focus
				TextBuffer[0] = 0;
				ImGui::SetKeyboardFocusHere(-1);
			}
			ImGui::PopStyleVar();

			// draw nav highlight manually here
			bool bInputTextHasFocus = ImGui::IsItemActive() || (Context->ImguiContext->NavId == ImGui::GetItemID());
			if (bInputTextHasFocus)
			{
				ImDrawList* DrawList = ImGui::GetWindowDrawList();
				DrawList->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImGui::GetColorU32(ImGuiCol_FrameBgActive), 0.f, 1.f, ImDrawFlags_None);
			}
		}
	}

	FImGuiWidgetRegisterParams Params =
	{
		.InitFunction		 = &Initialize,
		.TickFunction		 = &Tick,
		.WidgetPath			 = "#CustomMenuBar", // this creates a top level entry '#CustomMenuBar' , sorted by name so '#' ensures it's the first item
		.WidgetDescription	 = "",
		.bTickInMenuBar		 = true
	};
	IMGUI_REGISTER_MAIN_MENU_WIDGET(Params);
}

// demo for adding widget to sub menu (similar to ImGuiDemoWidget.cpp)
namespace ImGuiMenuBar_SubMenuWidget
{
	static float Volume = 0.5f;
	static bool bIsMuted = false;

	static void Initialize()
	{
	}

	// don't use as reference, just something hacked together for the demo...
	static bool UnrealSlider(const char* label, float* p_data, const float p_min, const float p_max)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const float w = ImGui::CalcItemWidth();
		const float h = style.GrabMinSize;

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, h + style.FramePadding.y * 2.0f));

		ImGui::ItemSize(frame_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(frame_bb, id, &frame_bb))
			return false;

		const bool hovered = ImGui::ItemHoverable(frame_bb, id, g.LastItemData.ItemFlags);
		{
			// Tabbing or Ctrl+Click on Slider turns it into an input box
			const bool clicked = hovered && ImGui::IsMouseClicked(0, ImGuiInputFlags_None, id);
			const bool make_active = (clicked || g.NavActivateId == id);
			if (make_active && clicked)
				ImGui::SetKeyOwner(ImGuiKey_MouseLeft, id);

			// Store initial value (not used by main lib but available as a convenience but some mods e.g. to revert)
			if (make_active)
				memcpy(&g.ActiveIdValueOnActivation, p_data, ImGui::DataTypeGetInfo(ImGuiDataType_Float)->Size);

			if (make_active)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
		}

		// Slider behavior
		ImRect grab_bb;
		const bool value_changed = ImGui::SliderBehavior(frame_bb, id, ImGuiDataType_Float, p_data, &p_min, &p_max, "", ImGuiSliderFlags_NoInput, &grab_bb);
		if (value_changed)
			ImGui::MarkItemEdited(id);
		const float grab_radius = grab_bb.GetSize().x * 0.5f;
		const ImVec2 grab_center = (grab_bb.Min + grab_bb.Max) * 0.5f;

		ImGui::RenderNavCursor(frame_bb, id);
		{
			ImVec2 frame_bb_min = frame_bb.Min + ImVec2(grab_radius, grab_radius);
			ImVec2 frame_bb_max = frame_bb.Max + ImVec2(-grab_radius, -grab_radius);

			// inactive bar
			if (*p_data < 1.f)
			{
				window->DrawList->AddRectFilled(ImVec2(ImMax(frame_bb_min.x, grab_center.x - grab_radius * 0.5f), frame_bb_min.y), frame_bb_max, ImGui::GetColorU32((g.ActiveId == id || hovered) ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), 2.f);
			}

			// active bar
			if (*p_data > 0.f)
			{
				window->DrawList->AddRectFilled(frame_bb_min, ImVec2(grab_center.x, frame_bb_max.y), ImGui::GetColorU32(ImGuiCol_FrameBgActive) | IM_COL32_A_MASK, 2.f);
			}
		}

		// Render grab
		if (grab_bb.Max.x > grab_bb.Min.x)
			window->DrawList->AddCircleFilled(grab_center, grab_radius, ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab), 32.f);

		return value_changed;
	}

	static void Tick(FImGuiTickContext* Context)
	{
		FImGuiTickScope Scope{ Context };

		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		// the widget slot is never activated so we should not receive window tick callbacks!
		check(Context->bIsTickingMainMenuBar);

		// we can add items
		{
			static const FVolumeControlStyle VolumeStyle = FAppStyle::GetWidgetStyle<FVolumeControlStyle>(TEXT("VolumeControl"));
			const FSlateBrush* VolumeIconBrush = nullptr;
			if (bIsMuted)
			{
				VolumeIconBrush = &VolumeStyle.MutedImage;
			}
			else
			{
				if (Volume < KINDA_SMALL_NUMBER)	VolumeIconBrush = &VolumeStyle.NoVolumeImage;
				else if (Volume < 0.33f)			VolumeIconBrush = &VolumeStyle.LowVolumeImage;
				else if (Volume < 0.66f)			VolumeIconBrush = &VolumeStyle.MidVolumeImage;
				else								VolumeIconBrush = &VolumeStyle.HighVolumeImage;
			}

			FImGuiImageBindingParams VolumeIcon = ImGuiSubsystem->RegisterOneFrameResource(VolumeIconBrush, ImGui::GetTextLineHeightWithSpacing());
			ImGui::ImageWithBg(VolumeIcon.Id, VolumeIcon.Size, VolumeIcon.UV0, VolumeIcon.UV1);
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				bIsMuted = !bIsMuted;
			}
			ImGui::SameLine();

			ImGui::BeginDisabled(bIsMuted);
			UnrealSlider("##Volume", &Volume, 0.f, 1.f);
			ImGui::EndDisabled();
			if (!bIsMuted)
			{
				ImGui::SetItemTooltip("%i", FMath::RoundToInt(Volume * 100.f));
			}
		}

		// we can also add menus
		FImGuiImageBindingParams OptionsIcon = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON_BRUSH("CoreStyle", "Icons.Settings"), ImGui::GetTextLineHeight());
		FImGui::SubMenu("Options",
			[]()
			{
				ImGui::MenuItem("Option#1");
				ImGui::MenuItem("Option#2");
				ImGui::MenuItem("Option#3");
			}, OptionsIcon);
	}

	FImGuiWidgetRegisterParams Params =
	{
		.InitFunction = &Initialize,
		.TickFunction = &Tick,
		.WidgetPath = "ImGui.SubMenu.CustomWidget", // this creates a hierarchy ImGui -> SubMenu -> 'CustomWidget (this is our customization point)'
		.WidgetDescription = "",
		.bTickInMenuBar = true
	};
	IMGUI_REGISTER_MAIN_MENU_WIDGET(Params);
}

#endif //#if WITH_IMGUI
