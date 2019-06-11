// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShapefileReader.h"


TArray<FLine> UMyShapefileReader::getSplinePoints()
{

	TArray<FLine> splinePoints;
	splinePoints = TArray<FLine>();

	/* DEM file setup */
	char* demPNG = "D:/Daniel_Casadinho/New_Data/demclip.png";
	GDALDataset *png;
	GDALAllRegister();
	
	char* demTIF = "D:/Daniel_Casadinho/New_Data/demclip.tif";
	GDALDataset *tif;
	GDALAllRegister();

	png = (GDALDataset*)GDALOpen(demPNG, GA_ReadOnly);

	tif = (GDALDataset*)GDALOpen(demTIF, GA_ReadOnly);
	double tifTransformFactor[6];
	tif->GetGeoTransform(tifTransformFactor);


	float *tifScanLine = (float*)CPLMalloc(sizeof(float*) * 1);
	GDALRasterBand *tifBand = tif->GetRasterBand(1);

	float *pngScanLine = (float*)CPLMalloc(sizeof(float*)*1);
	GDALRasterBand *pngBand = png->GetRasterBand(1);
	/* Shapefile Setup */
	char* shapeFile = "D:/Daniel_Casadinho/New_Data/Vector2/L_Vias_3D_small.shp";
	GDALAllRegister();

	GDALDataset *road = (GDALDataset*)GDALOpenEx(shapeFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (road == NULL) { 
		printf("Open failed.\n"); 
		exit(1);
	}

	

	//FLine line;
	//for (int i = 0; i < tif->GetRasterYSize(); i = i + 100)
	//	for (int j = 0; j < tif->GetRasterXSize(); j = j + 100) {
	//		tifBand->RasterIO(GF_Read, j, i, 1, 1, tifScanLine, 1, 1, GDT_Float32, 0, 0);
	//		
	//		FVector pointCoords = FVector((float)i, (float)j, (*tifScanLine)*100);
	//		line.addPoint(pointCoords);
	//	}

	/* --- READING SHAPEFILE --- */
	
	OGRLayer  *pLayer = road->GetLayerByName("L_Vias_3D_small");
	OGRFeatureDefn *poFDefn = pLayer->GetLayerDefn();
	pLayer->ResetReading();
	OGRFeature *poFeature;
	OGRPoint point;

	//UE_LOG(LogTemp, Display, TEXT("Number of Features: %d\n"), pLayer->GetFeatureCount(true));
	for (int i = 0; i < pLayer->GetFeatureCount(true); i++)
	{
		poFeature = pLayer->GetNextFeature();
    	GIntBig FID = poFeature->GetFID();

		UE_LOG(LogTemp, Display, TEXT("Feature %d chosen\n"), FID);

//		if (FID == 48 || FID == 83 || FID == 98 || FID == 100 || FID == 157 || FID == 101 || FID == 525 || FID == 558 || FID == 565 || FID == 1110
//			|| FID == 1125 || FID == 1126 || FID == 1127 || FID == 1137 || FID == 1167)
		//if (FID >= 0 && FID <= 1000)
		{
			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();

			//if (poGeometry != NULL && poGeometry->getGeometryType() == wkbLineString25D)
				//UE_LOG(LogTemp, Display, TEXT("x"));

			if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
			{

				FLine line;
				OGRLineString *poLineString = (OGRLineString *)poGeometry;

				UE_LOG(LogTemp, Display, TEXT("Number of Points = %d\n"), poLineString->getNumPoints());

				for (int k = 0; k < poLineString->getNumPoints(); k++)
				{

					poLineString->getPoint(k, &point);
					double xCoord = point.getX();
					double yCoord = point.getY();


					double col = (xCoord - tifTransformFactor[0]) / tifTransformFactor[1];
					double row = (tifTransformFactor[3] - yCoord) / (-tifTransformFactor[5]);


					tifBand->RasterIO(GF_Read, (int) col, (int) row, 1, 1, tifScanLine, 1, 1, GDT_Float32, 1, 0);

					FVector pointCoords = FVector((float)getWorldX(col), (float)getWorldY(row), (*tifScanLine) * 100); //point.getZ();
					line.addPoint(pointCoords);
					//UE_LOG(LogTemp, Display, TEXT("[%lf,%lf, %lf]"), (float)getWorldX(col), (float)getWorldY(row), point.getZ()*100);
				}
				UE_LOG(LogTemp, Display, TEXT("\n"));
				splinePoints.Add(line);
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	
	CPLFree(pngScanLine);
	GDALClose(road);
	GDALClose(png);
	GDALClose(tif);


	return splinePoints;

}

double UMyShapefileReader::getElevation(double pixelX, double pixelY, GDALRasterBand *demBand, float *scanLine ) {
	demBand->RasterIO(GF_Read, pixelX, pixelY, 1, 1, scanLine, 1, 1, GDT_Float32, 0, 0);

	return *scanLine;
}

double UMyShapefileReader::getWorldX(double geoX)
{
	return geoX*500;
}

double UMyShapefileReader::getWorldY(double geoY)
{
	return geoY*500;
}


