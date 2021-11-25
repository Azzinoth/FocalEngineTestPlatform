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