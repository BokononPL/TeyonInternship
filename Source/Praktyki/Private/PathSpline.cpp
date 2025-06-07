// Copyright 2025 Teyon. All Rights Reserved.


#include "PathSpline.h"

APathSpline::APathSpline()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	RootComponent = SplineComponent;
}