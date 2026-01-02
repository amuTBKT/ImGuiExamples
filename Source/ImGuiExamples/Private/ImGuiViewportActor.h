#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImGuiViewportActor.generated.h"

class UTexture2D;

UCLASS(hideCategories = (Rendering, Replication, Collision, HLOD, Physics, Networking, Input, Actor, Cooking))
class AImGuiViewportActor : public AActor
{
	GENERATED_BODY()

public:
	AImGuiViewportActor();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

public:
#ifdef WITH_IMGUI
	TSharedPtr<class SImGuiScopedWidget> ImGuiWidget;
#endif
};
