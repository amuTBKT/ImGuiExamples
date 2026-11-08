#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

#ifdef WITH_IMGUI
#include "ImGuiSubsystem.h"
#endif

class FImGuiExamplesGameModule : public IModuleInterface
{
public:
	virtual bool IsGameModule() const override { return true; }

	virtual void StartupModule() override
	{
#ifdef WITH_IMGUI
		IMGUI_SETUP_DEFAULT_ALLOCATOR();

		auto CustomizeImGuiFonts = [](UImGuiSubsystem* ImGuiSubsystem)
			{
				ImFontAtlas* FontAtlas = ImGuiSubsystem->GetSharedFontAtlas();

				// optionally clear fonts if you would like to override primary font
				// FontAtlas->ClearFonts();

				// register primary font if needed
				if (FontAtlas->Fonts.Size == 0)
				{
					ImFont* PrimaryFont = FontAtlas->AddFontFromFileTTF(TCHAR_TO_ANSI(*(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"))), 15.f);
					check(PrimaryFont);
				}

				ImFontConfig LocalizedFontConfig{};
				LocalizedFontConfig.MergeMode = true;
				std::memcpy(LocalizedFontConfig.Name, "DroidSansFallback", strlen("DroidSansFallback"));
				ImFont* LocalizedFont = FontAtlas->AddFontFromFileTTF(TCHAR_TO_ANSI(*(FPaths::EngineContentDir() / TEXT("Slate/Fonts/DroidSansFallback.ttf"))), 15.f, &LocalizedFontConfig);
				check(LocalizedFont);

				ImGuiSubsystem->CommitSharedFontAtlasChanges();
			};

		if (UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get())
		{
			CustomizeImGuiFonts(ImGuiSubsystem);
		}
		else
		{
			UImGuiSubsystem::OnSubsystemInitialized.AddLambda(
				[&](UImGuiSubsystem* ImGuiSubsystem)
				{
					CustomizeImGuiFonts(ImGuiSubsystem);
				});
		}
#endif
	}

	virtual void ShutdownModule() override
	{

	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FImGuiExamplesGameModule, ImGuiExamples, "ImGuiExamples" );
