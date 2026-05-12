#include "FETPImage.h"
#include "NodeSystem/VisualNodeSystem/VisualNodeCore.h"

FETPImage::FETPImage()
{
	TemporaryRawData = nullptr;
}

FETPImage::FETPImage(FETPImage& Other)
{
	TemporaryRawData = nullptr;
	FullPath = Other.FullPath;
	unsigned char* TemporaryData = Other.GetRawData();
	Initialize(TemporaryData, Other.GetWidth(), Other.GetHeight());
	delete[] TemporaryData;
}

FETPImage::FETPImage(unsigned char* RawData, int Width, int Height)
{
	TemporaryRawData = nullptr;
	Initialize(RawData, Width, Height);
}

FETPImage::FETPImage(std::string FilePath)
{
	TemporaryRawData = nullptr;
	std::vector<unsigned char> RawData;
	unsigned ImageWidth, ImageHeight;
	lodepng::decode(RawData, ImageWidth, ImageHeight, FilePath);

	unsigned char* TemporaryData = new unsigned char[ImageWidth * ImageHeight * 4];
	memcpy_s(TemporaryData, ImageWidth * ImageHeight * 4, RawData.data(), ImageWidth * ImageHeight * 4);
	SetFullPath(FilePath);
	Initialize(TemporaryData, ImageWidth, ImageHeight);
	delete[] TemporaryData;
}

FETPImage::~FETPImage()
{
	glDeleteTextures(1, &TextureID);
}

