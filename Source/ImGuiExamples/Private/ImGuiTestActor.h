#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImGuiTestActor.generated.h"

class UTexture2D;

UCLASS(hideCategories = (Rendering, Replication, Collision, HLOD, Physics, Networking, Input, Actor, Cooking))
class AImGuiTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AImGuiTestActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	/* If disabled the actor creates a new window for showing ImGui widgets */
	UPROPERTY(EditAnywhere, Category = "Imgui")
	bool bAddWidgetToMainWindow = true;

	UPROPERTY(EditAnywhere, Category = "Imgui")
	UTexture2D* Texture = nullptr;

#ifdef WITH_IMGUI
private:
	void TickImGuiWidget(struct FImGuiTickContext* Context);

	FDelegateHandle MainWindowTickHandle;
	TWeakPtr<class SWindow> WidgetWindow;

	float TextureDisplayScale = 1.f;
#endif
};
