#ifdef WITH_IMGUI

#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "ImGuiSubsystem.h"
#include "Misc/ConfigCacheIni.h"

THIRD_PARTY_INCLUDES_START
#include <nanosvg.h>
#include <nanosvgrast.h>
THIRD_PARTY_INCLUDES_END

namespace ImGuiDemoWidget
{
	// persistent state for the widget
	static bool bShowUnrealDemo = true;
	static bool bShowImGuiDemo  = false;
	static bool bShowImPlotDemo = false;

	static int64 GetPackedWindowShowFlags()
	{
		int64 PackedWindowShowFlags = 0;
		if (bShowUnrealDemo) PackedWindowShowFlags |= 0x1;
		if (bShowImGuiDemo)  PackedWindowShowFlags |= 0x2;
		if (bShowImPlotDemo) PackedWindowShowFlags |= 0x4;
		return PackedWindowShowFlags;
	}
	static void LoadWidgetSettings()
	{
		FConfigFile* WidgetSettings = UImGuiSubsystem::Get()->GetSaveDataConfigFile();
		if (WidgetSettings)
		{
			int64 PackedWindowShowFlags;
			if (WidgetSettings->GetInt64(TEXT("ImGuiDemoWidget"), TEXT("PackedWindowShowFlags"), PackedWindowShowFlags))
			{
				bShowUnrealDemo = (PackedWindowShowFlags & 0x1) > 0;
				bShowImGuiDemo  = (PackedWindowShowFlags & 0x2) > 0;
				bShowImPlotDemo = (PackedWindowShowFlags & 0x4) > 0;
			}
		}
	}
	static void SaveWidgetSettings()
	{
		FConfigFile* WidgetSettings = UImGuiSubsystem::Get()->GetSaveDataConfigFile();
		if (WidgetSettings)
		{
			WidgetSettings->SetInt64(TEXT("ImGuiDemoWidget"), TEXT("PackedWindowShowFlags"), GetPackedWindowShowFlags());
			UImGuiSubsystem::Get()->SaveConfigToDisk();
		}
	}

	static void Initialize()
	{
		LoadWidgetSettings();
	}

	static void TickMenuItem()
	{
		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		auto MenuIcon = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON_BRUSH("CoreStyle", "Icons.Info"), ImGui::GetTextLineHeight());
		FImGui::SubMenu("Demo", MenuIcon, MenuIcon,
			[&]()
			{
				if (ImGui::MenuItem("Unreal", nullptr, bShowUnrealDemo))
				{
					bShowUnrealDemo = !bShowUnrealDemo;
				}
				ImGui::SetItemTooltip("Show Unreal demo window");

				if (ImGui::MenuItem("ImGui", nullptr, bShowImGuiDemo))
				{
					bShowImGuiDemo = !bShowImGuiDemo;
				}
				ImGui::SetItemTooltip("Show ImGui demo window");

				if (ImGui::MenuItem("ImPlot", nullptr, bShowImPlotDemo))
				{
					bShowImPlotDemo = !bShowImPlotDemo;
				}
				ImGui::SetItemTooltip("Show ImPlot demo window");
			});

