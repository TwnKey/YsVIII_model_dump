#pragma once
#include "IT3File.h"

class FBXExporter
{
public:
	FBXExporter();
	~FBXExporter();
	void GenerateScene(IT3File file);
};

