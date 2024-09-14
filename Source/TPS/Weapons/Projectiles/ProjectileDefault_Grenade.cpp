// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectiles/ProjectileDefault_Grenade.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/TPS_IGameActor.h"

int32 DebugExplodeShow = 0;
FAutoConsoleVariableRef CVARExplodeShow(
TEXT("TPS.DebugExplode"), 
DebugExplodeShow, 
TEXT("DrawDebugForExplode"), 
ECVF_Cheat);

void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);
}

void AProjectileDefault_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExplose)
		{
			Explose();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::InitProjectile(FProjectileInfo InitParam)
{
	Super::InitProjectile(InitParam);
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	ImpactProjectile();
	if (ProjectileSettings.PreExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.PreExploseSound, Hit.ImpactPoint);
		ProjectileSettings.PreExploseSound = nullptr;
	}
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explose()
{
	if (DebugExplodeShow)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSettings.ProjectileMaxRadiusDamage, 12, FColor::Red, false, 12.0f);
	}

	TimerEnabled = false;

	if (ProjectileSettings.ExploseFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSettings.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}

	if (ProjectileSettings.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSettings.ExploseSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;

	/*
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), 
		ProjectileSettings.ExploseMaxDamage,
		ProjectileSettings.ExploseMaxDamage*0.2, 
		GetActorLocation(),
		1000.0f,
		2000.0f,
		5,
		NULL, IgnoredActor, this, nullptr);
		*/
	
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(), 
		ProjectileSettings.ExploseMaxDamage, 
		GetActorLocation(), 
		ProjectileSettings.ProjectileMaxRadiusDamage, 
		DamageType,
		IgnoredActor,
		this,
		nullptr,
		true);
		

	this->Destroy();
}
