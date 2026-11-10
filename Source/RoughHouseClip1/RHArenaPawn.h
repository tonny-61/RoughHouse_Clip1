#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RHArenaPawn.generated.h"
class UCameraComponent;
class UDirectionalLightComponent;
class UPointLightComponent;
class USkyLightComponent;
class UStaticMeshComponent;
class ARHFighter;
UCLASS()
class ROUGHHOUSECLIP1_API ARHArenaPawn : public APawn
{
 GENERATED_BODY()
public:
 ARHArenaPawn();
 virtual void BeginPlay() override;
 virtual void Tick(float DeltaSeconds) override;
 virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
 UPROPERTY(EditAnywhere, Category="RoughHouse|Movement") float WalkSpeed=240.f;
 UPROPERTY(EditAnywhere, Category="RoughHouse|Movement") float MinimumSeparation=125.f;
 UPROPERTY(EditAnywhere, Category="RoughHouse|Camera") float CameraSmoothing=4.f;
 UPROPERTY(EditAnywhere, Category="RoughHouse|Demo") bool bAutoDemo=true;
private:
 UPROPERTY(VisibleAnywhere) TObjectPtr<USceneComponent> SceneRoot;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> Camera;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> ArenaFloor;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> BackWall;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeftWall;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> RightWall;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UDirectionalLightComponent> KeyLight;
 UPROPERTY(VisibleAnywhere) TObjectPtr<USkyLightComponent> FillLight;
 UPROPERTY(VisibleAnywhere) TObjectPtr<UPointLightComponent> RimLight;
 UPROPERTY() TObjectPtr<ARHFighter> Soho;
 UPROPERTY() TObjectPtr<ARHFighter> Slick;
 float ForwardAxis=0.f, SideAxis=0.f, DemoElapsed=0.f, LastDemoPunch=-10.f;
 void MoveForward(float Value){ForwardAxis=Value;}
 void MoveSide(float Value){SideAxis=Value;}
 void Attack();
};
