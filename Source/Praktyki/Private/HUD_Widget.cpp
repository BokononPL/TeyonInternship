// Copyright 2025 Teyon. All Rights Reserved.


#include "HUD_Widget.h"

void UHUD_Widget::UpdateSpeed(float Value)
{
	Speed = (Value / 100.f) * 3.6f;
}
