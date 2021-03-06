// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "EnginePrivate.h"
#include "MeshVertexPainter/MeshVertexPainterKismetLibrary.h"


void UMeshVertexPainterKismetLibrary::PaintVerticesSingleColor(UStaticMeshComponent* StaticMeshComponent, const FLinearColor& FillColor, bool bConvertToSRGB)
{
	FMeshVertexPainter::PaintVerticesSingleColor(StaticMeshComponent, FillColor, bConvertToSRGB);
}

void UMeshVertexPainterKismetLibrary::PaintVerticesLerpAlongAxis(UStaticMeshComponent* StaticMeshComponent, const FLinearColor& StartColor, const FLinearColor& EndColor, EVertexPaintAxis Axis, bool bConvertToSRGB)
{
	FMeshVertexPainter::PaintVerticesLerpAlongAxis(StaticMeshComponent, StartColor, EndColor, Axis, bConvertToSRGB);
}

void UMeshVertexPainterKismetLibrary::RemovePaintedVertices(UStaticMeshComponent* StaticMeshComponent)
{
	FMeshVertexPainter::RemovePaintedVertices(StaticMeshComponent);
}