		// optionally update widget active state to disable window tick
		bool* bWidgetActiveState = ImGuiSubsystem->GetMainMenuWidgetActiveState(/*World=*/nullptr, "ImGui.Demo");
		if (bWidgetActiveState)
		{
			*bWidgetActiveState = bShowUnrealDemo || bShowImGuiDemo || bShowImPlotDemo;
		}

	}
	static void TickWindows()
	{
		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		if (bShowUnrealDemo)
		{
			if (ImGui::Begin("Unreal Demo", &bShowUnrealDemo))
			{
				static int32 ClickCount = 0;
				if (ImGui::Button("Click"))
				{
					++ClickCount;
				}
				ImGui::SameLine();
				ImGui::Text("Count: %i", ClickCount);

				ImGui::NewLine();
				ImGui::SeparatorText("Font");
				{
					// how to use custom fonts with ImGui

					static ImFont* CustomFont = ImGuiSubsystem->GetSharedFontAtlas()->AddFontFromFileTTF(
						TCHAR_TO_ANSI(*(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Italic.ttf"))), 20.f);
					if (CustomFont)
					{
						ImGui::PushFont(CustomFont, 20.f);
						ImGui::Text("Text using custom font");
						ImGui::PopFont();
					}
					ImGui::Text("Text using default font");
				}

				ImGui::NewLine();
				ImGui::SeparatorText("Icon");
				{
					// how to use slate icons/brushes with ImGui

					auto UnrealLogo = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON_BRUSH("CoreStyle", "UnrealCircle.Thick"), 32.f * ImGui::GetStyle().FontScaleMain);
					ImGui::Image(UnrealLogo.Id, UnrealLogo.Size, UnrealLogo.UV0, UnrealLogo.UV1);
					ImGui::SameLine(); ImGui::SetCursorPosY(ImGui::GetCursorPosY() + UnrealLogo.Size.y * 0.25f);
					ImGui::Text("Slate icon");
				}

				ImGui::NewLine();
				ImGui::SeparatorText("SVG");
				{
					// how to use custom SVGs with ImGui

					static float SVGSize = 32.f;
					static char SVGFilepath[512] = { 0 };
					UE_CALL_ONCE([]
						{
							const FAnsiString DefaulthPath = TCHAR_TO_ANSI(*(FPaths::ConvertRelativePathToFull(FPaths::EngineContentDir()) / TEXT("Slate/Starship/Common/unreal-circle-thick.svg")));
							FMemory::Memcpy(SVGFilepath, *DefaulthPath, DefaulthPath.Len() + 1);
						});
					static ImFontAtlasRectId SVGAtlasRectId = ImFontAtlasRectId_Invalid;

					ImGui::InputTextWithHint("##SVGPath", "SVG file path...", SVGFilepath, sizeof(SVGFilepath));
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
					ImGui::DragFloat("##SVGSize", &SVGSize, 1.f, 4.f, 128.f);
					ImGui::SameLine();
					if (ImGui::Button("Rasterize SVG"))
					{
						ImFontAtlas* FontAtlas = ImGuiSubsystem->GetSharedFontAtlas();
						if (SVGAtlasRectId != ImFontAtlasRectId_Invalid)
						{
							// this can be skipped if size stays the same (no need to re-register the rect)
							FontAtlas->RemoveCustomRect(SVGAtlasRectId);
							SVGAtlasRectId = ImFontAtlasRectId_Invalid;
						}

						SVGAtlasRectId = FontAtlas->AddCustomRect(SVGSize, SVGSize);
						if (SVGAtlasRectId != ImFontAtlasRectId_Invalid)
						{
							NSVGimage* Image = nsvgParseFromFile(SVGFilepath, "px", /*DPI=*/96.f);
							if (Image)
							{
								ImFontAtlasRect SVGRect = {};
								if (FontAtlas->GetCustomRect(SVGAtlasRectId, &SVGRect))
								{
									NSVGrasterizer* Rasterizer = nsvgCreateRasterizer();

									const float SVGScaleX = (float)SVGRect.w / Image->width;
									const float SVGScaleY = (float)SVGRect.h / Image->height;

									const int32 Stride = FontAtlas->TexData->Width * FontAtlas->TexData->BytesPerPixel;
									nsvgRasterizeFull(Rasterizer, Image, 0, 0, SVGScaleX, SVGScaleY, (uint8*)FontAtlas->TexData->GetPixelsAt(SVGRect.x, SVGRect.y), SVGRect.w, SVGRect.h, Stride);

									nsvgDeleteRasterizer(Rasterizer);

									ImFontAtlasTextureBlockQueueUpload(FontAtlas, FontAtlas->TexData, SVGRect.x, SVGRect.y, SVGRect.w, SVGRect.h);
								}
								nsvgDelete(Image);
							}
						}
					}
					ImFontAtlasRect SVGRect = {};
					if (ImGuiSubsystem->GetSharedFontAtlas()->GetCustomRect(SVGAtlasRectId, &SVGRect))
					{
						ImGui::Image(ImGuiSubsystem->GetSharedFontTextureID(), ImVec2(SVGRect.w, SVGRect.h), SVGRect.uv0, SVGRect.uv1);
					}
					else
					{
						ImGui::Image(UImGuiSubsystem::GetMissingImageTextureID(), ImVec2(SVGSize, SVGSize));
					}
				}
			}
			ImGui::End();
		}

		if (bShowImGuiDemo)
		{
			ImGui::ShowDemoWindow(&bShowImGuiDemo);
		}

		if (bShowImPlotDemo)
		{
			ImPlot::ShowDemoWindow(&bShowImPlotDemo);
		}
	}

	static void Tick(FImGuiTickContext* Context)
	{
		FImGuiTickScope Scope{ Context };

		const int64 PrevWindowState = GetPackedWindowShowFlags();
		ON_SCOPE_EXIT
		{
			if (PrevWindowState != GetPackedWindowShowFlags())
			{
				SaveWidgetSettings();
			}
		};

		if (Context->bIsTickingMainMenuBar)
		{
			// since the widget uses `bTickInMenuBar` we can customize the menu item here
			TickMenuItem();
		}
		else
		{
			// `bTickInMenuBar` also allows widget to tick windows
			TickWindows();
		}
	}

	FImGuiWidgetRegisterParams Params =
	{
		.InitFunction		 = &Initialize,
		.TickFunction		 = &Tick,
		.WidgetIcon			 = IMGUI_STYLE_ICON("CoreStyle", "Icons.Info"),
		.WidgetPath			 = "ImGui.Demo",
		.WidgetDescription	 = "Demo Widget",		
		.bSkipWindowCreation = true,	// allows widget to create and manage multiple ImGui windows		
		.bTickInMenuBar		 = true		// needed to customize menu bar item
	};
	IMGUI_REGISTER_MAIN_MENU_WIDGET(Params);
}

#endif //#if WITH_IMGUI
