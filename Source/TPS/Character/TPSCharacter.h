// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Types.h"
#include "Weapons/WeaponDefault.h"

#include "TPSCharacter.generated.h"

class UTPSInventoryComponent;
class UTPSCharacterHealthComponent;

UCLASS(Blueprintable)
class ATPSCharacter : public ACharacter
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

	/** A decal that projects to the cursor location. */
	/*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
	*/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EMovementState MovementState = EMovementState::Run_State;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementSpeedInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool SprintRunEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool WalkEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool AimEnabled = false;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float StaminaPoints = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float CurrentStamina = StaminaPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MinusStamina = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement", meta = (ClampMin = "0", ClampMax = "100"))
	float PlusStamina = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	bool bIsAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	TArray<UAnimMontage*>  DeadAnims;

	AWeaponDefault* CurrentWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Demo")
	FName InitWeaponName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 CurrentIndexWeapon = 0;

	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UTPSCharacterHealthComponent* HealthComponent = nullptr;

	UFUNCTION(BlueprintCallable)
	UDecalComponent* GetCursorToWorld();

	UFUNCTION()
	void InputAxisY(float Value);

	UFUNCTION()
	void InputAxisX(float Value);

	UFUNCTION()
	void InputAttackPressed();

	UFUNCTION()
	void InputAttackReleased();

	float AxisX = 0.0f;
	float AxisY = 0.0f;

	//TickFunction
	UFUNCTION()
	void MovementTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void CharacterUpdate();

	UFUNCTION(BlueprintCallable)
	void ChangeMovementState();

	UFUNCTION(BlueprintCallable)
	void DecreaseStamina();

	UFUNCTION(BlueprintCallable)
	void IncreaseStamina();

	UFUNCTION(BlueprintCallable)
	void AttackCharEvent(bool bIsFiring);

	UFUNCTION(BlueprintCallable)
	AWeaponDefault* GetCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentWeaponIndex();

	UFUNCTION(BlueprintCallable)
	void InitWeapon(FName IdWepaon, FAdditionalWeaponInfo AdditionalWeaponInfo, int32 NewCurrentIndexWeapon);

	UFUNCTION(BlueprintCallable)
	void RemoveCurrentWeapon();

	UFUNCTION(BlueprintCallable)
	void TryReloadWeapon();

	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);

	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);

	UFUNCTION()
	void CharFire(UAnimMontage* Anim);

	UFUNCTION(BlueprintNativeEvent)
	void CharFire_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintCallable)
	void TrySwitchNextWeapon();

	UFUNCTION(BlueprintCallable)
	void TrySwitchPreviousWeapon();

	UFUNCTION()
	void CharDead();

	UFUNCTION()
	void EnableRagdoll();

	virtual float TakeDamage(
		float DamageAmount, 
		struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, 
		AActor* DamageCauser) override;
};

