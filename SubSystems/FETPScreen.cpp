#include "FETPScreen.h"

FETPScreen::FETPScreen()
{
	ScreenData = new unsigned char[TEST_PLATFORM.GetScreenWidth() * TEST_PLATFORM.GetScreenHeight() * 4];
};

unsigned char* FETPScreen::GetScreenData()
{
	UpdateScreenData();
	return ScreenData;
}

void FETPScreen::UpdateScreenData()
{
	static HDC hScreen = GetDC(GetDesktopWindow());
	static HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, static_cast<int>(TEST_PLATFORM.GetScreenWidth()), static_cast<int>(TEST_PLATFORM.GetScreenHeight()));
	static HDC hdcMem = CreateCompatibleDC(hScreen);

	HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, static_cast<int>(TEST_PLATFORM.GetScreenWidth()), static_cast<int>(TEST_PLATFORM.GetScreenWidth()), hScreen, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);
	
	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = static_cast<LONG>(TEST_PLATFORM.GetScreenWidth());
	bmi.biHeight = -int(TEST_PLATFORM.GetScreenHeight());
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;

	GetDIBits(hdcMem, hBitmap, 0, static_cast<UINT>(TEST_PLATFORM.GetScreenHeight()), ScreenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	for (size_t i = 0; i < 4 * TEST_PLATFORM.GetScreenWidth() * TEST_PLATFORM.GetScreenHeight(); i += 4)
	{
		std::swap(ScreenData[i], ScreenData[i + 2]);
	}
}

void FETPScreen::GetScreenRegion(unsigned char* Data, int Left, int Top, int Width, int Height, bool bUpdateScreenDataFlag)
{
	if (bUpdateScreenDataFlag)
		UpdateScreenData();

	if (Width <= 0 || Height <= 0 || Top < 0 || Left < 0)
		return;

	if (size_t(Top + Height) > TEST_PLATFORM.GetScreenHeight())
		return;

	if (size_t(Left + Width) > TEST_PLATFORM.GetScreenWidth())
		return;

	size_t DestinationIndex = 0;
	for (size_t i = Top; i < size_t(Top + Height); i++)
	{
		for (size_t j = Left; j < size_t(Left + Width); j++)
		{
			size_t SourceIndex = (i * TEST_PLATFORM.GetScreenWidth() + j) * 4;
			Data[DestinationIndex++] = ScreenData[SourceIndex];
			Data[DestinationIndex++] = ScreenData[SourceIndex + 1];
			Data[DestinationIndex++] = ScreenData[SourceIndex + 2];
			Data[DestinationIndex++] = ScreenData[SourceIndex + 3];
		}
	}
}

size_t FETPScreen::GetScreenWidth()
{
	return TEST_PLATFORM.GetScreenWidth();
}

size_t FETPScreen::GetScreenHeight()
{
	return TEST_PLATFORM.GetScreenHeight();
}

int FETPScreen::Compare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, unsigned char* DifferenceData, int MaxColorShift)
{
	if (FirstData == nullptr || SecondData == nullptr)
		return 0;

	int DataLength = static_cast<int>(Width * Height * 4);
	int Count = 0;
	for (int i = 0; i < DataLength; i+=4)
	{
		int Difference = abs(FirstData[i] - SecondData[i]);
		DifferenceData[i] = Difference;

		if (Difference > MaxColorShift)
			Count++;

		Difference = abs(FirstData[i + 1] - SecondData[i + 1]);
		DifferenceData[i] += Difference;

		if (Difference > MaxColorShift)
			Count++;

		Difference = abs(FirstData[i + 2] - SecondData[i + 2]);
		DifferenceData[i] += Difference;

		if (Difference > MaxColorShift)
			Count++;

		DifferenceData[i] /= 3;
		DifferenceData[i + 1] = DifferenceData[i];
		DifferenceData[i + 2] = DifferenceData[i];
		DifferenceData[i + 3] = 255;
	}

	int Similarity = int(100.0f - float(Count) / float(DataLength) * 100.0f);
	return Similarity;
}

