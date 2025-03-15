// Copyright Surround, Inc. All Rights Reserved.


#include "SMPlayerCharacterBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "NiagaraComponent.h"
#include "SMGameplayTags.h"
#include "StereoMixLog.h"
#include "AbilitySystem/SMAbilitySystemComponent.h"
#include "AbilitySystem/SMTags.h"
#include "AbilitySystem/AttributeSets/SMCharacterAttributeSet.h"
#include "Actors/Notes/SMNoteBase.h"
#include "Actors/Tiles/SMTile.h"
#include "Actors/Weapons/SMWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/Character/SMCharacterLandingComponent.h"
#include "Components/Character/SMCharacterLevelTransitionVisualComponent.h"
#include "Components/Character/SMFaceControlComponent.h"
#include "Components/Character/SMCharacterMovementComponent.h"
#include "Components/Character/SMEmoticonWidgetComponent.h"
#include "Components/Character/SMHIC_Character.h"
#include "Components/Character/SMNeutralizeTimerComponent.h"
#include "Components/Common/SMTeamComponent.h"
#include "Components/Core/SMTileManagerComponent.h"
#include "Components/PlayerController/SMScreenIndicatorComponent.h"
#include "Data/SMControlData.h"
#include "Data/Ability/SMActiveAbility.h"
#include "Data/DataAsset/Character/SMPlayerCharacterDataAsset.h"
#include "Data/DataTable/SMCharacterData.h"
#include "FunctionLibraries/SMAbilitySystemBlueprintLibrary.h"
#include "FunctionLibraries/SMBasicShapeFunctionLibrary.h"
#include "FunctionLibraries/SMDataTableFunctionLibrary.h"
#include "FunctionLibraries/SMScoreFunctionLibrary.h"
#include "FunctionLibraries/SMTeamBlueprintLibrary.h"
#include "FunctionLibraries/SMTileFunctionLibrary.h"
#include "Games/Round/SMRoundComponent.h"
#include "Input/CommonUIActionRouterBase.h"
#include "Messages/SMVerbMessage.h"
#include "Net/UnrealNetwork.h"
#include "Player/SMPlayerController.h"
#include "Player/SMPlayerState.h"
#include "UI/Game/SMNameplateWidget.h"
#include "UI/Game/SMScreenIndicatorWidget.h"
#include "UI/GameplayPop/SMGameplayPopSubsystem.h"
#include "Utilities/SMCollision.h"
#include "Utilities/SMLog.h"

void FSMCharacterStateEntry::PreReplicatedRemove(const FFastArraySerializer& InArraySerializer)
{
	UE_LOG(LogTemp, Warning, TEXT("PreReplicatedRemove"));
}

void FSMCharacterStateEntry::PostReplicatedAdd(const FFastArraySerializer& InArraySerializer)
{
	UE_LOG(LogTemp, Warning, TEXT("PostReplicatedAdd"));
}

void FSMCharacterStateEntry::PostReplicatedChange(const FFastArraySerializer& InArraySerializer)
{
	UE_LOG(LogTemp, Warning, TEXT("PostReplicatedChange"));
}

