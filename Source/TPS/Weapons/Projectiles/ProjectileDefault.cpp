// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	BulletCollisionSphere->SetSphereRadius(8.f);
	BulletCollisionSphere->bReturnMaterialOnMove = true;
	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projectile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet Projectile Movement"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	BulletProjectileMovement->InitialSpeed = 1.f;
	BulletProjectileMovement->MaxSpeed = 0.f;
	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();
	
	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);
}

// Called every frame
void AProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	ProjectileSettings = InitParam;

	BulletProjectileMovement->InitialSpeed = InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = InitParam.ProjectileInitSpeed * 2;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);

	if (InitParam.ProjectileStaticMesh)
	{
		BulletMesh->SetStaticMesh(InitParam.ProjectileStaticMesh);
		BulletMesh->SetRelativeTransform(InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFx)
	{
		BulletFX->SetTemplate(InitParam.ProjectileTrailFx);
		BulletFX->SetRelativeTransform(InitParam.PorjectileTrailFxOffset);
	}
	else
	{
		BulletFX->DestroyComponent();
	}

	/*
	UE_LOG(LogTemp, Warning, TEXT("_____InitParam_____"));
	UE_LOG(LogTemp, Warning, TEXT("Init Speed: %.2f"), BulletProjectileMovement->InitialSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Max Speed: %.2f"), BulletProjectileMovement->MaxSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Projectile Lifetime: %.2f"), this->GetLifeSpan());
	*/

}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		if (ProjectileSettings.HitDecals.Contains(mySurfaceType))
		{
			UMaterialInterface* myMaterial = ProjectileSettings.HitDecals[mySurfaceType];

			if (myMaterial && OtherComp)
			{
				UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), OtherComp, 
					NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
			}
		}
		if (ProjectileSettings.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSettings.HitFXs[mySurfaceType];
			if (myParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, 
					FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(2.0f)));
			}
		}
		if (ProjectileSettings.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.HitSound, Hit.ImpactPoint);
		}
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSettings.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);

	ImpactProjectile();
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AProjectileDefault::ImpactProjectile()
{
	this->Destroy();
}