glm::vec2 FETPScreen::ImageSizeInRegion(size_t ImageWidth, size_t ImageHeight, size_t RegionWidth, size_t RegionHeight)
{
	if (ImageWidth == 0.0f || ImageHeight == 0.0f)
		return glm::vec2(1.0f);

	glm::vec2 Result;
	float AspectRatio = float(ImageWidth) / float(ImageHeight);
	if (ImageWidth > ImageHeight)
	{
		Result.x = float(RegionWidth);
		Result.y = Result.x / AspectRatio;

		if (Result.y > RegionHeight)
		{
			Result.y = float(RegionHeight);
			Result.x = Result.y * AspectRatio;
		}
	}
	else
	{
		Result.y = float(RegionHeight);
		Result.x = Result.y * AspectRatio;

		if (Result.x > RegionWidth)
		{
			Result.x = float(RegionWidth);
			Result.y = Result.x / AspectRatio;
		}
	}

	return Result;
}

int FETPScreen::SimpleCompare(size_t Width, size_t Height, unsigned char* FirstData, unsigned char* SecondData, int MaxColorShift)
{
	if (FirstData == nullptr || SecondData == nullptr)
		return 0;

	int DataLength = static_cast<int>(Width * Height * 4);
	int Count = 0;
	for (int i = 0; i < DataLength; i += 4)
	{
		int Difference = abs(FirstData[i] - SecondData[i]);
		if (Difference > MaxColorShift)
			Count++;

		Difference = abs(FirstData[i + 1] - SecondData[i + 1]);
		if (Difference > MaxColorShift)
			Count++;

		Difference = abs(FirstData[i + 2] - SecondData[i + 2]);
		if (Difference > MaxColorShift)
			Count++;

		// Early way out if more than 5 % difference.
		if (float(Count) / float(DataLength) * 100.0f > 5)
			return 0;
	}

	int Similarity = int(100.0f - float(Count) / float(DataLength) * 100.0f);
	return Similarity;
}

FETPImage* FETPScreen::GetScreenDataAsImage(unsigned int MonitorIndex)
{
	return FETPScreenCapture::GetInstance().GetScreenImage(MonitorIndex);
}

int ConvertXYToIndex(size_t Width, int x, int y, int ImageDepth)
{
	return static_cast<int>((y * Width + x) * ImageDepth);
}

