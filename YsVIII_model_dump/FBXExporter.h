#pragma once
#include "IT3File.h"
#include "MTBFile.h"
#include "Scene.h"
class FBXExporter
{
public:
	FBXExporter();
	~FBXExporter();
	void ExportScene(Scene scene);
};

