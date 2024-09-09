// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types.h"

#include "TPSInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSwitchWeapon, FName, WeaponIdName, FAdditionalWeaponInfo, WeaponAdditionalInfo, int32, NewCurrentIndexWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChange, EWeaponType, TypeAmmo, int32, Cout);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAdditionalInfoChange, int32, IndexSlot, FAdditionalWeaponInfo, AdditionalInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoEmpty, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAmmoAvailable, EWeaponType, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateWeaponSlots, int32, IndexSlotChange, FWeaponSlot, NewInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWeaponInInventory, int32, NewIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_API UTPSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSInventoryComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnSwitchWeapon OnSwitchWeapon;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnAmmoChange OnAmmoChange;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponAdditionalInfoChange OnWeaponAdditionalInfoChange;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponAmmoEmpty OnWeaponAmmoEmpty;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponAmmoAvailable OnWeaponAmmoAvailable;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnUpdateWeaponSlots OnUpdateWeaponSlots;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnSwitchWeaponInInventory OnSwitchWeaponInInventory;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FWeaponSlot> WeaponSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	TArray<FAmmoSlot> AmmoSlots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons")
	int32 MaxSlotsWeapon = 0;

	bool SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);

	FAdditionalWeaponInfo GetAdditionalWeaponInfo(int32 WeaponIndex);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);

	UFUNCTION(BlueprintCallable)
	FName GetWeaponNameBySlotIndex(int32 IndexSlot);

	void SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo);

	UFUNCTION(BlueprintCallable)
	void AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo);

	bool CheckAmmoForWeapon(EWeaponType TypeWeapon, int8 &AvailableAmmoForWeapon);

	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool CheckCanTakeAmmo(EWeaponType AmmoType);
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool CheckCanTakeWeapon(int32 &FreeSlot);
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem &DropItemInfo);
	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool TryGetWeaponToInventory(FWeaponSlot NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Interface")
	bool GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem &DropItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Interface")
	void SaveItemToInventory();

	UFUNCTION(BlueprintCallable)
	void UpdateInventory(TArray<FWeaponSlot> Weapons);

private:

#pragma region SwitchWeaponInInventoryHelpFunctions
	void SelectNewWeapon(
		int8 tmpIndex,
		bool bIsSuccess,
		FName NewIdWeapon,
		FAdditionalWeaponInfo NewAdditionalWeaponInfo,
		int32 NewCurrentIndex);

	void ForwardSwitch(
		bool bIsSuccess,
		int32 ChangeToIndex,
		FName NewIdWeapon,
		FAdditionalWeaponInfo NewAdditionalWeaponInfo,
		int32 NewCurrentIndex);
	void PreviousSwitch(
		bool bIsSuccess,
		int32 ChangeToIndex,
		FName NewIdWeapon,
		FAdditionalWeaponInfo NewAdditionalWeaponInfo,
		int32 NewCurrentIndex);
};
#pragma endregion