ASMPlayerCharacterBase::ASMPlayerCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bAlwaysRelevant = true;

	UCapsuleComponent* CachedCapsuleComponent = GetCapsuleComponent();
	CachedCapsuleComponent->SetCollisionProfileName(SMCollisionProfileName::Player);

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();
	CachedMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CachedMeshComponent->SetRelativeRotation(FRotator(0.0, -90.0, 0.0));
	CachedMeshComponent->bRenderCustomDepth = true;
	CachedMeshComponent->bReceivesDecals = false;

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->MaxAcceleration = 9999.0f;
	CachedMovementComponent->MaxStepHeight = 10.0f;
	CachedMovementComponent->SetWalkableFloorAngle(5.0f);
	CachedMovementComponent->bCanWalkOffLedges = false;

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionProfileName(SMCollisionProfileName::PlayerProjectileHitbox);
	HitBox->InitCapsuleSize(125.0f, 125.0f);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetRelativeRotation(FRotator(-52.5f, 0.0, 0.0));
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->TargetArmLength = 2000.0f;
	CameraBoom->TargetOffset = FVector(-50.0, 0.0, 0.0);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->SetFieldOfView(75.0f);

	ListenerComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ListenerComponent"));
	ListenerComponent->SetAbsolute(false, true);
	ListenerComponent->SetupAttachment(RootComponent);

	TeamComponent = CreateDefaultSubobject<USMTeamComponent>(TEXT("Team"));

	NoteSlotComponent = CreateDefaultSubobject<USceneComponent>(TEXT("NoteSlotComponent"));
	NoteSlotComponent->SetupAttachment(CachedMeshComponent);
	NoteSlotComponent->SetAbsolute(false, true, false);

	NeutralizeTimerComponent = CreateDefaultSubobject<USMNeutralizeTimerComponent>(TEXT("NeutralizeTimerComponent"));

	LandingComponent = CreateDefaultSubobject<USMCharacterLandingComponent>(TEXT("LandingComponent"));

	LevelTransitionVisualComponent = CreateDefaultSubobject<USMCharacterLevelTransitionVisualComponent>(TEXT("LevelTransitionVisualComponent"));

	NameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameplateWidgetComponent"));
	NameplateWidgetComponent->SetupAttachment(RootComponent);
	NameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameplateWidgetComponent->SetDrawAtDesiredSize(true);

	DirectionIndicatorMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DirectionIndicatorVFXComponent"));
	DirectionIndicatorMeshComponent->SetupAttachment(RootComponent);
	DirectionIndicatorMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	DirectionIndicatorMeshComponent->SetVisibility(false);
	DirectionIndicatorMeshComponent->SetRelativeRotation(FRotator(0.0, 45.0, 0.0));

	FloorCircleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorCircleMeshComponent"));
	FloorCircleMeshComponent->SetupAttachment(RootComponent);
	FloorCircleMeshComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	FloorCircleMeshComponent->SetVisibility(false);
	FloorCircleMeshComponent->ComponentTags.Add(TEXT("FloorCircle"));

	DefaultMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DefaultMoveTrailFXComponent"));
	DefaultMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	DefaultMoveTrailFXComponent->SetAbsolute(false, true, true);
	DefaultMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	DefaultMoveTrailFXComponent->SetAutoActivate(false);

	CatchMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CatchMoveTrailFXComponent"));
	CatchMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	CatchMoveTrailFXComponent->SetAbsolute(false, true, true);
	CatchMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	CatchMoveTrailFXComponent->SetAutoActivate(false);

	ImmuneMoveTrailFXComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ImmuneMoveTrailFXComponent"));
	ImmuneMoveTrailFXComponent->SetupAttachment(CachedMeshComponent);
	ImmuneMoveTrailFXComponent->SetAbsolute(false, true, true);
	ImmuneMoveTrailFXComponent->SetCollisionProfileName(SMCollisionProfileName::NoCollision);
	ImmuneMoveTrailFXComponent->SetAutoActivate(false);

	HIC = CreateDefaultSubobject<USMHIC_Character>(TEXT("HIC"));
	HIC->OnHoldStateEntry.AddUObject(this, &ThisClass::OnHoldStateEntry);
	HIC->OnHoldStateExit.AddUObject(this, &ThisClass::OnHoldStateExit);

	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Held);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Neutralized);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::Immune);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	IgnoreAttackTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);

	LockAimTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockAimTags.AddTag(SMTags::Character::State::Common::Held);
	LockAimTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	LockAimTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	LockAimTags.AddTag(SMTags::Character::State::Common::Neutralized);

	LockCameraTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockCameraTags.AddTag(SMTags::Character::State::Common::Held);
	LockCameraTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	LockCameraTags.AddTag(SMTags::Character::State::Common::Neutralized);

	LockMovementTags.AddTag(SMTags::Character::State::Common::Uncontrollable);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Held);
	LockMovementTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	LockMovementTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Neutralized);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Jump);
	LockMovementTags.AddTag(SMTags::Character::State::Common::Stun);

	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Held);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::NoiseBreak);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::NoiseBreaked);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Neutralized);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Immune);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Jump);
	PushBackImmuneTags.AddTag(SMTags::Character::State::Common::Stun);

	FaceControlComponent = CreateDefaultSubobject<USMFaceControlComponent>(TEXT("FaceControlComponent"));

	EmoticonWidgetComponent = CreateDefaultSubobject<USMEmoticonWidgetComponent>(TEXT("EmoticonWidgetComponent"));
	EmoticonWidgetComponent->SetupAttachment(RootComponent);
}

void ASMPlayerCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsActorHidden, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsNoteState, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsCollisionEnabled, SharedParams);

	SharedParams.Condition = COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsMovementEnabled, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bUseControllerRotation, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LastAttacker, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, LastAttackTags, SharedParams);

	DOREPLIFETIME(ThisClass, Note);
}

void ASMPlayerCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
	if (!World || !CommonDataAsset)
	{
		return;
	}

	const ESMTeam OwnerTeam = GetTeam();

	USkeletalMeshComponent* CachedMeshComponent = GetMesh();

	UCharacterMovementComponent* CachedMovementComponent = GetCharacterMovement();
	CachedMovementComponent->GravityScale = 2.0f;

	const UCapsuleComponent* CachedCapsuleComponent = GetCapsuleComponent();
	const float CapsuleHalfHeight = CachedCapsuleComponent ? CachedCapsuleComponent->GetScaledCapsuleHalfHeight() : 0.0f;
	DirectionIndicatorMeshComponent->SetRelativeLocation(FVector(0.0, 0.0, -CapsuleHalfHeight));
	FloorCircleMeshComponent->SetRelativeLocation(FVector(0.0, 0.0, -CapsuleHalfHeight));

	if (UStaticMesh* PlaneMesh = USMBasicShapeFunctionLibrary::GetPlaneMesh())
	{
		DirectionIndicatorMeshComponent->SetStaticMesh(PlaneMesh);
		FloorCircleMeshComponent->SetStaticMesh(PlaneMesh);
	}

	const float DirectionIndicatorScale = (DirectionIndicatorRadius / 100.0f) * 2.0f;
	DirectionIndicatorMeshComponent->SetRelativeScale3D(FVector(DirectionIndicatorScale));

	if (UMaterialInterface* DirectionIndicatorMaterial = CommonDataAsset->DirectionIndicatorMaterial)
	{
		DirectionIndicatorMeshComponent->SetMaterial(0, DirectionIndicatorMaterial);
	}

	constexpr float FloorCircleOffset = 15.0f;
	const float FloorCircleScale = ((FloorCircleRadius - FloorCircleOffset) / 100.0f) * 2.0f;
	FloorCircleMeshComponent->SetRelativeScale3D(FVector(FloorCircleScale));

	if (UMaterialInterface* FloorCircleMaterial = CommonDataAsset->FloorCircleMaterial)
	{
		FloorCircleMeshComponent->SetMaterial(0, FloorCircleMaterial);
	}

	// 트레일 위치를 교정하기 위해 재어태치합니다. 재어태치하는 이유는 생성자에서는 메시가 null이므로 소켓을 찾을 수 없기 때문입니다.
	DefaultMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	CatchMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);
	ImmuneMoveTrailFXComponent->AttachToComponent(CachedMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TrailSocket);

	// 원본 머티리얼을 저장해둡니다. 플레이 도중 시시각각 머티리얼이 변하게 되는데 이때 기존 머티리얼로 돌아오기 위해 사용됩니다.
	OriginalMaterials = CachedMeshComponent->GetMaterials();
	OriginalOverlayMaterial = CachedMeshComponent->GetOverlayMaterial();

	if (HasAuthority())
	{
		if (DataAsset->NoteClass.Contains(OwnerTeam))
		{
			if (Note = World->SpawnActor<ASMNoteBase>(DataAsset->NoteClass[OwnerTeam]); Note)
			{
				Note->AttachToComponent(NoteSlotComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Note->SetOwner(this);
			}
		}

		if (USMTileManagerComponent* TileManager = USMTileFunctionLibrary::GetTileManagerComponent(World))
		{
			TileManager->OnTilesCaptured.AddDynamic(this, &ThisClass::OnTilesCaptured);
		}

		if (const TSubclassOf<ASMWeaponBase>& CachedWeaponClass = DataAsset->WeaponClass.Contains(OwnerTeam) ? DataAsset->WeaponClass[OwnerTeam] : nullptr)
		{
			if (Weapon = GetWorld()->SpawnActor<ASMWeaponBase>(CachedWeaponClass); Weapon)
			{
				Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, DataAsset->WeaponSocketName);
				Weapon->SetOwner(this);
			}
		}
	}

	TeamComponent->OnTeamChanged.AddUObject(this, &ThisClass::OnTeamChanged);
}

void ASMPlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (const USMControlData* ControlData = GetDataAsset()->InputActions)
		{
			EnhancedInputComponent->BindAction(ControlData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
			EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Triggered, this, &ThisClass::GAInputPressed, ESMActiveAbilityType::Attack);
			EnhancedInputComponent->BindAction(ControlData->AttackAction, ETriggerEvent::Completed, this, &ThisClass::GAInputReleased, ESMActiveAbilityType::Attack);
			EnhancedInputComponent->BindAction(ControlData->HoldAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, ESMActiveAbilityType::Hold);
			EnhancedInputComponent->BindAction(ControlData->SkillAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, ESMActiveAbilityType::Skill);
			EnhancedInputComponent->BindAction(ControlData->NoiseBreakAction, ETriggerEvent::Started, this, &ThisClass::GAInputPressed, ESMActiveAbilityType::NoiseBreak);
		}

		for (UActorComponent* Component : GetComponents())
		{
			if (ISMInputBindableInterface* InputBindableComponents = Component->Implements<USMInputBindableInterface>() ? Cast<ISMInputBindableInterface>(Component) : nullptr) // 인풋바인드 가능한 컴포넌트를 위한 호출입니다.
			{
				InputBindableComponents->SetupInputComponent(EnhancedInputComponent);
			}
		}
	}
}

void ASMPlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SMPlayerController = GetController<ASMPlayerController>();

	if (IsLocallyControlled())
	{
		if (SMPlayerController)
		{
			SMPlayerController->SetAudioListenerOverride(ListenerComponent, FVector::ZeroVector, FRotator::ZeroRotator);
		}

		DirectionIndicatorMeshComponent->SetVisibility(true);
		FloorCircleMeshComponent->SetVisibility(true);
	}

	if (GetNetMode() != NM_DedicatedServer)
	{
		// 1초 주기를 갖고 지속적으로 아웃라인을 업데이트합니다. 확실하게 아웃라인을 적용하기 위함입니다. 라운드가 시작된 뒤로는 종료됩니다.
		UpdateStencilBeforeRoundStart();

		if (const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr)
		{
			FaceControlComponent->AddBlinkCurve(CommonDataAsset->BlinkCurve);
			FaceControlComponent->AddMouthCurve(CommonDataAsset->MouthCurve);
			FaceControlComponent->Activate(true);
		}
	}
}

void ASMPlayerCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled())
	{
		CameraBoom->SetRelativeLocation(FVector::ZeroVector); // 카메라 위치를 초기화합니다.

		const APlayerController* PC = Cast<APlayerController>(GetController());
		if (const UCommonUIActionRouterBase* Router = ULocalPlayer::GetSubsystem<UCommonUIActionRouterBase>(PC->GetLocalPlayer()))
		{
			if (Router->GetActiveInputMode() == ECommonInputMode::Menu)
			{
				return;
			}
		}

		if (ASC.IsValid())
		{
			if (!ASC->HasAnyMatchingGameplayTags(LockAimTags))
			{
				UpdateFocusToCursor();
			}
			if (!ASC->HasAnyMatchingGameplayTags(LockCameraTags))
			{
				UpdateCameraLocation();
			}
		}
	}
}

void ASMPlayerCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ASC.IsValid())
	{
		ASC->OnTagUpdatedEvent.RemoveAll(this);
	}

	if (Weapon)
	{
		Weapon->Destroy();
	}

	if (Note)
	{
		Note->Destroy();
	}

	if (const UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ASMPlayerCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	SMPlayerController = GetController<ASMPlayerController>();
}

void ASMPlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SMPlayerController = GetController<ASMPlayerController>();

	InitASC();
}

void ASMPlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitASC();
	if (const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr)
	{
		const ESMTeam SourceTeam = GetTeam();

		DefaultMoveTrailFXComponent->SetAsset(CommonDataAsset->DefaultMoveTrailVFX[SourceTeam]);
		DefaultMoveTrailFXComponent->Activate();

		CatchMoveTrailFXComponent->SetAsset(CommonDataAsset->HoldMoveTrailVFX[SourceTeam]);
		CatchMoveTrailFXComponent->Deactivate();

		ImmuneMoveTrailFXComponent->SetAsset(CommonDataAsset->ImmuneMoveTrailVFX[SourceTeam]);
		ImmuneMoveTrailFXComponent->Deactivate();
	}
}

void ASMPlayerCharacterBase::SetActorHiddenInGame(bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (Weapon)
	{
		Weapon->SetActorHiddenInGame(bNewHidden);
	}

	if (Note)
	{
		Note->SetActorHiddenInGame(bNewHidden);
	}
}

UAbilitySystemComponent* ASMPlayerCharacterBase::GetAbilitySystemComponent() const
{
	return ASC.Get();
}

USMAbilitySystemComponent* ASMPlayerCharacterBase::GetStereoMixAbilitySystemComponent() const
{
	return ASC.Get();
}

ESMTeam ASMPlayerCharacterBase::GetTeam() const
{
	return TeamComponent->GetTeam();
}

USMHoldInteractionComponent* ASMPlayerCharacterBase::GetHoldInteractionComponent() const
{
	return HIC;
}

FVector ASMPlayerCharacterBase::GetLocationFromCursor(bool bUseZeroBasis, float MaxDistance)
{
	FVector TargetLocation = GetCursorTargetingPoint(bUseZeroBasis);
	if (MaxDistance > 0.0f)
	{
		const FVector SourceLocation = GetActorLocation();
		const FVector SourceLocationWithTargetZ(SourceLocation.X, SourceLocation.Y, TargetLocation.Z);
		const FVector SourceToTarget = TargetLocation - SourceLocationWithTargetZ;

		if (SourceToTarget.SizeSquared() >= FMath::Square(MaxDistance))
		{
			const FVector TargetDirection = SourceToTarget.GetSafeNormal2D();
			TargetLocation = SourceLocationWithTargetZ + (TargetDirection * MaxDistance);
		}
	}

	return TargetLocation;
}

bool ASMPlayerCharacterBase::GetTileLocationFromCursor(FVector& OutTileLocation, float MaxDistance)
{
	OutTileLocation = FVector::ZeroVector;

	const FVector CursorLocation = GetLocationFromCursor(true, MaxDistance);
	if (const ASMTile* Tile = USMTileFunctionLibrary::GetTileFromLocation(GetWorld(), CursorLocation))
	{
		OutTileLocation = Tile->GetTileLocation();
		return true;
	}

	// 만약 해당위치에 타일이 없다면 자신의 위치부터 최대사거리까지 경로상에 있는 모든 타일을 검사한 후 커서에 가장 가까운 타일을 검출해냅니다.
	const FVector OwnerLocation = GetActorLocation();
	const FVector OwnerLocationWithCursorZ(OwnerLocation.X, OwnerLocation.Y, CursorLocation.Z);
	const FVector OwnerToCursorDirection = (CursorLocation - OwnerLocationWithCursorZ).GetSafeNormal2D();
	const FVector MaxDistanceLocation = OwnerLocationWithCursorZ + (OwnerToCursorDirection * MaxDistance);

	const ASMTile* NearestTileFromCursor = nullptr;
	float NearestDistanceSquaredFromCursor = UE_MAX_FLT;
	for (const ASMTile* PathTile : USMTileFunctionLibrary::GetTilesInCapsule(GetWorld(), OwnerLocationWithCursorZ, MaxDistanceLocation, 3.0f))
	{
		// 장애물이 있는지 검사하기 위해 GetTileFromLocation을 사용해서 필터링합니다.
		if (const ASMTile* Tile = USMTileFunctionLibrary::GetTileFromLocation(GetWorld(), PathTile->GetTileLocation()))
		{
			const float CursorToTileDistanceSquared = FVector::DistSquared2D(CursorLocation, Tile->GetTileLocation());
			if (CursorToTileDistanceSquared < NearestDistanceSquaredFromCursor)
			{
				NearestDistanceSquaredFromCursor = CursorToTileDistanceSquared;
				NearestTileFromCursor = PathTile;
			}
		}
	}

	if (NearestTileFromCursor)
	{
		OutTileLocation = NearestTileFromCursor->GetTileLocation();
		return true;
	}

	return false;
}

void ASMPlayerCharacterBase::ServerSetActorHiddenInGame_Implementation(bool bNewIsActorHidden)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsActorHidden, this);
	bIsActorHidden = bNewIsActorHidden;
	OnRep_bIsActorHidden();
}

void ASMPlayerCharacterBase::ServerSetCollisionEnabled_Implementation(bool bNewIsCollisionEnabled)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsCollisionEnabled, this);
	bIsCollisionEnabled = bNewIsCollisionEnabled;
	OnRep_bIsCollisionEnabled();
}

void ASMPlayerCharacterBase::ServerSetMovementEnabled_Implementation(bool bNewIsMovementEnabled)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsMovementEnabled, this);
	bIsMovementEnabled = bNewIsMovementEnabled;
	OnRep_bIsMovementEnabled();
}

