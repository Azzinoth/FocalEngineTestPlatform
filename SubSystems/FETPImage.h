#pragma once

#include "../FETestPlatform.h"

class FETPImage
{
public:
	FETPImage();
	FETPImage(FETPImage& Other);
	FETPImage(unsigned char* RawData, int Width, int Height);
	FETPImage(std::string FilePath);
	~FETPImage();

	void Initialize(unsigned char* RawData, int Width, int Height);

	GLuint GetTextureID();

	int GetWidth();
	int GetHeight();

	unsigned char* GetRawData();

	FETPImage* GetRegion(int Left, int Top, int Width, int Height);

	std::string GetFullPath();
	void SetFullPath(std::string NewValue);

	std::string EncodeRawDataToBase64();
	void DecodeBase64ToRawData(std::string Base64String, int Width, int Height);

	void ModifyPixels(std::function<void(unsigned char& R, unsigned char& G, unsigned char& B)> ModificationRule);
private:
	GLuint TextureID = -1;
	int Width = 0;
	int Height = 0;

	std::string FullPath;
	unsigned char* TempRawData = nullptr;

	std::string Base64Characters =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

	std::string Base64Encode(unsigned char const* BytesToEncode, unsigned int Length);
	bool IsBase64(unsigned char Character);
	std::string Base64Decode(std::string const& EncodedString);
};