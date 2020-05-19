// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class CPPFPS_API DualVector
{
public:
	DualVector();
	DualVector(const FVector& vectorA, const FVector& vectorB);
	~DualVector();

	FVector vector;
	FVector vector2;
};