void ASMPlayerCharacterBase::ServerSetUseControllerRotation_Implementation(bool bNewUseControllerRotation)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bUseControllerRotation, this);
	bUseControllerRotation = bNewUseControllerRotation;
	OnRep_bUseControllerRotation();
}

void ASMPlayerCharacterBase::MulticastAddScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// 자기 자신은 제외합니다.
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 추가해줍니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		if (const TSubclassOf<USMScreenIndicatorWidget> ScreenIndicatorWidgetClass = GetDataAsset()->GetCommonDataAsset()->ScreenIndicatorWidgetClass)
		{
			ScreenIndicatorComponent->AddScreenIndicator(TargetActor, ScreenIndicatorWidgetClass);
		}
		else
		{
			UE_LOG(LogStereoMix, Error, TEXT("스크린 인디케이터 위젯 클래스가 무효합니다."));
		}
	}
}

void ASMPlayerCharacterBase::MulticastRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	// 자기 자신은 인디케이터가 추가되지 않았으니 제외합니다.
	const ASMPlayerCharacterBase* TargetCharacter = Cast<ASMPlayerCharacterBase>(TargetActor);
	if (!TargetCharacter || TargetCharacter->IsLocallyControlled())
	{
		return;
	}

	// 로컬 컨트롤러를 찾고 스크린 인디케이터를 제거합니다.
	const UWorld* World = GetWorld();
	const APlayerController* LocalPlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = LocalPlayerController ? LocalPlayerController->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->RemoveScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::ClientRemoveScreenIndicatorToSelf_Implementation(AActor* TargetActor)
{
	if (USMScreenIndicatorComponent* ScreenIndicatorComponent = Controller ? Controller->GetComponentByClass<USMScreenIndicatorComponent>() : nullptr)
	{
		ScreenIndicatorComponent->RemoveScreenIndicator(TargetActor);
	}
}

void ASMPlayerCharacterBase::MulticastSetCharacterStateVisibility_Implementation(bool bNewVisibility)
{
	if (HasAuthority())
	{
		return;
	}

	NameplateWidgetComponent->SetVisibility(bNewVisibility);
}

void ASMPlayerCharacterBase::MulticastRPCSetYawRotation_Implementation(float InYaw)
{
	SetActorRotation(FRotator(0.0, InYaw, 0.0));
}

void ASMPlayerCharacterBase::MulticastRPCSetLocation_Implementation(const FVector_NetQuantize10& NewLocation)
{
	SetActorLocation(NewLocation);
}

void ASMPlayerCharacterBase::SetLastAppliedBuffAndGCTag(FActiveGameplayEffectHandle NewBuffHandle, FGameplayTag NewGCTag)
{
	if (LastAppliedBuffHandle.IsValid())
	{
		if (USMCharacterMovementComponent* SourceMovement = GetCharacterMovement<USMCharacterMovementComponent>())
		{
			SourceMovement->ClientRemoveMoveSpeedBuff();
		}
		ASC->OnGameplayEffectRemoved_InfoDelegate(LastAppliedBuffHandle)->RemoveAll(this);
		ASC->RemoveActiveGameplayEffect(LastAppliedBuffHandle);
		OnBuffRemoved(FGameplayEffectRemovalInfo());
	}

	LastAppliedBuffHandle = NewBuffHandle;
	LastAppliedGCTag = NewGCTag;

	if (ASC.IsValid())
	{
		FGameplayCueParameters AddGCParams;
		AddGCParams.SourceObject = this;
		ASC->AddGC(this, LastAppliedGCTag, AddGCParams);
	}

	if (LastAppliedBuffHandle.IsValid())
	{
		ASC->OnGameplayEffectRemoved_InfoDelegate(LastAppliedBuffHandle)->AddUObject(this, &ThisClass::OnBuffRemoved);
	}
}

void ASMPlayerCharacterBase::OnBuffRemoved(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	LastAppliedBuffHandle.Invalidate();
	if (ASC.IsValid() && LastAppliedGCTag.IsValid())
	{
		FGameplayCueParameters RemoveGCParams;
		RemoveGCParams.SourceObject = this;
		ASC->RemoveGC(this, LastAppliedGCTag, RemoveGCParams);
		LastAppliedGCTag = FGameplayTag();
	}
}

void ASMPlayerCharacterBase::ReceiveDamage(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags)
{
	const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
	if (!ASC.Get() || !CommonDataAsset)
	{
		return;
	}

	// TODO: 임시로 대미지 증가 버프를 하드코딩해두었습니다.
	if (const USMAbilitySystemComponent* AttackerASC = USMAbilitySystemBlueprintLibrary::GetStereoMixAbilitySystem(NewAttacker))
	{
		if (AttackerASC->HasMatchingGameplayTag(SMTags::Character::State::Common::PowerUp))
		{
			NET_LOG(this, Log, TEXT("%s가 강화된 공격에 맞았습니다."), *GetName());
			constexpr float PowerUpDamageMultiplier = 2.0f;
			InDamageAmount *= PowerUpDamageMultiplier;
		}
	}

	const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(CommonDataAsset->DamageGE, 1.0f, ASC->MakeEffectContext());
	if (GESpecHandle.IsValid())
	{
		LastAttacker = NewAttacker;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, LastAttacker, this);

		LastAttackTags = AggregatedSourceTags;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, LastAttackTags, this);

		GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::Damage, InDamageAmount);
		ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
		USMScoreFunctionLibrary::RecordDamage(this, NewAttacker, InDamageAmount);
	}

	FGameplayEventData EventData;
	EventData.Instigator = NewAttacker;
	EventData.Target = this;
	EventData.EventMagnitude = InDamageAmount;
	EventData.TargetTags = AggregatedSourceTags;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, SMTags::Event::Character::Hit, EventData);

	FGameplayCueParameters GCParams;
	GCParams.SourceObject = this;
	GCParams.Instigator = this;
	GCParams.EffectCauser = NewAttacker;
	GCParams.TargetAttachComponent = GetMesh();
	GCParams.AggregatedSourceTags = AggregatedSourceTags;
	GCParams.RawMagnitude = InDamageAmount;
	ASC->ExecuteGC(this, SMTags::GameplayCue::Common::HitFlash, GCParams);

	MulticastDamageReceived(NewAttacker, InDamageAmount, AggregatedSourceTags);
}

