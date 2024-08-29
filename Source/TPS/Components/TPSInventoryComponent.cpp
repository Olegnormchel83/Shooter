// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/TPSInventoryComponent.h"
#include "Game/TPSGameInstance.h"
#include "TPSCharacter.h"
#include "Kismet/GameplayStatics.h"

#pragma optimize("", off)

// Sets default values for this component's properties
UTPSInventoryComponent::UTPSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTPSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (int8 i = 0; i < WeaponSlots.Num(); i++)
	{
		UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
		if (myGI)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				FWeaponInfo Info;
				if (myGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
				{
					WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
				}
				else
				{
					//WeaponSlots.RemoveAt(i);
					//i--;
				}
			}
		}
	}
	

	MaxSlotsWeapon = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
		{
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
		}
	}
}

void UTPSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UTPSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
	{
		CorrectIndex = WeaponSlots.Num() - 1;
		return false;
	}
	else
	{
		if (ChangeToIndex < 0)
		{
			CorrectIndex = 0;
			return false;
		}
	}

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalWeaponInfo;
	int32 NewCurrentIndex = 0;

	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
			if (myGI)
			{
				FWeaponInfo myInfo;
				myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);

				bool bIsFind = false;
				int8 j = 0;
				while (j < AmmoSlots.Num() && !bIsFind)
				{
					if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
					{
						bIsSuccess = true;
						bIsFind = true;
					}
					j++;
				}
			}
		}

		if (bIsSuccess)
		{
			NewCurrentIndex = CorrectIndex;
			NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
			NewAdditionalWeaponInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
		}
	}

	if (!bIsSuccess)
	{
		if (bIsForward)
		{
			ForwardSwitch(
				bIsSuccess, 
				ChangeToIndex, 
				NewIdWeapon, 
				NewAdditionalWeaponInfo, 
				NewCurrentIndex);
		}
		else
		{
			PreviousSwitch(
				bIsSuccess, 
				ChangeToIndex, 
				NewIdWeapon, 
				NewAdditionalWeaponInfo, 
				NewCurrentIndex);
		}
	}

	if (bIsSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Switched In Inventory"));
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalWeaponInfo, NewCurrentIndex);
		OnSwitchWeaponInInventory.Broadcast(NewCurrentIndex);
	}

	return bIsSuccess;
}

FAdditionalWeaponInfo UTPSInventoryComponent::GetAdditionalWeaponInfo(int32 WeaponIndex)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(WeaponIndex))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == WeaponIndex)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - No found weapon with index - %d"), WeaponIndex);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - Not correct index weapon - %d"), WeaponIndex);
	}

	return result;
}

int32 UTPSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;

	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i;
		}
		i++;
	}

	return result;
}

FName UTPSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;

	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		result = WeaponSlots[IndexSlot].NameItem;
	}

	return result;
}

void UTPSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;

				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - No found weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTPSInventoryComponent::SetAdditionalInfoWeapon - Not correct index weapon - %d"), IndexWeapon);
	}
}

void UTPSInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutChangeAmmo;
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
			{
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			}

			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);
			bIsFind = true;
		}
		i++;
	}
}

bool UTPSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AvailableAmmoForWeapon)
{
	AvailableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AvailableAmmoForWeapon = AmmoSlots[i].Cout;
			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
		}
		i++;
	}

	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);

	return false;
}

bool UTPSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
		{
			result = true;
		}
		i++;
	}

	return result;
}

bool UTPSInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}

	return bIsFreeSlot;
}

bool UTPSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	bool result = false;

	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;

		SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
		OnSwitchWeaponInInventory.Broadcast(CurrentIndexWeaponChar);
		result = true;
	}

	UE_LOG(LogTemp, Display, TEXT("Weapon Switched"));

	return result;
}

bool UTPSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	const auto Player = Cast<ATPSCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	int32 IndexSlot = -1;
	if (CheckCanTakeWeapon(IndexSlot))
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			WeaponSlots[IndexSlot] = NewWeapon;

			OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
			OnSwitchWeaponInInventory.Broadcast(Player->GetCurrentWeaponIndex());
			return true;
		}
	}
	return false;
}

bool UTPSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;

	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		result = myGI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
			DropItemInfo.WeaponInfo.NameItem = WeaponSlots[IndexSlot].NameItem;
		}
	}

	return result;
}

void UTPSInventoryComponent::SaveItemToInventory()
{

}

void UTPSInventoryComponent::SelectNewWeapon
(int8 tmpIndex, 
	bool bIsSuccess, 
	FName NewIdWeapon, 
	FAdditionalWeaponInfo NewAdditionalWeaponInfo,
	int32 NewCurrentIndex)
{
	if (tmpIndex > WeaponSlots.Num() - 1)
	{
		tmpIndex = WeaponSlots.Num() - 1;
	}
	else
	{
		if (tmpIndex < 0)
		{
			tmpIndex = 0;
		}
	}

	if (WeaponSlots[tmpIndex].NameItem != "None")
	{
		FWeaponInfo myInfo;
		UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
		myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

		bool bIsFind = false;
		int8 j = 0;
		while (j < AmmoSlots.Num() && !bIsFind)
		{
			if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
			{
				bIsSuccess = true;
				NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
				NewAdditionalWeaponInfo = WeaponSlots[tmpIndex].AdditionalInfo;
				NewCurrentIndex = tmpIndex;
				bIsFind = true;
			}
			j++;
		}
	}
}

void UTPSInventoryComponent::ForwardSwitch(
	bool bIsSuccess, 
	int32 ChangeToIndex, 
	FName NewIdWeapon,
	FAdditionalWeaponInfo NewAdditionalWeaponInfo,
	int32 NewCurrentIndex)
{
	int8 iteration = 0;
	int8 Seconditeration = 0;
	while (iteration < WeaponSlots.Num() && !bIsSuccess)
	{
		iteration++;
		int8 tmpIndex = ChangeToIndex + iteration;
		if (WeaponSlots.IsValidIndex(tmpIndex))
		{
			SelectNewWeapon(tmpIndex, bIsSuccess, NewIdWeapon, NewAdditionalWeaponInfo, NewCurrentIndex);
		}
		else
		{
			if (WeaponSlots.IsValidIndex(Seconditeration))
			{
				SelectNewWeapon(tmpIndex, bIsSuccess, NewIdWeapon, NewAdditionalWeaponInfo, NewCurrentIndex);
			}
			Seconditeration++;
		}
	}
}

void UTPSInventoryComponent::PreviousSwitch(
	bool bIsSuccess,
	int32 ChangeToIndex,
	FName NewIdWeapon,
	FAdditionalWeaponInfo NewAdditionalWeaponInfo,
	int32 NewCurrentIndex)
{
	int8 iteration = 0;
	int8 Seconditeration = WeaponSlots.Num() - 1;
	while (iteration < WeaponSlots.Num() && !bIsSuccess)
	{
		iteration++;
		int8 tmpIndex = ChangeToIndex - iteration;
		if (WeaponSlots.IsValidIndex(tmpIndex))
		{
			SelectNewWeapon(tmpIndex, bIsSuccess, NewIdWeapon, NewAdditionalWeaponInfo, NewCurrentIndex);
		}
		else
		{
			if (WeaponSlots.IsValidIndex(Seconditeration))
			{
				SelectNewWeapon(tmpIndex, bIsSuccess, NewIdWeapon, NewAdditionalWeaponInfo, NewCurrentIndex);
			}
			Seconditeration--;
		}
	}
}

#pragma optimize("", on)