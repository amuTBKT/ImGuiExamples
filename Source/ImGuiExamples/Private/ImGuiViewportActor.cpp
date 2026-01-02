#include "ImGuiViewportActor.h"

#ifdef WITH_IMGUI
#include "SImGuiWidgets.h"
#include "ImGuiSubsystem.h"
#include "Engine/GameViewportClient.h"

class SImGuiScopedWidget : public SImGuiWidgetBase
{
	using Super = SImGuiWidgetBase;
public:
	SLATE_BEGIN_ARGS(SImGuiScopedWidget)
		: _MainViewportWindow(nullptr)
		, _ConfigFileName(nullptr)
		{
		}
		SLATE_ARGUMENT(TSharedPtr<SWindow>, MainViewportWindow);
		SLATE_ARGUMENT(const ANSICHAR*, ConfigFileName);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs)
	{
		Super::Construct(
			Super::FArguments()
			.MainViewportWindow(InArgs._MainViewportWindow)
			.ConfigFileName(InArgs._ConfigFileName));

		// disable tick as the ImGui context is manually updated
		SetCanTick(false);
		FCoreDelegates::OnBeginFrame.AddRaw(this, &SImGuiScopedWidget::BeginFrame);
		FCoreDelegates::OnEndFrame.AddRaw(this, &SImGuiScopedWidget::EndFrame);

		// first frame setup
		BeginFrame();
	}

	~SImGuiScopedWidget()
	{
		FCoreDelegates::OnBeginFrame.RemoveAll(this);
		FCoreDelegates::OnEndFrame.RemoveAll(this);
	}

	FImGuiTickContext* GetTickContext() const { return (FImGuiTickContext*)m_ImGuiContext->IO.UserData; }

private:
	void BeginFrame()
	{
		check(!m_ImGuiContext->WithinFrameScope);
		BeginImGuiFrame(GetCachedGeometry(), FApp::GetDeltaTime());
	}

	void EndFrame()
	{
		EndImGuiFrame();
	}

	virtual void TickImGuiInternal(FImGuiTickContext* TickContext) override
	{
		checkNoEntry();
	}
};

struct FImGuiScopedWidget : FNoncopyable
{
	explicit FImGuiScopedWidget(FImGuiTickContext* InContext)
		: Context(InContext)
	{
		FImGuiTickScope::BeginContext(Context);
	}
	~FImGuiScopedWidget()
	{
		FImGuiTickScope::EndContext();
	}
	operator bool() const
	{
		return Context->ImGuiContext->WithinFrameScope;
	}
	FImGuiTickContext* Context = nullptr;
};

#endif //#ifdef WITH_IMGUI

AImGuiViewportActor::AImGuiViewportActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AImGuiViewportActor::BeginPlay()
{
	Super::BeginPlay();

#ifdef WITH_IMGUI
	auto InitializeWidget = [this]()
		{
			UGameViewportClient* GameViewport = GEngine->GameViewport;
			if (!IsValid(GameViewport))
			{
				return;
			}

			// TODO: add focus logic to toggle b/w ImGui and Game inputs
			ImGuiWidget = SNew(SImGuiScopedWidget)
				.MainViewportWindow(GEngine->GameViewport->GetWindow())
				.ConfigFileName("GameplayWidget");
			GEngine->GameViewport->AddViewportWidgetContent(ImGuiWidget.ToSharedRef(), TNumericLimits<int32>::Max());
		};

	if (UGameViewportClient* GameViewport = GEngine->GameViewport)
	{
		InitializeWidget();
	}
	else
	{
		UGameViewportClient::OnViewportCreated().AddWeakLambda(this, [&]() { InitializeWidget(); });
	}
#endif
}

void AImGuiViewportActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
#ifdef WITH_IMGUI
	if (ImGuiWidget.IsValid())
	{
		if (UGameViewportClient* GameViewport = GEngine->GameViewport)
		{
			GameViewport->RemoveViewportWidgetContent(ImGuiWidget.ToSharedRef());
		}
		ImGuiWidget.Reset();
	}
	UGameViewportClient::OnViewportCreated().RemoveAll(this);
#endif

	Super::EndPlay(EndPlayReason);
}

void AImGuiViewportActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#ifdef WITH_IMGUI
	if (ImGuiWidget.IsValid())
	{
		FImGuiScopedWidget ScopedWidget{ ImGuiWidget->GetTickContext() };
		if (ScopedWidget)
		{
			if (ImGui::Begin("ViewportWindow"))
			{
				ImGui::Text("Some text...");
				if (ImGui::Button("Button"))
				{
				}
			}
			ImGui::End();
		}
	}
#endif
}
