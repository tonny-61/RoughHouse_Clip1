#include "RHFighter.h"

#include "Animation/AnimationAsset.h"
#include "Animation/AnimSequence.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "UObject/ConstructorHelpers.h"

ARHFighter::ARHFighter()
{
    PrimaryActorTick.bCanEverTick = true;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(SceneRoot);

    CharacterMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
    CharacterMesh->SetupAttachment(SceneRoot);
    CharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    NameLabel = CreateDefaultSubobject<UTextRenderComponent>(TEXT("NameLabel"));
    NameLabel->SetupAttachment(SceneRoot);
    NameLabel->SetRelativeLocation(FVector(0.0f, 0.0f, 220.0f));
    NameLabel->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    NameLabel->SetWorldSize(28.0f);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    auto CreateBlock = [this](const TCHAR* Name, const FVector& Location, const FVector& Scale)
    {
        UStaticMeshComponent* Block = CreateDefaultSubobject<UStaticMeshComponent>(Name);
        Block->SetupAttachment(SceneRoot);
        Block->SetRelativeLocation(Location);
        Block->SetRelativeScale3D(Scale);
        Block->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        if (CubeAsset.Succeeded())
        {
            Block->SetStaticMesh(CubeAsset.Object);
        }
        return Block;
    };
    BodyBlock = CreateBlock(TEXT("BodyBlock"), FVector(0.0f, 0.0f, 104.0f), FVector(0.42f, 0.28f, 0.58f));
    HeadBlock = CreateBlock(TEXT("HeadBlock"), FVector(0.0f, 0.0f, 184.0f), FVector(0.25f, 0.25f, 0.25f));
    LeftArmBlock = CreateBlock(TEXT("LeftArmBlock"), FVector(8.0f, -40.0f, 125.0f), FVector(0.16f, 0.16f, 0.45f));
    RightArmBlock = CreateBlock(TEXT("RightArmBlock"), FVector(8.0f, 40.0f, 125.0f), FVector(0.16f, 0.16f, 0.45f));
    LeftLegBlock = CreateBlock(TEXT("LeftLegBlock"), FVector(0.0f, -17.0f, 42.0f), FVector(0.17f, 0.17f, 0.42f));
    RightLegBlock = CreateBlock(TEXT("RightLegBlock"), FVector(0.0f, 17.0f, 42.0f), FVector(0.17f, 0.17f, 0.42f));

    static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyAsset(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
    static ConstructorHelpers::FObjectFinder<USkeletalMesh> QuinnAsset(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> IdleAsset(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/MM_Idle.MM_Idle"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> WalkAsset(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Walk/MF_Unarmed_Walk_Fwd.MF_Unarmed_Walk_Fwd"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> PunchOneAsset(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Attack/MM_Attack_01.MM_Attack_01"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> PunchTwoAsset(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/Attack/MM_Attack_02.MM_Attack_02"));
    static ConstructorHelpers::FObjectFinder<UAnimSequence> HitAsset(TEXT("/Game/Characters/Mannequins/Anims/Rifle/HitReact/MM_HitReact_Front_Lgt_01.MM_HitReact_Front_Lgt_01"));

    MannyMesh = MannyAsset.Object;
    QuinnMesh = QuinnAsset.Object;
    IdleAnimation = IdleAsset.Object;
    WalkAnimation = WalkAsset.Object;
    PunchOneAnimation = PunchOneAsset.Object;
    PunchTwoAnimation = PunchTwoAsset.Object;
    HitReactionAnimation = HitAsset.Object;
}

void ARHFighter::BeginPlay()
{
    Super::BeginPlay();
    PlayLocomotion();
}

void ARHFighter::ConfigureFighter(FName InName, bool bUseManny)
{
    FighterName = InName;
    NameLabel->SetText(FText::FromName(FighterName));
    NameLabel->SetTextRenderColor(bUseManny ? FColor(255, 120, 45) : FColor(65, 170, 255));
    CharacterMesh->SetSkeletalMesh(bUseManny ? MannyMesh : QuinnMesh);
    CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    // Keep a clean, always-visible arena fallback for the Clip 1 recording.
    // The free Epic mannequins remain assigned above; these simple silhouettes
    // guarantee that both fighters are readable on every local machine.
    BodyBlock->SetVisibility(true);
    HeadBlock->SetVisibility(true);
    LeftArmBlock->SetVisibility(true);
    RightArmBlock->SetVisibility(true);
    LeftLegBlock->SetVisibility(true);
    RightLegBlock->SetVisibility(true);
    PlayLocomotion();
}

void ARHFighter::SetMoving(bool bInMoving)
{
    if (bMoving == bInMoving)
    {
        return;
    }

    bMoving = bInMoving;
    if (!IsBusy())
    {
        PlayLocomotion();
    }
}

void ARHFighter::PlayLocomotion()
{
    UAnimSequence* DesiredAnimation = bMoving ? WalkAnimation : IdleAnimation;
    if (CharacterMesh && DesiredAnimation)
    {
        CharacterMesh->PlayAnimation(DesiredAnimation, true);
        bPlayingLocomotion = true;
    }
}

void ARHFighter::Punch()
{
    if (IsBusy())
    {
        return;
    }

    ++PunchCount;
    AttackRemaining = PunchDuration;
    bDamageApplied = false;
    bPlayingLocomotion = false;
    UAnimSequence* AttackAnimation = (PunchCount % 2 == 0) ? PunchTwoAnimation : PunchOneAnimation;
    if (CharacterMesh && AttackAnimation)
    {
        CharacterMesh->PlayAnimation(AttackAnimation, false);
    }
}

void ARHFighter::ReceiveHit(const FVector& FromDirection)
{
    HitRemaining = HitReactionDuration;
    AttackRemaining = 0.0f;
    bPlayingLocomotion = false;
    if (CharacterMesh && HitReactionAnimation)
    {
        CharacterMesh->PlayAnimation(HitReactionAnimation, false);
    }
}

void ARHFighter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Opponent)
    {
        FVector Direction = Opponent->GetActorLocation() - GetActorLocation();
        Direction.Z = 0.0f;
        if (!Direction.IsNearlyZero())
        {
            SetActorRotation(FMath::RInterpTo(GetActorRotation(), Direction.Rotation(), DeltaTime, 14.0f));
        }
    }

    if (AttackRemaining > 0.0f)
    {
        AttackRemaining = FMath::Max(0.0f, AttackRemaining - DeltaTime);
        const float AttackAlpha = 1.0f - AttackRemaining / PunchDuration;
        if (!bDamageApplied && AttackAlpha >= 0.42f && Opponent)
        {
            bDamageApplied = true;
            const FVector ToOpponent = Opponent->GetActorLocation() - GetActorLocation();
            const bool bInRange = ToOpponent.Size2D() <= PunchRange;
            const bool bFacingOpponent = FVector::DotProduct(GetActorForwardVector(), ToOpponent.GetSafeNormal2D()) > 0.65f;
            if (bInRange && bFacingOpponent)
            {
                Opponent->ReceiveHit(ToOpponent);
            }
        }
    }

    if (HitRemaining > 0.0f)
    {
        HitRemaining = FMath::Max(0.0f, HitRemaining - DeltaTime);
    }

    if (!IsBusy() && !bPlayingLocomotion)
    {
        PlayLocomotion();
    }

    ResetBlockingPose();
    if (AttackRemaining > 0.0f)
    {
        const float Alpha = 1.0f - AttackRemaining / PunchDuration;
        const float Reach = FMath::Sin(Alpha * PI) * 75.0f;
        UStaticMeshComponent* PunchArm = (PunchCount % 2 == 0) ? RightArmBlock : LeftArmBlock;
        const float Side = (PunchCount % 2 == 0) ? 40.0f : -40.0f;
        PunchArm->SetRelativeLocation(FVector(8.0f + Reach, Side, 125.0f));
    }
}

void ARHFighter::ResetBlockingPose()
{
    LeftArmBlock->SetRelativeLocation(FVector(8.0f, -40.0f, 125.0f));
    RightArmBlock->SetRelativeLocation(FVector(8.0f, 40.0f, 125.0f));
}
