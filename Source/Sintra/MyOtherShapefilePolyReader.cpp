// Fill out your copyright notice in the Description page of Project Settings.

#include "MyOtherShapefilePolyReader.h"


namespace mapbox {
	namespace util {

		template <>
		struct nth<0, FVector> {
			inline static auto get(const FVector &t) {
				return t.X;
			};
		};
		template <>
		struct nth<1, FVector> {
			inline static auto get(const FVector &t) {
				return t.Y;
			};
		};

	}
}

TArray<FPolygon> UMyOtherShapefilePolyReader::getSplinePolygons()
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
	char* shapeFile = "D:/Daniel_Casadinho/New_Data/Vetor/A_river_buffered.shp";
	GDALAllRegister();

	char* heightFile = "D:/Daniel_Casadinho/New_Data/Vetor/A_river.shp";
	GDALAllRegister();

	GDALDataset *water = (GDALDataset*)GDALOpenEx(shapeFile, GDAL_OF_VECTOR, NULL, NULL, NULL);

	GDALDataset *heights = (GDALDataset*)GDALOpenEx(heightFile, GDAL_OF_VECTOR, NULL, NULL, NULL);
	if (water == NULL || heights == NULL) {
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

	OGRLayer  *pLayer = water->GetLayerByName("A_river_buffered");
	OGRLayer  *hLayer = heights->GetLayerByName("A_river");
	//OGRFeatureDefn *poFDefn = pLayer->GetLayerDefn();

	//start at first feature
	pLayer->ResetReading();
	hLayer->ResetReading();

	//local variables
	OGRFeature *poFeature;
	OGRPoint point;

	OGRFeature *hFeature;
	OGRPoint height;

	UE_LOG(LogTemp, Display, TEXT("Number of Features: %d\n"), pLayer->GetFeatureCount(true));
	for (int i = 0; i < pLayer->GetFeatureCount(true); i++)
	{

		poFeature = pLayer->GetNextFeature();
		hFeature = hLayer->GetNextFeature();

		GIntBig FID = poFeature->GetFID();
		//UE_LOG(LogTemp, Display, TEXT("Feature %d chosen\n"), FID);

		//if(FID == 0)
		{
			OGRGeometry *poGeometry;
			poGeometry = poFeature->GetGeometryRef();

			OGRGeometry *hGeometry;
			hGeometry = hFeature->GetGeometryRef();

			//if (poGeometry != NULL && poGeometry->getGeometryType() == wkbLineString25D)
				//UE_LOG(LogTemp, Display, TEXT("x"));

			if (poGeometry != NULL && wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon
				&& hGeometry != NULL && wkbFlatten(hGeometry->getGeometryType()) == wkbPolygon)
			{

				FPolygon poly;
				OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
				OGRPolygon *hPolygon = (OGRPolygon *)hGeometry;

				//UE_LOG(LogTemp, Display, TEXT("Number of Rings = %d\n"), poPolygon->getNumInteriorRings()+1);

				/*Exterior Ring*/
				OGRLinearRing *poExtRing = poPolygon->getExteriorRing();
				OGRLinearRing *hExtRing = hPolygon->getExteriorRing();

				//float average = 0;
				//float minor = std::numeric_limits<float>::max();
				//Average heights for exterior ring
				//for (int k = 0; k < hExtRing->getNumPoints(); k++)
				//{
				//	hExtRing->getPoint(k, &height);
				//	double zCoord = height.getZ() * 100;

				//	average += zCoord;
				//	minor = minor < zCoord ? minor : zCoord;
					//UE_LOG(LogTemp, Display, TEXT("Elevation = [%lf]\n"), zCoord);
				//}
				//average /= hExtRing->getNumPoints();
				//UE_LOG(LogTemp, Display, TEXT("Average Elevation = [%lf]\n"), average);

				//UE_LOG(LogTemp, Display, TEXT("Number of Points = %d\n"), poExtRing->getNumPoints());
				//UE_LOG(LogTemp, Display, TEXT("Number of Heights = %d\n"), hExtRing->getNumPoints());
				
				//float step = (float)poExtRing->getNumPoints() / (float)hExtRing->getNumPoints();
				
				//UE_LOG(LogTemp, Display, TEXT("Step = %lf\n"), step);

				double min = std::numeric_limits<double>::max();
				int index = 0;


				for (int k = 0; k < poExtRing->getNumPoints(); k++)
				{

					poExtRing->getPoint(k, &point);
					double xCoord = point.getX();
					double yCoord = point.getY();


					double col = (xCoord - tifTransformFactor[0]) / tifTransformFactor[1];
					double row = (tifTransformFactor[3] - yCoord) / (-tifTransformFactor[5]);

					for (int l = 0; l < hExtRing->getNumPoints(); l++)
					{
						hExtRing->getPoint(l, &height);

						double x2 = height.getX();
						double y2 = height.getY();

						double col2 = (x2 - tifTransformFactor[0]) / tifTransformFactor[1];
						double row2 = (tifTransformFactor[3] - y2) / (-tifTransformFactor[5]);


						double dist = sqrt(pow(col - col2, 2) + pow(row - row2, 2));

						if (dist < min)
						{
							index = l;
							min = dist;
						}
					}

					hExtRing->getPoint(index, &height);
					double zCoord = height.getZ() * 100;

					UE_LOG(LogTemp, Display, TEXT("Point = %d; Corresponding = %d; Height = %lf; Dist = %lf\n"), k, index, zCoord, min);


					//int quota = k / step;
					//hExtRing->getPoint(quota, &height);
					//double zCoord = height.getZ() * 100;
					//UE_LOG(LogTemp, Display, TEXT("Height Point = %d = %lf\n"), quota, zCoord);


					//tifBand->RasterIO(GF_Read, (int)col, (int)row, 1, 1, tifScanLine, 1, 1, GDT_Float32, 1, 0);

					FVector pointCoords = FVector((float)getWorldX(col), (float)getWorldY(row), zCoord);
					poly.addPoint(pointCoords);
					//UE_LOG(LogTemp, Display, TEXT("[%lf,%lf, %lf]\n"), (float)getWorldX(col), (float)getWorldY(row), point.getZ()*100);
				}

				/*Interior Rings*/
				/*for (int k = 0; k < poPolygon->getNumInteriorRings(); k++)
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
				}*/
				splinePolygons.Add(poly);
			}
		}
		OGRFeature::DestroyFeature(poFeature);
	}
	//UE_LOG(LogTemp, Display, TEXT("Number of Features to add %d\n"), splinePolygons.Num());

	CPLFree(pngScanLine);
	CPLFree(tifScanLine);
	GDALClose(water);
	GDALClose(png);
	GDALClose(tif);

	return splinePolygons;
}

