#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImGuiTestActor.generated.h"

struct ImGuiContext;
class SWindow;

UCLASS(hideCategories = (Rendering, Replication, Collision, HLOD, Physics, Networking, Input, Actor, Cooking))
class IMGUIEXAMPLES_API AImGuiTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AImGuiTestActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void TickImGuiWidget(struct ImGuiContext* Context);

public:
	/* If disabled the actor creates a new window for showing ImGui widgets */
	UPROPERTY(EditAnywhere, Category = "Imgui")
	bool AddWidgetToMainWindow = true;

	UPROPERTY(EditAnywhere, Category = "Imgui")
	UTexture2D* TestImage = nullptr;

private:
	FDelegateHandle MainWindowTickHandle;
	TSharedPtr<SWindow> WidgetWindow;
};
