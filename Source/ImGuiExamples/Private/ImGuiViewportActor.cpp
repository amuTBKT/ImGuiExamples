#include "ImGuiViewportActor.h"

#ifdef WITH_IMGUI
#include "ImGuiSubsystem.h"
#endif //#ifdef WITH_IMGUI

AImGuiViewportActor::AImGuiViewportActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AImGuiViewportActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#ifdef WITH_IMGUI
	if (FImGuiTickContext* TickContext = UImGuiSubsystem::Get()->GetWidgetTickContext(GetWorld()))
	{
		FImGuiTickScope Scope{ TickContext };

		if (ImGui::Begin("ViewportWindow"))
		{
			ImGui::Text("Some text...");
			if (ImGui::Button("Button"))
			{
			}
		}
		ImGui::End();
	}
#endif
}