void ASMPlayerCharacterBase::MulticastDamageReceived_Implementation(AActor* NewAttacker, float InDamageAmount, const FGameplayTagContainer& AggregatedSourceTags)
{
	FSMVerbMessage VerbMessage;
	VerbMessage.Verb = StereoMix::Tags::Character_Event_Hit;
	VerbMessage.Instigator = NewAttacker;
	VerbMessage.Target = this;
	VerbMessage.Magnitude = InDamageAmount;
	VerbMessage.InstigatorTags = AggregatedSourceTags;

	UGameplayMessageSubsystem::Get(this).BroadcastMessage(VerbMessage.Verb, VerbMessage);

	if (GetNetMode() != NM_DedicatedServer)
	{
		if (USMGameplayPopSubsystem* PopSubsystem = USMGameplayPopSubsystem::Get(this))
		{
			bool bIsCritical = AggregatedSourceTags.HasTag(SMTags::Ability::NoiseBreak);
			PopSubsystem->QueueActivateDamagePop(NewAttacker, GetActorLocation(), InDamageAmount, bIsCritical);
		}
	}
}

bool ASMPlayerCharacterBase::CanIgnoreAttack() const
{
	if (!ASC.Get())
	{
		return false;
	}

	return ASC->HasAnyMatchingGameplayTags(IgnoreAttackTags);
}

void ASMPlayerCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnCharacterLanded.Broadcast(this);
}

void ASMPlayerCharacterBase::FocusToCursor()
{
	if (!SMPlayerController)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Yellow, TEXT("SMPlayerController가 무효합니다."));
		return;
	}

	const FVector Direction = (GetLocationFromCursor() - GetActorLocation()).GetSafeNormal();
	const FRotator NewRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	Controller->SetControlRotation(NewRotation);
}

void ASMPlayerCharacterBase::AddMoveSpeed(float MoveSpeedMultiplier, float Duration)
{
	USMCharacterMovementComponent* SourceMovement = GetCharacterMovement<USMCharacterMovementComponent>();
	if (!SourceMovement)
	{
		return;
	}

	SourceMovement->AddMoveSpeedBuff(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::ServerRPCCharacterPushBack_Implementation(FVector_NetQuantize10 Velocity)
{
	LaunchCharacter(Velocity, true, true);
}

void ASMPlayerCharacterBase::ClientRPCCharacterPushBack_Implementation(FVector_NetQuantize10 Velocity)
{
	if (!ASC.Get())
	{
		return;
	}

	if (ASC->HasAnyMatchingGameplayTags(PushBackImmuneTags))
	{
		return;
	}

	LaunchCharacter(Velocity, true, true);
	ServerRPCCharacterPushBack(Velocity);
}

void ASMPlayerCharacterBase::ClientRPCAddMoveSpeed_Implementation(float MoveSpeedMultiplier, float Duration)
{
	AddMoveSpeed(MoveSpeedMultiplier, Duration);
	ServerRPCAddMoveSpeed(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::OnTeamChanged(AActor* TeamActor, ESMTeam PreviousTeam, ESMTeam NewTeam)
{
}

void ASMPlayerCharacterBase::ServerSetNoteState_Implementation(bool bNewIsNote)
{
	if (bIsNoteState != bNewIsNote)
	{
		bIsNoteState = bNewIsNote;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsNoteState, this);
		OnRep_bIsNoteState();
	}
}

void ASMPlayerCharacterBase::RecalculateDefaultStencil()
{
	if (GetNetMode() == NM_DedicatedServer)
	{
		return;
	}

	DefaultShaderStencil = ESMShaderStencil::NonOutline;

	if (IsLocallyControlled())
	{
		DefaultShaderStencil = ESMShaderStencil::GreenOutline;
	}
	else if (!USMTeamBlueprintLibrary::IsSameLocalTeam(this))
	{
		DefaultShaderStencil = ESMShaderStencil::RedOutline;
	}
}

void ASMPlayerCharacterBase::Move(const FInputActionValue& InputActionValue)
{
	if (!ASC.Get())
	{
		return;
	}
	if (ASC->HasAnyMatchingGameplayTags(LockMovementTags))
	{
		return;
	}

	const FVector2D InputScalar = InputActionValue.Get<FVector2D>().GetSafeNormal();
	const FRotator CameraYawRotation(0.0, Camera->GetComponentRotation().Yaw, 0.0);
	const FVector ForwardDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(CameraYawRotation).GetUnitAxis(EAxis::Y);
	const FVector MoveVector = (ForwardDirection * InputScalar.X) + (RightDirection * InputScalar.Y);
	AddMovementInput(MoveVector);
}

void ASMPlayerCharacterBase::InitASC()
{
	ASMPlayerState* StereoMixPlayerState = GetPlayerState<ASMPlayerState>();
	if (!ensureAlways(StereoMixPlayerState))
	{
		return;
	}

	ASC = Cast<USMAbilitySystemComponent>(StereoMixPlayerState->GetAbilitySystemComponent());
	if (!ensureAlways(ASC.Get()))
	{
		return;
	}

	HIC->SetASC(ASC.Get());

	if (HasAuthority())
	{
		ASC->ClearAllAbilities();
		ASC->InitAbilityActorInfo(StereoMixPlayerState, this);
		GiveDefaultAbilities();
	}

	TeamComponent->SetTeam(StereoMixPlayerState->GetTeam());

	OnASCInitialized.Broadcast();

	if (GetNetMode() != NM_DedicatedServer)
	{
		GetWorldTimerManager().SetTimerForNextTick([this] { InitUI(); });

		if (const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(CommonDataAsset->HealthAttribute).AddUObject(this, &ThisClass::OnHealthChanged);
		}
	}
}

void ASMPlayerCharacterBase::GiveDefaultAbilities()
{
	const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
	if (!HasAuthority() || !CommonDataAsset || !ASC.Get())
	{
		return;
	}

	InitStat();

	for (const auto& DefaultActiveAbility : DataAsset->DefaultActiveAbilities)
	{
		if (DefaultActiveAbility.Value)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultActiveAbility.Value, 1, static_cast<int32>(DefaultActiveAbility.Key));
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for (const auto& DefaultAbility : CommonDataAsset->DefaultGAs)
	{
		if (DefaultAbility)
		{
			FGameplayAbilitySpec AbilitySpec(DefaultAbility);
			ASC->GiveAbility(AbilitySpec);
		}
	}

	for (const auto& PassiveAbility : CommonDataAsset->PassiveGAs)
	{
		if (PassiveAbility)
		{
			FGameplayAbilitySpec AbilitySpec(PassiveAbility);
			ASC->GiveAbility(AbilitySpec);
			ASC->TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void ASMPlayerCharacterBase::InitStat()
{
	const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
	if (!CommonDataAsset)
	{
		return;
	}

	const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(CommonDataAsset->StatInitGE, 0, ASC->MakeEffectContext());
	if (!GESpecHandle.IsValid())
	{
		return;
	}

	if (const FSMCharacterStatsData* CharacterStat = USMDataTableFunctionLibrary::GetCharacterStatData(CharacterType))
	{
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxHP, CharacterStat->HP);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::HP, CharacterStat->HP);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MoveSpeed, CharacterStat->MoveSpeed);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxStamina, CharacterStat->Stamina);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::Stamina, CharacterStat->Stamina);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::MaxSkillGauge, CharacterStat->SkillGauge);
		GESpecHandle.Data->SetByCallerTagMagnitudes.FindOrAdd(SMTags::AttributeSet::SkillGauge, CharacterStat->SkillGauge);
		ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
	}
}

