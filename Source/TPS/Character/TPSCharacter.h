// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "Interfaces/TPS_IGameActor.h"
#include "StateEffects/TPS_StateEffect.h"

#include "TPSCharacter.generated.h"

class AWeaponDefault;
class UTPSInventoryComponent;
class UTPSCharacterHealthComponent;

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter, public ITPS_IGameActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	ATPSCharacter();

	FTimerHandle TimerHadle_RagdollTimer;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	//FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:

#pragma region Cursor

	//VARIABLES

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UDecalComponent* CurrentCursor = nullptr;

	//FUNCTIONS

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();

#pragma endregion

#pragma region Movement

	//VARIABLES

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementSpeedInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float StaminaPoints = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentStamina = StaminaPoints;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MinusStamina = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "0", ClampMax = "100"))
	float PlusStamina = 0.5f;

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	//flags

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool Stunned = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SprintRunEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool WalkEnabled = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool AimEnabled = false;

	//FUNCTIONS

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();
	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();
	UFUNCTION(BlueprintCallable)
	void DecreaseStamina();
	UFUNCTION(BlueprintCallable)
	void IncreaseStamina();

	UFUNCTION()
	void InputAxisY(float Value);
	UFUNCTION()
	void InputAxisX(float Value);
	UFUNCTION()
	void MovementTick(float DeltaTime);
	

#pragma endregion

#pragma region Health

	//VARIABLES

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSCharacterHealthComponent* HealthComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	TArray<UAnimMontage*>  DeadAnims;

	//flags

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsAlive = true;

	//FUNCTIONS

	UFUNCTION()
	void CharDead();
	UFUNCTION()
	void EnableRagdoll();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

#pragma endregion

#pragma region Inventory

	//VARIABLES

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSInventoryComponent* InventoryComponent = nullptr;
	
	//FUNCTIONS

	UFUNCTION(BlueprintCallable)
	void TrySwitchNextWeapon();
	UFUNCTION(BlueprintCallable)
	void TrySwitchPreviousWeapon();

#pragma endregion

#pragma region Weapon

	//VARIABLES

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FName InitWeaponName;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	AWeaponDefault* CurrentWeapon = nullptr;

	//FUNCTIONS

	UFUNCTION(BlueprintCallable)
	void InitWeapon(
		FName IdWepaon, 
		FAdditionalWeaponInfo AdditionalWeaponInfo, 
		int32 NewCurrentIndexWeapon);

	UFUNCTION(BlueprintCallable)
	void RemoveCurrentWeapon();
	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();
	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentWeaponIndex();
	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);
	UFUNCTION(BlueprintNativeEvent)
	void CharFire_BP(UAnimMontage* Anim);

	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake);
	UFUNCTION()
	void InputAttackPressed();
	UFUNCTION()
	void InputAttackReleased();
	UFUNCTION()
	void CharFire(UAnimMontage* Anim);

#pragma endregion

#pragma region StateEffects

	//VARIABLES

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UAnimMontage* StunAnimation = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTPS_StateEffect> AbilityEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FVector ParticleOffset = FVector(0.0f, 0.0f, 100.0f);

	TArray<UTPS_StateEffect*> Effects;

	//FUNCTIONS

	UFUNCTION()
	void GetStun();
	UFUNCTION()
	void StunOut();

	void TryAbilityEnabled();

#pragma endregion

#pragma region Interface

	//FUNCTIONS

	FVector GetParticleOffset() override;
	EPhysicalSurface GetSurfaceType() override;
	TArray<UTPS_StateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UTPS_StateEffect* RemovedEffect) override;
	void AddEffect(UTPS_StateEffect* NewEffect) override;

#pragma endregion

};