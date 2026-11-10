#include "RHFighter.h"

#include "Animation/AnimationAsset.h"
#include "Animation/AnimSequence.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/TextRenderComponent.h"
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
}
