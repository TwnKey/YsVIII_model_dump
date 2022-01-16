#pragma once
#include "IT3File.h"
#include "MTBFile.h"
#include "Scene.h"
class DAEExporter
{
public:
	DAEExporter();
	~DAEExporter();
	void ExportScene(Scene scene);
};

