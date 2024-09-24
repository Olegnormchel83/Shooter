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

	UpdateInventory(WeaponSlots);

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

bool UTPSInventoryComponent::SwitchWeaponToNextPreviousIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
	{ 
		CorrectIndex = 0;
	}
	else
	{
		if (ChangeToIndex < 0)
		{
			CorrectIndex = WeaponSlots.Num() - 1;
		}
	}


	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
			{
				bIsSuccess = true;
			}
			else
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
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
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
				NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
			}
		}
	}
	if (!bIsSuccess)
	{
		int8 iteration = 0;
		int8 Seconditeration = 0;
		int8 tmpIndex = 0;
		while (iteration < WeaponSlots.Num() && !bIsSuccess)
		{
			iteration++;

			if (bIsForward)
			{
				tmpIndex = ChangeToIndex + iteration;
			}
			else
			{
				Seconditeration = WeaponSlots.Num() - 1;

				tmpIndex = ChangeToIndex - iteration;
			}

			if (WeaponSlots.IsValidIndex(tmpIndex))
			{
				if (!WeaponSlots[tmpIndex].NameItem.IsNone())
				{
					if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
					{
						bIsSuccess = true;
						NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
						NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
						NewCurrentIndex = tmpIndex;
					}
					else
					{
						FWeaponInfo myInfo;
						UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

						myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

						bool bIsFind = false;
						int8 j = 0;
						while (j < AmmoSlots.Num() && !bIsFind)
						{
							if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
							{
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
								NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
								NewCurrentIndex = tmpIndex;
								bIsFind = true;
							}
							j++;
						}
					}
				}
			}
			else
			{
				if (OldIndex != Seconditeration)
				{
					if (WeaponSlots.IsValidIndex(Seconditeration))
					{
						if (!WeaponSlots[Seconditeration].NameItem.IsNone())
						{
							if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
							{
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
								NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
								NewCurrentIndex = Seconditeration;
							}
							else
							{
								FWeaponInfo myInfo;
								UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

								myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
									{
										bIsSuccess = true;
										NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
										NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
										NewCurrentIndex = Seconditeration;
										bIsFind = true;
									}
									j++;
								}
							}
						}
					}
				}
				else
				{
					if (WeaponSlots.IsValidIndex(Seconditeration))
					{
						if (!WeaponSlots[Seconditeration].NameItem.IsNone())
						{
							if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
							{
								//WeaponGood, it same weapon do nothing
							}
							else
							{
								FWeaponInfo myInfo;
								UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

								myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
									{
										if (AmmoSlots[j].Cout > 0)
										{
											//WeaponGood, it same weapon do nothing
										}
										else
										{
											UE_LOG(LogTemp, Error, TEXT("UTPSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
										}
									}
									j++;
								}
							}
						}
					}
				}
				if (bIsForward)
				{
					Seconditeration++;
				}
				else
				{
					Seconditeration--;
				}

			}
		}
	}


	if (bIsSuccess)
	{
		
		UE_LOG(LogTemp, Display, TEXT("Weapon Switched In Inventory"));
		UE_LOG(LogTemp, Display, TEXT("Old Index: %i"), OldIndex);
		UE_LOG(LogTemp, Display, TEXT("New Index: %i"), NewCurrentIndex);
		UE_LOG(LogTemp, Display, TEXT("Current Weapon Name: %s"), *WeaponSlots[NewCurrentIndex].NameItem.ToString());
		
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
		OnSwitchWeaponInInventory.Broadcast(NewCurrentIndex);
	}

	return bIsSuccess;
}

bool UTPSInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo)
{
	bool bIsSuccess = false;
	FName ToSwitchIdWeapon;
	FAdditionalWeaponInfo ToSwitchAdditiobnalInfo;

	ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	ToSwitchAdditiobnalInfo = GetAdditionalWeaponInfo(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviousIndex, PreviousWeaponInfo);
		OnSwitchWeapon.Broadcast(ToSwitchIdWeapon, ToSwitchAdditiobnalInfo, IndexWeaponToChange);

		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AvailableAmmoForWeapon = -1;
			if (CheckAmmoForWeapon(ToSwitchWeaponType, AvailableAmmoForWeapon))
			{

			}
		}
		bIsSuccess = true;
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

bool UTPSInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;

	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			myGI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}

	return bIsFind;
}

bool UTPSInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	UTPSGameInstance* myGI = Cast<UTPSGameInstance>(GetWorld()->GetGameInstance());

	if (myGI)
	{
		myGI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}

	return bIsFind;
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

		SwitchWeaponToNextPreviousIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

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


void UTPSInventoryComponent::UpdateInventory(TArray<FWeaponSlot> Weapons)
{
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
			}
			
		}
	}
}

void UTPSInventoryComponent::DropWeaponByIndex(int32 ByIndex, FDropItem& DropItemInfo)
{
	FWeaponSlot EmptyWeaponSlot;

	bool bIsCanDrop = false;
	int8 i = 0;
	int8 AvailableWeaponNum = 0;

	while (i < WeaponSlots.Num() && !bIsCanDrop)
	{
		if (!WeaponSlots[i].NameItem.IsNone())
		{
			AvailableWeaponNum++;
			if (AvailableWeaponNum > 1)
			{
				bIsCanDrop = true;
			}
		}
		i++;
	}

	if (bIsCanDrop && 
		WeaponSlots.IsValidIndex(ByIndex) && 
		GetDropItemInfoFromInventory(ByIndex, DropItemInfo))
	{
		GetDropItemInfoFromInventory(ByIndex, DropItemInfo);

		bool bIsFindWeapon = false;
		int8 j = 0;

		while (j < WeaponSlots.Num() && !bIsFindWeapon)
		{
			if (!WeaponSlots[j].NameItem.IsNone())
			{
				OnSwitchWeapon.Broadcast(WeaponSlots[j].NameItem, WeaponSlots[j].AdditionalInfo, j);
			}
			j++;
		}

		WeaponSlots[ByIndex] = EmptyWeaponSlot;
		if (GetOwner()->GetClass()->ImplementsInterface(UTPS_IGameActor::StaticClass()))
		{
			ITPS_IGameActor::Execute_DropWeaponToWorld(GetOwner(), DropItemInfo);
		}
		
		FAdditionalWeaponInfo PreviousWeaponInfo = WeaponSlots[ByIndex].AdditionalInfo;
		SwitchWeaponToNextPreviousIndex(ByIndex + 1, ByIndex, PreviousWeaponInfo, true);

		OnUpdateWeaponSlots.Broadcast(ByIndex, EmptyWeaponSlot);
	}
}

#pragma optimize("", on)