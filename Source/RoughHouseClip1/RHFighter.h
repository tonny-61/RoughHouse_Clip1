#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RHFighter.generated.h"

class UAnimSequence;
class USkeletalMesh;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

UCLASS()
class ROUGHHOUSECLIP1_API ARHFighter : public AActor
{
    GENERATED_BODY()

public:
    ARHFighter();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    void ConfigureFighter(FName InName, bool bUseManny);
    void SetOpponent(ARHFighter* InOpponent) { Opponent = InOpponent; }
    void SetMoving(bool bInMoving);
    void Punch();
    void ReceiveHit(const FVector& FromDirection);
    bool IsBusy() const { return AttackRemaining > 0.0f || HitRemaining > 0.0f; }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoughHouse|Combat")
    float PunchRange = 205.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoughHouse|Combat")
    float PunchDuration = 0.62f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RoughHouse|Combat")
    float HitReactionDuration = 0.55f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RoughHouse|Presentation")
    TObjectPtr<USkeletalMeshComponent> CharacterMesh;

private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UTextRenderComponent> NameLabel;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> BodyBlock;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> HeadBlock;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeftArmBlock;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> RightArmBlock;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeftLegBlock;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> RightLegBlock;
    UPROPERTY() TObjectPtr<ARHFighter> Opponent;
    UPROPERTY() TObjectPtr<USkeletalMesh> MannyMesh;
    UPROPERTY() TObjectPtr<USkeletalMesh> QuinnMesh;
    UPROPERTY() TObjectPtr<UAnimSequence> IdleAnimation;
    UPROPERTY() TObjectPtr<UAnimSequence> WalkAnimation;
    UPROPERTY() TObjectPtr<UAnimSequence> PunchOneAnimation;
    UPROPERTY() TObjectPtr<UAnimSequence> PunchTwoAnimation;
    UPROPERTY() TObjectPtr<UAnimSequence> HitReactionAnimation;

    FName FighterName = TEXT("Fighter");
    float AttackRemaining = 0.0f;
    float HitRemaining = 0.0f;
    bool bDamageApplied = false;
    bool bMoving = false;
    bool bPlayingLocomotion = false;
    int32 PunchCount = 0;

    void PlayLocomotion();
    void ResetBlockingPose();
};
