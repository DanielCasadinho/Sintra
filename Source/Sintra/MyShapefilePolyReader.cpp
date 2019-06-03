// Fill out your copyright notice in the Description page of Project Settings.

#include "MyShapefilePolyReader.h"
TArray<FPolygon> UMyShapefilePolyReader::getSplinePolygons()
{
	TArray<FPolygon> splinePolygons;
	splinePolygons = TArray<FPolygon>();

	/* DEM file setup */
	char* demPNG = "D:/Daniel_Casadinho/New_Data/demclip.png";
	GDALDataset *png;
	GDALAllRegister();

	char* demTIF = "D:/Daniel_Casadinho/New_Data/demclip.tif";
	GDALDataset *tif;
	GDALAllRegister();

	png = (GDALDataset*)GDALOpen(demPNG, GA_ReadOnly);

	tif = (GDALDataset*)GDALOpen(demTIF, GA_ReadOnly);

	/*Retrieve projection info*/
	double tifTransformFactor[6];
	tif->GetGeoTransform(tifTransformFactor);

	/*Setting up Buffers (pngscanline not necessary?)*/
	float *tifScanLine = (float*)CPLMalloc(sizeof(float*) * 1);
	float *pngScanLine = (float*)CPLMalloc(sizeof(float*) * 1);

	/*Access Bands on Rasters*/
	GDALRasterBand *tifBand = tif->GetRasterBand(1);
	GDALRasterBand *pngBand = png->GetRasterBand(1);


	/* Shapefile Setup */
	char* shapeFile = "D:/Daniel_Casadinho/New_Data/Vetor/A_hidrografia_3D_20790_extracted.shp";
	GDALAllRegister();

	GDALDataset *water = (GDALDataset*)GDALOpenEx(shapeFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (water == NULL) {
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
	 
	OGRLayer  *pLayer = water->GetLayerByName("A_hidrografia_3D_20790_extracted");
	//OGRFeatureDefn *poFDefn = pLayer->GetLayerDefn();

	//start at first feature
	pLayer->ResetReading();

	//local variables
	OGRFeature *poFeature;
	OGRPoint point;

	//UE_LOG(LogTemp, Display, TEXT("Number of Features: %d\n"), pLayer->GetFeatureCount(true));
	for (int i = 0; i < pLayer->GetFeatureCount(true); i++)
	{
		
		poFeature = pLayer->GetNextFeature();
		GIntBig FID = poFeature->GetFID();

		UE_LOG(LogTemp, Display, TEXT("Feature %d chosen\n"), FID);

		if(FID == 0)
		{
			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef()->clone();

			//if (poGeometry != NULL && poGeometry->getGeometryType() == wkbLineString25D)
				//UE_LOG(LogTemp, Display, TEXT("x"));

			if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon)
			{

				FPolygon poly;
				OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;

				UE_LOG(LogTemp, Display, TEXT("Number of Rings = %d\n"), poPolygon->getNumInteriorRings()+1);
				/*
				for (int k = 0; k < poPolygon->getNumPoints(); k++)
				{

					poPolygon->getPoint(k, &point);
					double xCoord = point.getX();
					double yCoord = point.getY();


					double col = (xCoord - tifTransformFactor[0]) / tifTransformFactor[1];
					double row = (tifTransformFactor[3] - yCoord) / (-tifTransformFactor[5]);


					tifBand->RasterIO(GF_Read, (int)col, (int)row, 1, 1, tifScanLine, 1, 1, GDT_Float32, 1, 0);

					FVector pointCoords = FVector((float)getWorldX(col), (float)getWorldY(row), point.getZ() * 100);
					poly.addPoint(pointCoords);
					//UE_LOG(LogTemp, Display, TEXT("[%lf,%lf, %lf]"), (float)getWorldX(col), (float)getWorldY(row), point.getZ()*100);
				}
				UE_LOG(LogTemp, Display, TEXT("\n"));
				splinePoints.Add(poly);*/
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}

	CPLFree(pngScanLine);
	CPLFree(tifScanLine);
	GDALClose(water);
	GDALClose(png);
	GDALClose(tif);

	return splinePolygons;
}


double UMyShapefilePolyReader::getElevation(double pixelX, double pixelY, GDALRasterBand *demBand, float *scanLine) {
	demBand->RasterIO(GF_Read, pixelX, pixelY, 1, 1, scanLine, 1, 1, GDT_Float32, 0, 0);

	return *scanLine;
}

double UMyShapefilePolyReader::getWorldX(double geoX)
{
	return geoX * 500;
}

double UMyShapefilePolyReader::getWorldY(double geoY)
{
	return geoY * 500;
}