TArray<int> UMyOtherShapefilePolyReader::triangulate(TArray<FVector> vertices)
{
	TArray<int> splineTriangles;
	splineTriangles = TArray<int>();

	// The number type to use for tessellation
	//using Coord = double;

	// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
	// data won't have more than 65536 vertices.
	using N = uint32_t;

	// Create array
	//using Point = std::array<Coord, 2>;
	std::vector<std::vector<FVector>> polygon;

	// Fill polygon structure with actual data. Any winding order works.
	// The first polyline defines the main polygon.
	//polygon.push_back({ {100, 0}, {100, 100}, {0, 100}, {0, 0} });
	// Following polylines define holes.
	//polygon.push_back({ {75, 25}, {75, 75}, {25, 75}, {25, 25} });

	std::vector<FVector > ring;


	for (int i = 0; i < vertices.Num(); i++)
	{
		ring.push_back(vertices[i]);

		//UE_LOG(LogTemp, Display, TEXT("%lf - %lf - %lf\n"), ring[i].X, ring[i].Y, ring[i].Z);
		//UE_LOG(LogTemp, Display, TEXT("[%lf,%lf, %lf]\n"), vertices[i].X, vertices[i].Y, vertices[i].Z);

	}

	polygon.push_back(ring);


	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygon);


	//UE_LOG(LogTemp, Display, TEXT("Number of points - %d\n"), vertices.Num());

	for (int i = 0; i < indices.size(); i++)
	{
		//splineTriangles.Add(indices[i]);
		splineTriangles.Insert(indices[i], 0);
		//UE_LOG(LogTemp, Display, TEXT("Tri - %d\n"), indices[i]);
	}

	return splineTriangles;
}


double UMyOtherShapefilePolyReader::getElevation(double pixelX, double pixelY, GDALRasterBand *demBand, float *scanLine) {
	demBand->RasterIO(GF_Read, pixelX, pixelY, 1, 1, scanLine, 1, 1, GDT_Float32, 0, 0);

	return *scanLine;
}

double UMyOtherShapefilePolyReader::getWorldX(double geoX)
{
	return geoX * 500;
}

double UMyOtherShapefilePolyReader::getWorldY(double geoY)
{
	return geoY * 500;
}