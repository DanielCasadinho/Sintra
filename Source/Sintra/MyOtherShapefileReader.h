// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/Containers/Array.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "MyShapefileReader.h"
#include "MyOtherShapefileReader.generated.h"

/*
USTRUCT(BlueprintType)
struct FLine {

	GENERATED_USTRUCT_BODY()
public:
		UPROPERTY(BlueprintReadOnly)
		TArray<FVector> linePoints;

	void addPoint(FVector newPoint) {
		linePoints.Add(newPoint);
	}

	TArray<FVector> getLinePoints() {
		return linePoints;
	}

	FLine() {
		linePoints = TArray<FVector>();
	}

};*/

UCLASS()
class SINTRA_API UMyOtherShapefileReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		static TArray<FLine> getSplinePoints();
private:

	static double getElevation(double pixelX, double pixelY, GDALRasterBand *demBand, float *scanLine);
	static double getWorldX(double geoX);
	static double getWorldY(double geoY);

};