std::vector<long long> ConvertToIntegralImage(size_t Width, size_t Height, unsigned char* Data, int ColorDepth)
{
	std::vector<long long> ResultRGB;
	std::vector<long long> ResultR;
	std::vector<long long> ResultG;
	std::vector<long long> ResultB;

	ResultRGB.resize(Width * Height);
	ResultR.resize(Width * Height);
	ResultG.resize(Width * Height);
	ResultB.resize(Width * Height);

	int IndexCounter = 0;
	if (ColorDepth > 0)
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				int Index = ConvertXYToIndex(Width, j, i, 4);
				ResultR[IndexCounter] = Data[Index];

				if ((j - 1) >= 0)
				{
					ResultR[IndexCounter] += ResultR[ConvertXYToIndex(Width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					ResultR[IndexCounter] += ResultR[ConvertXYToIndex(Width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					ResultR[IndexCounter] -= ResultR[ConvertXYToIndex(Width, j - 1, i - 1, 1)];
				}

				IndexCounter++;
			}
		}
	}

	if (ColorDepth > 1)
	{
		IndexCounter = 0;
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				int Index = ConvertXYToIndex(Width, j, i, 4);
				ResultG[IndexCounter] = Data[Index + 1];

				if ((j - 1) >= 0)
				{
					ResultG[IndexCounter] += ResultG[ConvertXYToIndex(Width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					ResultG[IndexCounter] += ResultG[ConvertXYToIndex(Width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					ResultG[IndexCounter] -= ResultG[ConvertXYToIndex(Width, j - 1, i - 1, 1)];
				}

				IndexCounter++;
			}
		}
	}

	if (ColorDepth > 2)
	{
		IndexCounter = 0;
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				int Index = ConvertXYToIndex(Width, j, i, 4);
				ResultB[IndexCounter] = Data[Index + 2];

				if ((j - 1) >= 0)
				{
					ResultB[IndexCounter] += ResultB[ConvertXYToIndex(Width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					ResultB[IndexCounter] += ResultB[ConvertXYToIndex(Width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					ResultB[IndexCounter] -= ResultB[ConvertXYToIndex(Width, j - 1, i - 1, 1)];
				}

				IndexCounter++;
			}
		}
	}

	for (int i = 0; i < ResultR.size(); i++)
	{
		ResultRGB[i] = ResultR[i] + ResultG[i] + ResultB[i];
	}

	return ResultRGB;
}

long long GetSubIntegralImage(int x, int y, int SubImageWidth, int SubImageHeight, int TotalWidth, int TotalHeight, std::vector<long long>& IntegralImage)
{
	long long Result = 0;

	SubImageWidth -= 1;
	SubImageHeight -= 1;

	if (x < 0 || y < 0 || SubImageWidth < 1 || SubImageHeight < 1 || x + SubImageWidth > TotalWidth || y + SubImageHeight > TotalHeight)
		return Result;

	long long D = IntegralImage[ConvertXYToIndex(TotalWidth, x + SubImageWidth, y + SubImageHeight, 1)];

	long long B = 0;
	if (y - 1 >= 0)
		B = IntegralImage[ConvertXYToIndex(TotalWidth, x + SubImageWidth, y - 1, 1)];

	long long C = 0;
	if (x - 1 >= 0)
		C = IntegralImage[ConvertXYToIndex(TotalWidth, x - 1, y + SubImageHeight, 1)];

	long long A = 0;
	if (x - 1 >= 0 && y - 1 >= 0)
		A = IntegralImage[ConvertXYToIndex(TotalWidth, x - 1, y - 1, 1)];

	Result = D - B - C + A;

	return Result;
}

bool FETPScreen::SearchOnScreen(size_t Width, size_t Height, unsigned char* Data, size_t& X, size_t& Y, float CorrectnessThreshold, int MaxColorShift, int* MaxSimilarity)
{
	UpdateScreenData();

	//int DataLength = Width * Height * 3;
	//float MaxDifference = 1.0f - (correctnessThreshold / 100.0f);
	//int PercentInPixels = int(DataLength * MaxDifference);

	//int maxSimilarity = 0;
	//int MaxSimilarityX = -1;
	//int MaxSimilarityY = -1;

	//int screenW = TEST_PLATFORM.getScreenWidth();
	//int screenH = TEST_PLATFORM.getScreenHeight();

	//std::vector<long long> screenIntegral = ConvertToIntegralImage(TEST_PLATFORM.getScreenWidth(), TEST_PLATFORM.getScreenHeight(), screenData, 3);
	//std::vector<long long> regionIntegral = ConvertToIntegralImage(Width, Height, Data, 3);

	//long long lookingFor = regionIntegral.back();

	//float pixelCount = Width * Height;
	//float threshold = pixelCount * (1.0f - correctnessThreshold / 100.0f);

	//for (size_t i = 0; i < screenH; i++)
	//{
	//	for (size_t j = 0; j < screenW; j++)
	//	{
	//		if (i + Height >= screenH ||
	//			j + Width >= screenW)
	//			continue;

	//		long long subImage = GetSubIntegralImage(j, i, Width, Height, screenW, screenH, screenIntegral);
	//		long long diff = abs(lookingFor - subImage);

	//		/*if (diff < maxSimilarity)
	//		{
	//			maxSimilarity = diff;
	//			MaxSimilarityX = j;
	//			MaxSimilarityY = i;
	//		}*/

	//		if (diff == 0/*< threshold*/)
	//		{
	//			/*x = j;
	//			y = i;
	//			return true;*/

	//			int similarity = 0;
	//			int Count = 0;

	//			for (size_t k = i; k < size_t(i + Height); k++)
	//			{
	//				for (size_t l = j; l < size_t(j + Width); l++)
	//				{
	//					size_t SourceIndex = ConvertXYToIndex(TEST_PLATFORM.getScreenWidth(), l, k, 4);
	//					//size_t SourceIndex = (k * TEST_PLATFORM.getScreenWidth() + l) * 4;
	//					size_t Index = ConvertXYToIndex(Width, l - j, k - i, 4);
	//					//size_t Index = ((k - i) * Width + l - j) * 4;

	//					if (abs(Data[Index] - screenData[SourceIndex]) > maxColorShift)
	//						Count++;

	//					if (abs(Data[Index + 1] - screenData[SourceIndex + 1] > maxColorShift))
	//						Count++;

	//					if (abs(Data[Index + 2] - screenData[SourceIndex + 2] > maxColorShift))
	//						Count++;

	//					// Early way out if more than 5 % difference.
	//					if (Count > PercentInPixels)
	//					{
	//						Count = DataLength;
	//						break;
	//					}
	//				}

	//				// Early way out if more than 5 % difference.
	//				if (Count > PercentInPixels)
	//				{
	//					Count = DataLength;
	//					break;
	//				}
	//			}

	//			similarity = int(100.0f - float(Count) / float(DataLength) * 100.0f);

	//			if (maxSimilarity < similarity)
	//			{
	//				maxSimilarity = similarity;
	//				MaxSimilarityX = j;
	//				MaxSimilarityY = i;
	//			}

	//			if (similarity >= correctnessThreshold)
	//			{
	//				x = j;
	//				y = i;

	//				return true;
	//			}
	//		}
	//	}
	//}

	//return false;


	// We compare only rgb.
	int DataLength = static_cast<int>(Width * Height * 3);

	float MaxDifference = 1.0f - (CorrectnessThreshold / 100.0f);
	int PercentInPixels = int(DataLength * MaxDifference);

	int LocalMaxSimilarity = 0;
	int MaxSimilarityX = -1;
	int MaxSimilarityY = -1;

	DWORD TotalTimeRegion = 0;
	int Iteration = 0;

	for (size_t i = 0; i < GetScreenWidth(); i++)
	{
		for (size_t j = 0; j < GetScreenHeight(); j++)
		{
			if (j + Height >= GetScreenHeight() ||
				i + Width >= GetScreenWidth())
				continue;

			int Similarity = 0;
			int Count = 0;

			for (size_t k = j; k < size_t(j + Height); k++)
			{
				for (size_t l = i; l < size_t(i + Width); l++)
				{
					size_t SourceIndex = (k * TEST_PLATFORM.GetScreenWidth() + l) * 4;
					size_t Index = ((k - j) * Width + l - i) * 4;

					if (abs(Data[Index] - ScreenData[SourceIndex]) > MaxColorShift)
						Count++;

					if (abs(Data[Index + 1] - ScreenData[SourceIndex + 1]) > MaxColorShift)
						Count++;

					if (abs(Data[Index + 2] - ScreenData[SourceIndex + 2]) > MaxColorShift)
						Count++;

					// Early way out if more than 5 % difference.
					if (Count > PercentInPixels)
					{
						Count = DataLength;
						break;
					}
				}

				// Early way out if more than 5 % difference.
				if (Count > PercentInPixels)
				{
					Count = DataLength;
					break;
				}
			}

			Similarity = int(100.0f - float(Count) / float(DataLength) * 100.0f);

			if (LocalMaxSimilarity < Similarity)
			{
				LocalMaxSimilarity = Similarity;
				MaxSimilarityX = static_cast<int>(i);
				MaxSimilarityY = static_cast<int>(j);
			}
			
			if (Similarity >= CorrectnessThreshold)
			{
				X = i;
				Y = j;

				if (MaxSimilarity != nullptr)
					*MaxSimilarity = LocalMaxSimilarity;

				return true;
			}
		}
	}

	if (MaxSimilarity != nullptr)
		*MaxSimilarity = LocalMaxSimilarity;

	return false;
}