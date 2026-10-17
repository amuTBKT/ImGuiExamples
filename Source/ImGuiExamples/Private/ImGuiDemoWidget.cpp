#ifdef WITH_IMGUI

#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "ImGuiSubsystem.h"
#include "Misc/ConfigCacheIni.h"

THIRD_PARTY_INCLUDES_START
#include <nanosvg.h>
#include <nanosvgrast.h>
THIRD_PARTY_INCLUDES_END

#include "Styling/SlateTypes.h"
#include "Styling/StyleColors.h"

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

	static void TickMenuItem(FImGuiTickContext* Context)
	{
		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		auto MenuIcon = ImGuiSubsystem->RegisterOneFrameResource(IMGUI_STYLE_ICON_BRUSH("CoreStyle", "Icons.Info"), ImGui::GetTextLineHeight());
		FImGui::SubMenu(Context, "Demo",
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
			}, MenuIcon);

		// optionally update widget active state to disable window tick
		*Context->MainMenuBar_CurrentItemEnabledState = bShowUnrealDemo || bShowImGuiDemo || bShowImPlotDemo;
	}
	static void TickWindows(FImGuiTickContext* TickContext)
	{
		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

		if (bShowUnrealDemo)
		{
			if (ImGui::Begin("Unreal Demo", &bShowUnrealDemo))
			{
				static const FSpinBoxStyle& SpinBoxStyle = FAppStyle::Get().GetWidgetStyle<FSpinBoxStyle>(IMGUI_FNAME("SpinBox"));
				static const FButtonStyle& PrimaryButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>(IMGUI_FNAME("PrimaryButton"));
				static const FButtonStyle& SecondaryButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>(IMGUI_FNAME("SecondaryButton"));
				static const FComboButtonStyle& DarkComboButtonStyle = FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>(IMGUI_FNAME("ComboButton"));
				static const FComboButtonStyle& LightComboButtonStyle = []()
					{
						auto Style = FAppStyle::Get().GetWidgetStyle<FComboButtonStyle>(IMGUI_FNAME("SimpleComboButton"));
						Style.ButtonStyle = SecondaryButtonStyle;
						Style.ButtonStyle.NormalPadding = FMargin(8.f, 1.f, 8.f, 1.f);
						Style.ButtonStyle.PressedPadding = FMargin(8.f, 2.f, 8.f, 0.f);
						Style.ButtonStyle.Normal.OutlineSettings.Color = FSlateColor(EStyleColor::White25);
						Style.ButtonStyle.Hovered.OutlineSettings.Color = FSlateColor(EStyleColor::Hover);
						Style.ButtonStyle.Pressed.OutlineSettings.Color = FSlateColor(EStyleColor::Hover);
						return Style;
					}();

				static int32 ClickCount = 0;
				if (FImGui::Button(TickContext, "Button##Primary", PrimaryButtonStyle))
				{
					++ClickCount;
				}
				ImGui::SameLine();
				FImGui::TextBlockf(TickContext, "Count: %i", ClickCount);

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
					FImGui::Image(UnrealLogo);
					ImGui::SameLine();
					FImGui::TextBlock(TickContext, "Slate icon");
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

					FImGui::EditableTextBox("##SVGPath", "SVG file path...", SVGFilepath, sizeof(SVGFilepath));

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
					FImGui::SpinBox(TickContext, "##SVGSize", SVGSize, SpinBoxStyle, 1.f, 4.f, 128.f);

					ImGui::SameLine();

					if (FImGui::Button("Rasterize SVG", SecondaryButtonStyle, ImVec4(4.f, 5.f, 4.f, 5.f)))
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
						ImGui::Image(ImTextureID_Invalid, ImVec2(SVGSize, SVGSize));
					}
				}

				ImGui::NewLine();
				ImGui::SeparatorText("Combo");
				{
					/** different style combo widget
					FImGui::PushComboListViewDropdownStyle/FImGui::PopComboListViewDropdownStyle to apply popup styling
					FImGui::BeginComboListViewDropdownBody/FImGui::EndComboListViewDropdownBody to apply selectable styling
					**/

					static const char* PreviewText = "None";
					static const char* Items[] = { "Item A", "Item B", "Item C", "Item D", "Item E", "Item F" };

					// helper function to draw selectable spanning entire row (with padded text)
					auto ComboListSelectable = [](bool bSelected, const char* Text)
						{
							float CursorPosX = ImGui::GetCursorPosX();
							bSelected = ImGui::Selectable("", bSelected, ImGuiSelectableFlags_SpanAllColumns);

							ImGui::SameLine(); ImGui::SetCursorPosX(CursorPosX + 8.f);

							ImGui::TextUnformatted(Text);
							return bSelected;
						};
					auto ComboListSelectablef = [](bool bSelected, const char* Fmt, ...)
						{
							float CursorPosX = ImGui::GetCursorPosX();
							bSelected = ImGui::Selectable("", bSelected, ImGuiSelectableFlags_SpanAllColumns);

							ImGui::SameLine(); ImGui::SetCursorPosX(CursorPosX + 8.f);

							va_list args;
							va_start(args, Fmt);
							ImGui::TextV(Fmt, args);
							va_end(args);
							return bSelected;
						};

					ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.f, 0.5f));

					ImGui::SetNextItemWidth(120.f * ImGui::GetStyle().FontScaleMain);
					FImGui::PushComboListViewDropdownStyle(TickContext, /*bDarkTheme*/true);
					if (FImGui::BeginCombo(TickContext, "##DarkCombo", PreviewText, DarkComboButtonStyle))
					{
						FImGui::BeginComboListViewDropdownBody(TickContext);
						for (auto Item : Items)
						{
							FImGuiNamedScope SelectableScope{ Item };
							if (ComboListSelectable(PreviewText == Item, Item))
							{
								PreviewText = Item;
							}
						}
						FImGui::EndComboListViewDropdownBody(TickContext);

						FImGui::EndCombo();
					}
					FImGui::PopComboListViewDropdownStyle(TickContext);

					ImGui::SameLine();
					ImGui::Dummy(ImVec2(32.f, 0.f));
					ImGui::SameLine();

					ImGui::SetNextItemWidth(120.f * ImGui::GetStyle().FontScaleMain);
					FImGui::PushComboListViewDropdownStyle(TickContext, /*bDarkTheme*/false);
					if (FImGui::BeginCombo(TickContext, "##LightCombo", PreviewText, LightComboButtonStyle))
					{
						FImGui::BeginComboListViewDropdownBody(TickContext);
						for (auto Item : Items)
						{
							FImGuiNamedScope SelectableScope{ Item };
							if(ComboListSelectablef(PreviewText == Item, "-- %s --", Item))
							{
								PreviewText = Item;
							}
						}
						FImGui::EndComboListViewDropdownBody(TickContext);

						FImGui::EndCombo();
					}
					FImGui::PopComboListViewDropdownStyle(TickContext);

					ImGui::PopStyleVar();
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

		if (Context->MainMenuBar_bIsTicking)
		{
			// since the widget uses `bTickInMenuBar` we can customize the menu item here
			TickMenuItem(Context);
		}
		else
		{
			// `bTickInMenuBar` also allows widget to tick windows
			TickWindows(Context);
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
