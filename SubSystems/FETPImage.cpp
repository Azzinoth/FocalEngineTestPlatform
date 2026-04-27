#include "FETPImage.h"
#include "NodeSystem/VisualNodeSystem/VisualNodeCore.h"

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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
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

std::string FETPImage::EncodeRawDataToBase64()
{
	unsigned char* TempRawData = GetRawData();
	std::string Result = NODE_CORE.Base64Encode(TempRawData, GetWidth() * GetHeight() * 4);
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

	std::string RawData = NODE_CORE.Base64Decode(Base64String);
	Initialize((unsigned char*)(RawData.c_str()), Width, Height);
}

void FETPImage::ModifyPixels(std::function<void(unsigned char& R, unsigned char& G, unsigned char& B)> ModificationRule)
{
	unsigned char* data = GetRawData();

	if (data == nullptr) return;

	for (int i = 0; i < Width * Height * 4; i += 4)
	{
		// Extract the RGB values of the current pixel
		unsigned char R = data[i];
		unsigned char G = data[i + 1];
		unsigned char B = data[i + 2];

		// Apply the modification rule
		ModificationRule(R, G, B);

		// Update the pixel data
		data[i] = R;
		data[i + 1] = G;
		data[i + 2] = B;
	}

	// Update the texture with the modified data
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	// Clean up
	delete[] data;
}