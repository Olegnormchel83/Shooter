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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHaveRound, int32, IndexSlotWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHaveNotRound, int32, IndexSlotWeapon);

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

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponHaveRound OnWeaponHaveRound;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FOnWeaponHaveNotRound OnWeaponHaveNotRound;

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

	bool SwitchWeaponToNextPreviousIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward);
	bool SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo);

	FAdditionalWeaponInfo GetAdditionalWeaponInfo(int32 WeaponIndex);
	int32 GetWeaponIndexSlotByName(FName IdWeaponName);
	bool GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType &WeaponType);
	bool GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType &WeaponType);

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

	UFUNCTION(BlueprintCallable)
	void DropWeaponByIndex(int32 ByIndex, FDropItem &DropItemInfo);

};