void ASMPlayerCharacterBase::GAInputPressed(ESMActiveAbilityType InInputID)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	if (FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID)))
	{
		if (GASpec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*GASpec);
		}
		else
		{
			ASC->TryActivateAbility(GASpec->Handle);
		}
	}
}

void ASMPlayerCharacterBase::GAInputReleased(ESMActiveAbilityType InInputID)
{
	if (!ASC.Get())
	{
		NET_LOG(this, Warning, TEXT("ASC가 유효하지 않습니다."));
		return;
	}

	if (FGameplayAbilitySpec* GASpec = ASC->FindAbilitySpecFromInputID(static_cast<int32>(InInputID)))
	{
		if (GASpec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*GASpec);
		}
	}
}

void ASMPlayerCharacterBase::InitUI()
{
	if (bUserInterfaceInitialized)
	{
		return;
	}

	const USMCharacterAttributeSet* SourceAttributeSet = ASC.IsValid() ? ASC->GetSet<USMCharacterAttributeSet>() : nullptr;
	const APlayerState* CachedPlayerState = GetPlayerState();
	const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
	if (!SourceAttributeSet || !CachedPlayerState || !CommonDataAsset)
	{
		return;
	}

	if (USMNameplateWidget* NameplateWidget = CreateWidget<USMNameplateWidget>(GetWorld(), DataAsset->GetCommonDataAsset()->NameplateWidgetClass))
	{
		NameplateWidgetComponent->SetWidget(NameplateWidget);
		NameplateWidget->RegisterCharacter(this);
	}
	bUserInterfaceInitialized = true;
}

FVector ASMPlayerCharacterBase::GetCursorTargetingPoint(bool bUseZeroBasis) const
{
	const FVector SourceLocation = GetActorLocation();
	const float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector CapsuleOffset(0.0, 0.0, -CapsuleHalfHeight);
	const FVector BasisLocation = bUseZeroBasis ? SourceLocation + CapsuleOffset : SourceLocation;

	if (!SMPlayerController)
	{
		return BasisLocation;
	}

	FVector CursorWorldLocation;
	FVector CursorWorldDirection;
	SMPlayerController->DeprojectMousePositionToWorld(CursorWorldLocation, CursorWorldDirection);

	const FPlane Plane(BasisLocation, FVector::UpVector);
	const FVector IntersectionPoint = FMath::RayPlaneIntersection(CursorWorldLocation, CursorWorldDirection, Plane);
	if (!IntersectionPoint.ContainsNaN())
	{
		return IntersectionPoint;
	}

	return BasisLocation;
}

