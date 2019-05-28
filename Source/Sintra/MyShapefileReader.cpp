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
	char* shapeFile = "D:/Daniel_Casadinho/New_Data/Vetor/L_vias_3D_20790.shp";
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
	
	OGRLayer  *pLayer = road->GetLayerByName("L_vias_3D_20790");
	OGRFeatureDefn *poFDefn = pLayer->GetLayerDefn();
	pLayer->ResetReading();
	OGRFeature *poFeature;
	OGRPoint point;
	for (int i = 0; i < pLayer->GetFeatureCount(true); i++)
	{
		poFeature = pLayer->GetNextFeature();
    	GIntBig FID = poFeature->GetFID();
		if (FID == 48 || FID == 83 || FID == 98 || FID == 100 || FID == 157 || FID == 101 || FID == 525 || FID == 558 || FID == 565 || FID == 1110
			|| FID == 1125 || FID == 1126 || FID == 1127 || FID == 1137 || FID == 1167)
//		if (FID == 44 || FID == 45 || FID == 46 || FID == 51 || FID == 145 || FID == 146 || FID == 147 || FID == 527 || FID == 528 || FID == 799
//			|| FID == 800 || FID == 887 || FID == 888)
		{
			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();
			if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
			{
				FLine line;
				OGRLineString *poLineString = (OGRLineString *)poGeometry;
				for (int k = 0; k < poLineString->getNumPoints(); k++)
				{
					poLineString->getPoint(k, &point);
					double xCoord = point.getX();
					double yCoord = point.getY();


					int col = (int)(xCoord - tifTransformFactor[0]) / tifTransformFactor[1];
					int row = (int)(tifTransformFactor[3] - yCoord) / (-tifTransformFactor[5]);

					tifBand->RasterIO(GF_Read, col, row, 1, 1, tifScanLine, 1, 1, GDT_Float32, 1, 0);

					FVector pointCoords = FVector((float)getWorldX(col), (float)getWorldY(row), (*tifScanLine) * 100);
					line.addPoint(pointCoords);
				}
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


