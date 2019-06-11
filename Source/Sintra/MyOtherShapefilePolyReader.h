// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Core/Public/Containers/Array.h"
#include "gdal.h"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "earcut.hpp"
#include "MyShapefilePolyReader.h"
#include "MyOtherShapefilePolyReader.generated.h"

/**
 * 
 */
/*
USTRUCT(BlueprintType)
struct FPolygon {

	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		TArray<FVector> polyPoints;

	void addPoint(FVector newPoint) {
		polyPoints.Add(newPoint);
	}

	TArray<FVector> getPolyPoints() {
		return polyPoints;
	}

	FPolygon() {
		polyPoints = TArray<FVector>();
	}

};*/

/**
 *
 */
UCLASS()
class SINTRA_API UMyOtherShapefilePolyReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		static TArray<FPolygon> getSplinePolygons();
	UFUNCTION(BlueprintCallable)
		static TArray<int> triangulate(TArray<FVector> vertices);
private:

	static double getElevation(double pixelX, double pixelY, GDALRasterBand *demBand, float *scanLine);
	static double getWorldX(double geoX);
	static double getWorldY(double geoY);
};
