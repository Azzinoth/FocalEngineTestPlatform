#include "FETPImage.h"

FETPImage::FETPImage()
{
	tempRawData = nullptr;
}

FETPImage::FETPImage(FETPImage& src)
{
	tempRawData = nullptr;
	fullPath = src.fullPath;
	unsigned char* tempData = src.getRawData();
	initialize(tempData, src.getWidth(), src.getHeight());
	delete[] tempData;
}

FETPImage::FETPImage(unsigned char* rawData, int width, int height)
{
	tempRawData = nullptr;
	initialize(rawData, width, height);
}

FETPImage::FETPImage(std::string filePath)
{
	tempRawData = nullptr;
	std::vector<unsigned char> rawData;
	unsigned uWidth, uHeight;
	lodepng::decode(rawData, uWidth, uHeight, filePath);

	unsigned char* tempData = new unsigned char[uWidth * uHeight * 4];
	memcpy_s(tempData, uWidth * uHeight * 4, rawData.data(), uWidth * uHeight * 4);
	setFullPath(filePath);
	initialize(tempData, uWidth, uHeight);
	delete[] tempData;
}

FETPImage::~FETPImage()
{
	glDeleteTextures(1, &textureID);
}

void FETPImage::initialize(unsigned char* rawData, int width, int height)
{
	if (rawData == nullptr || width < 1 || width > 8196 || height < 1 || height > 8196)
		return;

	glGenTextures(1, &textureID);
	this->width = width;
	this->height = height;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rawData);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint FETPImage::getTextureID()
{
	return textureID;
}

unsigned char* FETPImage::getRawData()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	tempRawData = new unsigned char[width * height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempRawData);

	return tempRawData;
}

int FETPImage::getWidth()
{
	return width;
}

int FETPImage::getHeight()
{
	return height;
}

FETPImage* FETPImage::getRegion(int left, int top, int width, int height)
{
	if (width <= 0 || height <= 0 || top < 0 || left < 0)
		return nullptr;

	if (top + height > this->height)
		return nullptr;

	if (left + width > this->width)
		return nullptr;

	unsigned char* rawData = getRawData();
	unsigned char* newRawData = new unsigned char[width * height * 4];

	size_t destenationIndex = 0;
	for (size_t i = top; i < size_t(top + height); i++)
	{
		for (size_t j = left; j < size_t(left + width); j++)
		{
			size_t sourceIndex = (i * this->width + j) * 4;
			newRawData[destenationIndex++] = rawData[sourceIndex];
			newRawData[destenationIndex++] = rawData[sourceIndex + 1];
			newRawData[destenationIndex++] = rawData[sourceIndex + 2];
			newRawData[destenationIndex++] = rawData[sourceIndex + 3];
		}
	}

	FETPImage* result = new FETPImage(newRawData, width, height);

	delete[] rawData;
	delete[] newRawData;

	return result;
}

std::string FETPImage::getFullPath()
{
	return fullPath;
}

void FETPImage::setFullPath(std::string newValue)
{
	fullPath = newValue;
}

std::string FETPImage::Base64Encode(unsigned char const* BytesToEncode, unsigned int Length)
{
	int i = 0;
	int j = 0;
	unsigned char OriginalDataGroup[3];
	unsigned char Base64Quartet[4];
	std::string Result;

	while (Length--)
	{
		OriginalDataGroup[i++] = *(BytesToEncode++);
		if (i == 3)
		{
			Base64Quartet[0] = (OriginalDataGroup[0] & 0xfc) >> 2;
			Base64Quartet[1] = ((OriginalDataGroup[0] & 0x03) << 4) + ((OriginalDataGroup[1] & 0xf0) >> 4);
			Base64Quartet[2] = ((OriginalDataGroup[1] & 0x0f) << 2) + ((OriginalDataGroup[2] & 0xc0) >> 6);
			Base64Quartet[3] = OriginalDataGroup[2] & 0x3f;

			for (i = 0; (i < 4); i++)
				Result += Base64Characters[Base64Quartet[i]];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			OriginalDataGroup[j] = '\0';

		Base64Quartet[0] = (OriginalDataGroup[0] & 0xfc) >> 2;
		Base64Quartet[1] = ((OriginalDataGroup[0] & 0x03) << 4) + ((OriginalDataGroup[1] & 0xf0) >> 4);
		Base64Quartet[2] = ((OriginalDataGroup[1] & 0x0f) << 2) + ((OriginalDataGroup[2] & 0xc0) >> 6);
		Base64Quartet[3] = OriginalDataGroup[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			Result += Base64Characters[Base64Quartet[j]];

		while ((i++ < 3))
			Result += '=';
	}

	return Result;
}

bool FETPImage::IsBase64(unsigned char Character)
{
	return (isalnum(Character) || (Character == '+') || (Character == '/'));
}

std::string FETPImage::Base64Decode(std::string const& EncodedString)
{
	int Length = static_cast<int>(EncodedString.size());
	int i = 0;
	int j = 0;
	int InputStringIndex = 0;
	unsigned char OriginalDataGroup[3];
	unsigned char Base64Quartet[4];
	std::string Result;

	while (Length-- && (EncodedString[InputStringIndex] != '=') && IsBase64(EncodedString[InputStringIndex]))
	{
		Base64Quartet[i++] = EncodedString[InputStringIndex]; InputStringIndex++;
		if (i == 4)
		{
			for (i = 0; i < 4; i++)
				Base64Quartet[i] = static_cast<unsigned char>(Base64Characters.find(Base64Quartet[i]));

			OriginalDataGroup[0] = (Base64Quartet[0] << 2) + ((Base64Quartet[1] & 0x30) >> 4);
			OriginalDataGroup[1] = ((Base64Quartet[1] & 0xf) << 4) + ((Base64Quartet[2] & 0x3c) >> 2);
			OriginalDataGroup[2] = ((Base64Quartet[2] & 0x3) << 6) + Base64Quartet[3];

			for (i = 0; (i < 3); i++)
				Result += OriginalDataGroup[i];
			i = 0;
		}
	}

	if (i)
	{
		for (j = i; j < 4; j++)
			Base64Quartet[j] = 0;

		for (j = 0; j < 4; j++)
			Base64Quartet[j] = static_cast<unsigned char>(Base64Characters.find(Base64Quartet[j]));

		OriginalDataGroup[0] = (Base64Quartet[0] << 2) + ((Base64Quartet[1] & 0x30) >> 4);
		OriginalDataGroup[1] = ((Base64Quartet[1] & 0xf) << 4) + ((Base64Quartet[2] & 0x3c) >> 2);
		OriginalDataGroup[2] = ((Base64Quartet[2] & 0x3) << 6) + Base64Quartet[3];

		for (j = 0; (j < i - 1); j++) Result += OriginalDataGroup[j];
	}

	return Result;
}

std::string FETPImage::EncodeRawDataToBase64()
{
	unsigned char* TempRawData = getRawData();
	std::string Result = Base64Encode(tempRawData, getWidth() * getHeight() * 4);
	delete[] TempRawData;

	return Result;
}

void FETPImage::DecodeBase64ToRawData(std::string Base64String, int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	if (Base64String.size() == 0)
		return;

	if (Base64String.size() % 4 != 0)
		return;

	glDeleteTextures(1, &textureID);

	std::string RawData = Base64Decode(Base64String);
	initialize((unsigned char*)(RawData.c_str()), width, height);
}