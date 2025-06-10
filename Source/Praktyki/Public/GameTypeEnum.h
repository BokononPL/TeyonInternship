#pragma once

#include "UObject/ObjectMacros.h"

UENUM(Blueprintable, BlueprintType)
enum GameTypeEnum : uint8
{
	Practice	UMETA(DisplayName = "Practice"),
	Race		UMETA(DisplayName = "Race")
};

