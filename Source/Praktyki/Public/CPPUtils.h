#pragma once

static void Print(FString Message, float Duration, FColor Color = FColor::Green)
{
	GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);
}