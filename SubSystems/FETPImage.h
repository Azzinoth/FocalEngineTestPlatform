#pragma once

#include "../FETestPlatform.h"

class FETPImage
{
public:
	std::string getID();
	void setID(std::string NewID);

	GLuint getTextureID();

	FETPImage();
	FETPImage(FETPImage& src);
	FETPImage(unsigned char* rawData, int width, int height);
	FETPImage(std::string filePath);
	~FETPImage();

	void initialize(unsigned char* rawData, int width, int height);

	int getWidth();
	int getHeight();

	unsigned char* getRawData();

	FETPImage* getRegion(int left, int top, int width, int height);

	std::string getFullPath();
	void setFullPath(std::string newValue);
private:
	std::string ID;
	GLuint textureID = -1;
	int width = 0;
	int height = 0;

	std::string fullPath;
	unsigned char* tempRawData = nullptr;
};