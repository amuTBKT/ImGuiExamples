#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ImGuiViewportActor.generated.h"

UCLASS(hideCategories = (Rendering, Replication, Collision, HLOD, Physics, Networking, Input, Actor, Cooking))
class AImGuiViewportActor : public AActor
{
	GENERATED_BODY()

public:
	AImGuiViewportActor();

protected:
	virtual void Tick(float DeltaTime) override;
};