void ASMPlayerCharacterBase::UpdateCameraLocation()
{
	if (!GEngine)
	{
		return;
	}

	if (!SMPlayerController)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Yellow, TEXT("SMPlayerController가 무효합니다."));
		return;
	}

	FVector2D MouseScreenLocation;
	if (!SMPlayerController->GetMousePosition(MouseScreenLocation.X, MouseScreenLocation.Y))
	{
		return;
	}

	const FVector SourceLocation = GetActorLocation();
	const FVector MouseLocation = GetLocationFromCursor();
	FVector TargetLocation = (SourceLocation + MouseLocation) * 0.5;

	const FVector SourceToTargetVector = TargetLocation - SourceLocation;
	const float Distance = SourceToTargetVector.Size();

	const float MaxMouseRange = CameraMoveMouseThreshold / 2.0f;
	const float MoveDistance = FMath::Clamp((Distance / MaxMouseRange) * CameraMoveMaxDistance, 0.0f, CameraMoveMaxDistance);
	if (MoveDistance > 0)
	{
		const FVector AdjustVector = SourceToTargetVector.GetSafeNormal() * MoveDistance;
		TargetLocation = SourceLocation + AdjustVector;
	}
	else
	{
		TargetLocation = SourceLocation;
	}

	CameraBoom->SetWorldLocation(TargetLocation);
}

void ASMPlayerCharacterBase::UpdateFocusToCursor()
{
	FocusToCursor();
}

void ASMPlayerCharacterBase::OnHealthChanged(const FOnAttributeChangeData& AttributeChangeData)
{
}

void ASMPlayerCharacterBase::OnRep_bIsActorHidden()
{
	SetActorHiddenInGame(bIsActorHidden);
}

void ASMPlayerCharacterBase::OnRep_bIsCollisionEnabled()
{
	SetActorEnableCollision(bIsCollisionEnabled);
}

void ASMPlayerCharacterBase::OnRep_bIsMovementEnabled()
{
	UCharacterMovementComponent* SourceMovementComponent = GetCharacterMovement();
	if (!SourceMovementComponent)
	{
		return;
	}

	const EMovementMode MovementMode = bIsMovementEnabled ? MOVE_Walking : MOVE_None;
	SourceMovementComponent->SetMovementMode(MovementMode);
}

void ASMPlayerCharacterBase::OnRep_bUseControllerRotation()
{
	bUseControllerRotationPitch = bUseControllerRotation;
	bUseControllerRotationYaw = bUseControllerRotation;
	bUseControllerRotationRoll = bUseControllerRotation;
}

void ASMPlayerCharacterBase::OnRep_bIsNoteState()
{
	USkeletalMeshComponent* CharacterMeshComponent = GetMesh();
	USceneComponent* WeaponRootComponent = Weapon ? Weapon->GetRootComponent() : nullptr;
	USceneComponent* NoteRootComponent = Note ? Note->GetRootComponent() : nullptr;
	if (!CharacterMeshComponent || !WeaponRootComponent || !NoteRootComponent)
	{
		return;
	}

	CharacterMeshComponent->SetVisibility(!bIsNoteState);

	// SetActorHidden을 하지않고 루트를 기준으로 자식들을 비활성화하는 이유는 Hold, Held 될때 SetActorHidden을 사용하기 때문에 Held 되었을때 무기가 보이게 됩니다. 이를 방지하고자 아예 컴포넌트들의 비지빌리티를 끕니다.
	WeaponRootComponent->SetVisibility(!bIsNoteState, true);
	NoteRootComponent->SetVisibility(bIsNoteState, true);

	if (bIsNoteState)
	{
		Note->PlayAnimation();
	}
	else
	{
		Note->StopAnimation();
	}

	if (IsLocallyControlled())
	{
		DirectionIndicatorMeshComponent->SetVisibility(!bIsNoteState);
	}
}

void ASMPlayerCharacterBase::ServerRPCAddMoveSpeed_Implementation(float MoveSpeedMultiplier, float Duration)
{
	AddMoveSpeed(MoveSpeedMultiplier, Duration);
}

void ASMPlayerCharacterBase::OnTilesCaptured(const AActor* CapturedInstigator, int CapturedTileCount)
{
	if (CapturedInstigator == this)
	{
		const USMCommonCharacterDataAsset* CommonDataAsset = DataAsset ? DataAsset->GetCommonDataAsset() : nullptr;
		if (ASC.IsValid() && !ASC->HasMatchingGameplayTag(SMTags::Character::State::Bass::Charge) && CommonDataAsset) // 임시로 돌진중에 회복을 막습니다.
		{
			const FGameplayEffectSpecHandle GESpecHandle = ASC->MakeOutgoingSpec(CommonDataAsset->StaminaHealGE, 1.0f, ASC->MakeEffectContext());
			if (GESpecHandle.IsValid())
			{
				GESpecHandle.Data->SetSetByCallerMagnitude(SMTags::AttributeSet::SkillGauge, SkillGaugeHealAmountPerCapture * CapturedTileCount);
				ASC->BP_ApplyGameplayEffectSpecToSelf(GESpecHandle);
			}
		}
	}
}

void ASMPlayerCharacterBase::UpdateStencilBeforeRoundStart()
{
	RecalculateDefaultStencil();
	ApplyDefaultStencil();

	if (const USMRoundComponent* RoundManager = USMRoundComponent::Get(this))
	{
		if (RoundManager->GetRoundState() < ESMRoundState::InRound)
		{
			GetWorldTimerManager().SetTimer(StencilUpdateTimerHandle, FTimerDelegate::CreateUObject(this, &ThisClass::UpdateStencilBeforeRoundStart), 1.0f, false);
		}
	}
}
