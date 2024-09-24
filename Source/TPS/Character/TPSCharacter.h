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

	//Inputs
	void InputAxisY(float Value);
	void InputAxisX(float Value);

	void InputAttackPressed();
	void InputAttackReleased();

	void InputWalkPressed();
	void InputWalkReleased();

	void InputSprintPressed();
	void InputSprintReleased();

	void InputAimPressed();
	void InputAimReleased();

	//Inventory inputs
	void TrySwitchNextWeapon();
	void TrySwitchPreviousWeapon();

	//Ability inputs
	void TryAbilityEnabled();

	template<int32 Id>
	void TKeyPressed()
	{
		TrySwitchWeaponToIndexByKeyInput(Id);
	}
	//Inputs End

	//Input flags
	float AxisX = 0.0f;
	float AxisY = 0.0f;
	float MoveSpeedCoef = 1.0f;

	float MaxStaminaPoints = 100.0f;
	float CurrentStamina = MaxStaminaPoints;
	float MinusStamina = 1.0f;
	float PlusStamina = 0.5f;

	bool Stunned = false;
	bool SprintRunEnabled = false;
	bool WalkEnabled = false;
	bool AimEnabled = false;

	bool bIsAlive = true;

	EMovementState MovementState = EMovementState::Run_State;
	AWeaponDefault* CurrentWeapon = nullptr;
	UDecalComponent* CurrentCursor = nullptr;

	TArray<UTPS_StateEffect*> Effects;

	int32 CurrentIndexWeapon = 0;

	UFUNCTION()
	void CharDead();
	void EnableRagdoll();

	void DecreaseStamina();
	void IncreaseStamina();

	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser) override;

public:

	ATPSCharacter();

	FTimerHandle TimerHadle_RagdollTimer;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent) override;

	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSInventoryComponent* InventoryComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSCharacterHealthComponent* HealthComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementSpeedInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	TArray<UAnimMontage*>  DeadAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTPS_StateEffect> AbilityEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UAnimMontage* StunAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	FVector ParticleOffset = FVector(0.0f, 0.0f, 100.0f);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FName InitWeaponName;

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);
	UFUNCTION(BlueprintNativeEvent)
	void CharFire_BP(UAnimMontage* Anim);

	//Tick func
	UFUNCTION()
	void MovementTick(float DeltaTime);
	//Tick func end

	//Func
	void CharacterUpdate();
	void ChangeMovementState();

	void GetStun();
	void StunOut();

	void AttackCharEvent(bool bIsFiring);

	UFUNCTION()
	void InitWeapon(
		FName IdWepaon,
		FAdditionalWeaponInfo AdditionalWeaponInfo,
		int32 NewCurrentIndexWeapon);
	void TryReloadWeapon();
	UFUNCTION()
	void WeaponFireStart(UAnimMontage* Anim);
	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);
	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake);

	bool TrySwitchWeaponToIndexByKeyInput(int32 ToIndex);
	void RemoveCurrentWeapon();
	void DropCurrentWeapon();

#pragma region Getters

	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon() 
	{ 
		return CurrentWeapon; 
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentWeaponIndex()
	{
		return CurrentIndexWeapon;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EMovementState GetCurrentMovementState()
	{
		return MovementState;
	}

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld()
	{
		return CurrentCursor;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAiming()
	{
		return AimEnabled;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetCurrentStamina()
	{
		return CurrentStamina;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetStaminaPoints()
	{
		return MaxStaminaPoints;
	}

	bool IsAlive() 
	{ 
		return bIsAlive; 
	}

	bool IsStunned() 
	{
		return Stunned;
	}


#pragma endregion



	//Interface
	FVector GetParticleOffset() override;
	EPhysicalSurface GetSurfaceType() override;
	TArray<UTPS_StateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UTPS_StateEffect* RemovedEffect) override;
	void AddEffect(UTPS_StateEffect* NewEffect) override;
	//Interface end
};