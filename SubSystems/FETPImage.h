#pragma once

#include "../FETestPlatform.h"

class FETPImage
{
public:
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

	std::string EncodeRawDataToBase64();
	void DecodeBase64ToRawData(std::string Base64String, int width, int height);
private:
	GLuint textureID = -1;
	int width = 0;
	int height = 0;

	std::string fullPath;
	unsigned char* tempRawData = nullptr;

	std::string Base64Characters =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::string Base64Encode(unsigned char const* BytesToEncode, unsigned int Length);
	bool IsBase64(unsigned char Character);
	std::string Base64Decode(std::string const& EncodedString);
};