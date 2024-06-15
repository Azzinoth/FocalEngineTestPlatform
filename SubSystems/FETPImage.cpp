#include "FETPImage.h"

FETPImage::FETPImage()
{
	TempRawData = nullptr;
}

FETPImage::FETPImage(FETPImage& Other)
{
	TempRawData = nullptr;
	FullPath = Other.FullPath;
	unsigned char* TempData = Other.GetRawData();
	Initialize(TempData, Other.GetWidth(), Other.GetHeight());
	delete[] TempData;
}

FETPImage::FETPImage(unsigned char* RawData, int Width, int Height)
{
	TempRawData = nullptr;
	Initialize(RawData, Width, Height);
}

FETPImage::FETPImage(std::string FilePath)
{
	TempRawData = nullptr;
	std::vector<unsigned char> RawData;
	unsigned ImageWidth, ImageHeight;
	lodepng::decode(RawData, ImageWidth, ImageHeight, FilePath);

	unsigned char* TempData = new unsigned char[ImageWidth * ImageHeight * 4];
	memcpy_s(TempData, ImageWidth * ImageHeight * 4, RawData.data(), ImageWidth * ImageHeight * 4);
	SetFullPath(FilePath);
	Initialize(TempData, ImageWidth, ImageHeight);
	delete[] TempData;
}

FETPImage::~FETPImage()
{
	glDeleteTextures(1, &TextureID);
}

void FETPImage::Initialize(unsigned char* RawData, int Width, int Height)
{
	if (RawData == nullptr || Width < 1 || Width > 8196 || Height < 1 || Height > 8196)
		return;

	glGenTextures(1, &TextureID);
	this->Width = Width;
	this->Height = Height;

	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, RawData);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint FETPImage::GetTextureID()
{
	return TextureID;
}

unsigned char* FETPImage::GetRawData()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	TempRawData = new unsigned char[Width * Height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, TempRawData);

	return TempRawData;
}

int FETPImage::GetWidth()
{
	return Width;
}

int FETPImage::GetHeight()
{
	return Height;
}

FETPImage* FETPImage::GetRegion(int Left, int Top, int Width, int Height)
{
	if (Width <= 0 || Height <= 0 || Top < 0 || Left < 0)
		return nullptr;

	if (Top + Height > this->Height)
		return nullptr;

	if (Left + Width > this->Width)
		return nullptr;

	unsigned char* RawData = GetRawData();
	unsigned char* NewRawData = new unsigned char[Width * Height * 4];

	size_t DestinationIndex = 0;
	for (size_t i = Top; i < size_t(Top + Height); i++)
	{
		for (size_t j = Left; j < size_t(Left + Width); j++)
		{
			size_t SourceIndex = (i * this->Width + j) * 4;
			NewRawData[DestinationIndex++] = RawData[SourceIndex];
			NewRawData[DestinationIndex++] = RawData[SourceIndex + 1];
			NewRawData[DestinationIndex++] = RawData[SourceIndex + 2];
			NewRawData[DestinationIndex++] = RawData[SourceIndex + 3];
		}
	}

	FETPImage* Result = new FETPImage(NewRawData, Width, Height);

	delete[] RawData;
	delete[] NewRawData;

	return Result;
}

std::string FETPImage::GetFullPath()
{
	return FullPath;
}

void FETPImage::SetFullPath(std::string NewValue)
{
	FullPath = NewValue;
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
	unsigned char* TempRawData = GetRawData();
	std::string Result = Base64Encode(TempRawData, GetWidth() * GetHeight() * 4);
	delete[] TempRawData;

	return Result;
}

void FETPImage::DecodeBase64ToRawData(std::string Base64String, int Width, int Height)
{
	if (Width <= 0 || Height <= 0)
		return;

	if (Base64String.size() == 0)
		return;

	if (Base64String.size() % 4 != 0)
		return;

	glDeleteTextures(1, &TextureID);

	std::string RawData = Base64Decode(Base64String);
	Initialize((unsigned char*)(RawData.c_str()), Width, Height);
}

void FETPImage::ModifyPixels(std::function<void(unsigned char& R, unsigned char& G, unsigned char& B)> ModificationRule)
{
	unsigned char* data = GetRawData();

	if (data == nullptr) return;

	for (int i = 0; i < Width * Height * 4; i += 4)
	{
		// Extract the RGB values of the current pixel
		unsigned char r = data[i];
		unsigned char g = data[i + 1];
		unsigned char b = data[i + 2];

		// Apply the modification rule
		ModificationRule(r, g, b);

		// Update the pixel data
		data[i] = r;
		data[i + 1] = g;
		data[i + 2] = b;
	}

	// Update the texture with the modified data
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Clean up
	delete[] data;
}