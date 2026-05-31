#include "ImGuiWindowActor.h"

#ifdef WITH_IMGUI
#include "ImGuiSubsystem.h"
#include "Widgets/SWindow.h"
#endif

AImGuiWindowActor::AImGuiWindowActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AImGuiWindowActor::BeginPlay()
{
	Super::BeginPlay();

#ifdef WITH_IMGUI
	FOnTickImGuiWidgetDelegate TickDelegate;
	TickDelegate.BindUObject(this, &AImGuiWindowActor::TickImGuiWidget);

	UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();
	if (bAddWidgetToMainMenu)
	{
		ImGuiSubsystem->RegisterMainMenuWidget(
			GetWorld(), "Demo.Actor", "Actor properties widget", IMGUI_STYLE_ICON_BRUSH("CoreStyle", "Icons.Info"), TickDelegate, false);
	}
	else
	{
		WidgetWindow = ImGuiSubsystem->CreateWidget("ActorProperties", FVector2f{ 300.f, 300.f }, TickDelegate);
	}
#endif
}

void AImGuiWindowActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#ifdef WITH_IMGUI
	UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();
	ImGuiSubsystem->UnregisterMainMenuWidget(GetWorld(), "Demo.Actor");
	
	if (TSharedPtr<SWindow> Window = WidgetWindow.Pin())
	{
		Window->RequestDestroyWindow();
	}
#endif

	Super::EndPlay(EndPlayReason);
}

#ifdef WITH_IMGUI
void AImGuiWindowActor::TickImGuiWidget(FImGuiTickContext* Context)
{
	FImGuiTickScope Scope{ Context };
		
	UImGuiSubsystem* ImGuiSubsystem = UImGuiSubsystem::Get();

	ImGui::Text("Transform");
	ImGui::Indent();
	{
		float ActorLocation[] = { GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z };
		if (ImGui::InputFloat3("Location", &ActorLocation[0]))
		{
			SetActorLocation(FVector(ActorLocation[0], ActorLocation[1], ActorLocation[2]));
		}

		float ActorScale[] = { GetActorScale3D().X, GetActorScale3D().Y, GetActorScale3D().Z };
		if (ImGui::SliderFloat3("Scale", &ActorScale[0], 0.1f, 16.f))
		{
			SetActorScale3D(FVector(ActorScale[0], ActorScale[1], ActorScale[2]));
		}
	}
	ImGui::Unindent();
		
	ImGui::NewLine();

	ImGui::BeginDisabled(Texture == nullptr);
	ImGui::SliderFloat("Texture Size", &TextureDisplayScale, 0.1f, 8.f);
	ImGui::EndDisabled();
	if (Texture)
	{
		const auto BindingParams = ImGuiSubsystem->RegisterOneFrameResource(Texture);
		ImGui::Image(BindingParams.Id, BindingParams.Size * TextureDisplayScale, BindingParams.UV0, BindingParams.UV1);
	}
}
#endif
