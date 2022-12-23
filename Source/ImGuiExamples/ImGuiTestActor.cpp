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

	FImGuiRuntimeModule& ImGuiRuntimeModule = FModuleManager::GetModuleChecked<FImGuiRuntimeModule>("ImGuiRuntime");

	if (ImGui::Begin("TestActor", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button("Scale"))
		{
			SetActorScale3D(GetActorScale3D() * 1.2);
		}
		
		if (TestImage)
		{
			const auto BindingParams = ImGuiRuntimeModule.RegisterOneFrameResource(TestImage);

			// use the default size
			ImGui::Image(BindingParams.Id, BindingParams.Size, BindingParams.UV0, BindingParams.UV1);

			// override size
			ImGui::Image(BindingParams.Id, ImVec2{ 128.f, 128.f }, BindingParams.UV0, BindingParams.UV1);
		}
	}
	ImGui::End();
}