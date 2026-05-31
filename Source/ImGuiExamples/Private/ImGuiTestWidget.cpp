#ifdef WITH_IMGUI

#include "Misc/Paths.h"
#include "ImGuiSubsystem.h"

THIRD_PARTY_INCLUDES_START
#include <nanosvg.h>
#include <nanosvgrast.h>
THIRD_PARTY_INCLUDES_END

namespace ImGuiTestWidget
{
	static void Initialize()
	{
	}

	static void Tick(FImGuiTickContext* Context)
	{
		FImGuiTickScope Scope{ Context };

		UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();
			
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

	FImGuiWidgetRegisterParams Params =
	{
		.InitFunction		= &Initialize,
		.TickFunction		= &Tick,
		.WidgetIcon			= IMGUI_STYLE_ICON("CoreStyle", "Icons.Info"),
		.WidgetPath			= "Demo.Test",
		.WidgetDescription	= "Test Widget"
	};
	IMGUI_REGISTER_MAIN_MENU_WIDGET(Params);
}

#endif //#if WITH_IMGUI
