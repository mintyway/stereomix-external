// Copyright Studio Surround. All Rights Reserved.


#include "SMBasicShapeFunctionLibrary.h"


UStaticMesh* USMBasicShapeFunctionLibrary::GetPlaneMesh()
{
	const FSoftObjectPath PlanePath(TEXT("/Engine/BasicShapes/Plane.Plane"));
	return Cast<UStaticMesh>(PlanePath.TryLoad());
}
