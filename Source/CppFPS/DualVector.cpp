// Fill out your copyright notice in the Description page of Project Settings.

#include "DualVector.h"

DualVector::DualVector()
{
}

DualVector::~DualVector()
{
}

DualVector::DualVector(const FVector& vectorA, const FVector& vectorB)
{
	vector = vectorA;
	vector2 = vectorB;
}