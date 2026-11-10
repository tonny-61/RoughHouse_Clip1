#include "RHArenaPawn.h"
#include "RHFighter.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
ARHArenaPawn::ARHArenaPawn()
{
 PrimaryActorTick.bCanEverTick=true;
 SceneRoot=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));SetRootComponent(SceneRoot);
 Camera=CreateDefaultSubobject<UCameraComponent>(TEXT("FightCamera"));Camera->SetupAttachment(SceneRoot);
 Camera->FieldOfView=58.f;
 ArenaFloor=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArenaFloor"));ArenaFloor->SetupAttachment(SceneRoot);
 static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
 if(Cube.Succeeded()) ArenaFloor->SetStaticMesh(Cube.Object);
 ArenaFloor->SetRelativeLocation(FVector(0,0,-15));ArenaFloor->SetRelativeScale3D(FVector(18,18,.25f));
 ArenaFloor->SetCollisionEnabled(ECollisionEnabled::NoCollision);

 auto Wall=[this](const TCHAR* Name,const FVector& Location,const FVector& Scale)->UStaticMeshComponent*
 {
  UStaticMeshComponent* Component=CreateDefaultSubobject<UStaticMeshComponent>(Name);
  Component->SetupAttachment(SceneRoot);
  if(Cube.Succeeded()) Component->SetStaticMesh(Cube.Object);
  Component->SetRelativeLocation(Location);
  Component->SetRelativeScale3D(Scale);
  Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  return Component;
 };
 BackWall=Wall(TEXT("BackWall"),FVector(480,0,215),FVector(.3f,18.f,2.4f));
 LeftWall=Wall(TEXT("LeftWall"),FVector(0,-890,90),FVector(9.f,.25f,1.1f));
 RightWall=Wall(TEXT("RightWall"),FVector(0,890,90),FVector(9.f,.25f,1.1f));

 KeyLight=CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("KeyLight"));
 KeyLight->SetupAttachment(SceneRoot);
 KeyLight->SetRelativeRotation(FRotator(-52.f,-35.f,0.f));
 KeyLight->SetIntensity(7.5f);

 FillLight=CreateDefaultSubobject<USkyLightComponent>(TEXT("FillLight"));
 FillLight->SetupAttachment(SceneRoot);
 FillLight->SetIntensity(1.3f);

 RimLight=CreateDefaultSubobject<UPointLightComponent>(TEXT("RimLight"));
 RimLight->SetupAttachment(SceneRoot);
 RimLight->SetRelativeLocation(FVector(180.f,0.f,360.f));
 RimLight->SetIntensity(7000.f);
 RimLight->SetAttenuationRadius(1600.f);
 RimLight->SetLightColor(FLinearColor(.2f,.45f,1.f));
 AutoPossessPlayer=EAutoReceiveInput::Player0;
}
void ARHArenaPawn::BeginPlay()
{
 Super::BeginPlay();
 if(!GetWorld()) return;
 FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 Soho=GetWorld()->SpawnActor<ARHFighter>(ARHFighter::StaticClass(),FVector(-110,0,0),FRotator::ZeroRotator,Params);
 Slick=GetWorld()->SpawnActor<ARHFighter>(ARHFighter::StaticClass(),FVector(110,0,0),FRotator::ZeroRotator,Params);
 if(Soho && Slick)
 {
  Soho->ConfigureFighter(TEXT("SOHO"),true);Slick->ConfigureFighter(TEXT("SLICK"),false);
  Soho->SetOpponent(Slick);Slick->SetOpponent(Soho);
 }
}
void ARHArenaPawn::Tick(float Dt)
{
 Super::Tick(Dt);
 if(!Soho || !Slick) return;
 float Fwd=ForwardAxis, Side=SideAxis;
 if(bAutoDemo)
 {
  DemoElapsed+=Dt;
  // 25-second repeatable blocking pass: circle, close distance, demonstrate punches.
  const float T=FMath::Fmod(DemoElapsed,25.f);
  Side=(T<9.f)?0.8f:((T<15.f)?-0.65f:0.3f);
  Fwd=(T<3.f)?0.5f:((T<9.f)?0.f:((T<15.f)?0.3f:0.f));
  if(T>3.f && T<23.f && DemoElapsed-LastDemoPunch>1.25f)
  {
   Attack();LastDemoPunch=DemoElapsed;
  }
 }
 FVector ToEnemy=Slick->GetActorLocation()-Soho->GetActorLocation();ToEnemy.Z=0;
 const FVector Forward=ToEnemy.GetSafeNormal();
 const FVector Right=FVector::CrossProduct(FVector::UpVector,Forward);
 const FVector Input=(Forward*Fwd+Right*Side).GetClampedToMaxSize(1.f);
 Soho->SetMoving(!Input.IsNearlyZero());
 FVector Next=Soho->GetActorLocation()+Input*WalkSpeed*Dt;
 Next.X=FMath::Clamp(Next.X,-750.f,750.f);Next.Y=FMath::Clamp(Next.Y,-750.f,750.f);
 if(FVector::Dist2D(Next,Slick->GetActorLocation())>=MinimumSeparation) Soho->SetActorLocation(Next);
 const FVector Mid=(Soho->GetActorLocation()+Slick->GetActorLocation())*.5f;
 const float Distance=FVector::Dist2D(Soho->GetActorLocation(),Slick->GetActorLocation());
 const FVector Desired=Mid+FVector(-FMath::Max(530.f,Distance*1.45f),-660.f,380.f);
 Camera->SetWorldLocation(FMath::VInterpTo(Camera->GetComponentLocation(),Desired,Dt,CameraSmoothing));
 Camera->SetWorldRotation(FMath::RInterpTo(Camera->GetComponentRotation(),UKismetMathLibrary::FindLookAtRotation(Camera->GetComponentLocation(),Mid+FVector(0,0,115)),Dt,CameraSmoothing));
}
void ARHArenaPawn::SetupPlayerInputComponent(UInputComponent* Input)
{
 Super::SetupPlayerInputComponent(Input);
 Input->BindAxis(TEXT("MoveForward"),this,&ARHArenaPawn::MoveForward);
 Input->BindAxis(TEXT("MoveSide"),this,&ARHArenaPawn::MoveSide);
 Input->BindAction(TEXT("Punch"),IE_Pressed,this,&ARHArenaPawn::Attack);
}
void ARHArenaPawn::Attack(){if(Soho) Soho->Punch();}