FETPImage* FETPImage::FromClipboard()
{
	if (!OpenClipboard(nullptr))
		return nullptr;

	FETPImage* Result = nullptr;

	HANDLE DeviceIndependentBitmapHandle = GetClipboardData(CF_DIBV5);
	if (DeviceIndependentBitmapHandle == nullptr)
		DeviceIndependentBitmapHandle = GetClipboardData(CF_DIB);

	if (DeviceIndependentBitmapHandle != nullptr)
	{
		void* LockedMemory = GlobalLock(DeviceIndependentBitmapHandle);
		if (LockedMemory != nullptr)
		{
			BITMAPINFOHEADER* Header = reinterpret_cast<BITMAPINFOHEADER*>(LockedMemory);
			int Width = Header->biWidth;
			int RawHeight = Header->biHeight;
			int Height = (RawHeight < 0) ? -RawHeight : RawHeight;
			bool bTopDown = RawHeight < 0;
			int BitsPerPixel = Header->biBitCount;

			if ((BitsPerPixel == 24 || BitsPerPixel == 32) && Header->biCompression == BI_RGB && Width > 0 && Height > 0)
			{
				unsigned char* PixelData = reinterpret_cast<unsigned char*>(LockedMemory) + Header->biSize;
				int SourceRowStride = ((Width * BitsPerPixel + 31) / 32) * 4;
				int BytesPerPixel = BitsPerPixel / 8;

				unsigned char* RGBAPixels = new unsigned char[Width * Height * 4];

				for (int Y = 0; Y < Height; Y++)
				{
					int SourceY = bTopDown ? Y : (Height - 1 - Y);
					unsigned char* SourceRow = PixelData + SourceY * SourceRowStride;
					unsigned char* DestinationRow = RGBAPixels + Y * Width * 4;

					for (int X = 0; X < Width; X++)
					{
						DestinationRow[X * 4 + 0] = SourceRow[X * BytesPerPixel + 2];
						DestinationRow[X * 4 + 1] = SourceRow[X * BytesPerPixel + 1];
						DestinationRow[X * 4 + 2] = SourceRow[X * BytesPerPixel + 0];
						DestinationRow[X * 4 + 3] = (BitsPerPixel == 32) ? SourceRow[X * 4 + 3] : 255;
					}
				}

				// Many apps populate CF_DIBV5 with zero alpha even for opaque images.
				if (BitsPerPixel == 32)
				{
					bool bAnyNonZeroAlpha = false;
					for (int i = 3; i < Width * Height * 4; i += 4)
					{
						if (RGBAPixels[i] != 0) { bAnyNonZeroAlpha = true; break; }
					}
					if (!bAnyNonZeroAlpha)
					{
						for (int i = 3; i < Width * Height * 4; i += 4)
							RGBAPixels[i] = 255;
					}
				}

				Result = new FETPImage(RGBAPixels, Width, Height);
				delete[] RGBAPixels;
			}

			GlobalUnlock(DeviceIndependentBitmapHandle);
		}
	}

	if (Result == nullptr)
	{
		HBITMAP ClipboardBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
		if (ClipboardBitmap != nullptr)
		{
			BITMAP ClipboardBitmapDescription = {};
			if (GetObject(ClipboardBitmap, sizeof(BITMAP), &ClipboardBitmapDescription) != 0 && ClipboardBitmapDescription.bmWidth > 0 && ClipboardBitmapDescription.bmHeight > 0)
			{
				int Width = ClipboardBitmapDescription.bmWidth;
				int Height = ClipboardBitmapDescription.bmHeight;

				BITMAPINFO DIBDescription = {};
				DIBDescription.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
				DIBDescription.bmiHeader.biWidth = Width;
				DIBDescription.bmiHeader.biHeight = -Height;
				DIBDescription.bmiHeader.biPlanes = 1;
				DIBDescription.bmiHeader.biBitCount = 32;
				DIBDescription.bmiHeader.biCompression = BI_RGB;

				unsigned char* BGRAPixels = new unsigned char[Width * Height * 4];
				HDC DeviceContext = GetDC(nullptr);
				if (GetDIBits(DeviceContext, ClipboardBitmap, 0, Height, BGRAPixels, &DIBDescription, DIB_RGB_COLORS) != 0)
				{
					unsigned char* RGBAPixels = new unsigned char[Width * Height * 4];
					for (int i = 0; i < Width * Height; i++)
					{
						RGBAPixels[i * 4 + 0] = BGRAPixels[i * 4 + 2];
						RGBAPixels[i * 4 + 1] = BGRAPixels[i * 4 + 1];
						RGBAPixels[i * 4 + 2] = BGRAPixels[i * 4 + 0];
						RGBAPixels[i * 4 + 3] = 255;
					}
					Result = new FETPImage(RGBAPixels, Width, Height);
					delete[] RGBAPixels;
				}
				ReleaseDC(nullptr, DeviceContext);
				delete[] BGRAPixels;
			}
		}
	}

	CloseClipboard();
	return Result;
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

	TemporaryRawData = new unsigned char[Width * Height * 4];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, TemporaryRawData);

	return TemporaryRawData;
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
	unsigned char* TemporaryRawData = GetRawData();
	std::vector<unsigned char> PNGData;
	lodepng::encode(PNGData, TemporaryRawData, GetWidth(), GetHeight());
	std::string Result = NODE_CORE.Base64Encode(PNGData.data(), PNGData.size());
	delete[] TemporaryRawData;

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

	std::string PNGData = NODE_CORE.Base64Decode(Base64String);
	if (PNGData.empty())
		return;

	std::vector<unsigned char> RawData;
	unsigned ImageWidth = 0;
	unsigned ImageHeight = 0;
	unsigned ErrorCode = lodepng::decode(RawData, ImageWidth, ImageHeight, reinterpret_cast<const unsigned char*>(PNGData.data()), PNGData.size());
	if (ErrorCode != 0)
		return;

	glDeleteTextures(1, &TextureID);
	Initialize(RawData.data(), int(ImageWidth), int(ImageHeight));
}

void FETPImage::ModifyPixels(std::function<void(unsigned char& R, unsigned char& G, unsigned char& B)> ModificationRule)
{
	unsigned char* RawData = GetRawData();
	if (RawData == nullptr)
		return;

	for (int i = 0; i < Width * Height * 4; i += 4)
	{
		// Extract the RGB values of the current pixel
		unsigned char R = RawData[i];
		unsigned char G = RawData[i + 1];
		unsigned char B = RawData[i + 2];

		// Apply the modification rule
		ModificationRule(R, G, B);

		// Update the pixel data
		RawData[i] = R;
		RawData[i + 1] = G;
		RawData[i + 2] = B;
	}

	// Update the texture with the modified data
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, RawData);

	// Clean up
	delete[] RawData;
}