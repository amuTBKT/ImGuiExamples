#include "ImGuiTestActor.h"
#include "ImGuiRuntimeModule.h"
#include "Widgets/SWindow.h"

AImGuiTestActor::AImGuiTestActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AImGuiTestActor::BeginPlay()
{
	Super::BeginPlay();
	
	FOnTickImGuiWidgetDelegate TickDelegate;
	TickDelegate.BindUObject(this, &AImGuiTestActor::TickImGuiWidget);

	FImGuiRuntimeModule& ImGuiRuntimeModule = FModuleManager::GetModuleChecked<FImGuiRuntimeModule>("ImGuiRuntime");
	if (AddWidgetToMainWindow)
	{
		MainWindowTickHandle = ImGuiRuntimeModule.GetMainWindowTickDelegate().Add(TickDelegate);
	}
	else
	{
		WidgetWindow = ImGuiRuntimeModule.CreateWindow("TestWindow", { 300., 300. }, TickDelegate);
	}
}

void AImGuiTestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (MainWindowTickHandle.IsValid())
	{
		FImGuiRuntimeModule& ImGuiRuntimeModule = FModuleManager::GetModuleChecked<FImGuiRuntimeModule>("ImGuiRuntime");
		ImGuiRuntimeModule.GetMainWindowTickDelegate().Remove(MainWindowTickHandle);
	}
	if (WidgetWindow)
	{
		WidgetWindow->RequestDestroyWindow();
	}

	Super::EndPlay(EndPlayReason);
}

void AImGuiTestActor::TickImGuiWidget(ImGuiContext* Context)
{
	FImGuiTickScope Scope{ Context };

	if (ImGui::Begin("TestActor", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button("Scale"))
		{
			SetActorScale3D(GetActorScale3D() * 1.2);
		}
		
		ImGui::Button("Drag");
		if (ImGui::IsItemActive())
		{
			ImGuiIO& io = ImGui::GetIO();
			ImGui::GetForegroundDrawList()->AddLine(io.MouseClickedPos[0], io.MousePos, ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
		}
	}
	ImGui::End();
}