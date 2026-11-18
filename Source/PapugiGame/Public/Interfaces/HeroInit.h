// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HeroInit.generated.h"

class UHeroClassInfo;

UINTERFACE(MinimalAPI)
class UHeroInit : public UInterface
{
	GENERATED_BODY()
};

class PAPUGIGAME_API IHeroInit
{
	GENERATED_BODY()

public:
	virtual void OnHeroInitialized(UHeroClassInfo* ClassInfo) = 0;
};
