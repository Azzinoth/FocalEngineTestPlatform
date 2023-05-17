#include "FETPScreen.h"

FETPScreen* FETPScreen::Instance = nullptr;

FETPScreen::FETPScreen()
{
	screenData = new unsigned char[TEST_PLATFORM.getScreenWidth() * TEST_PLATFORM.getScreenHeight() * 4];
};

unsigned char* FETPScreen::getScreenData()
{
	updateScreenData();
	return screenData;
}

void FETPScreen::updateScreenData()
{
	//if (screenData == nullptr)
	//	screenData = new unsigned char[TEST_PLATFORM.getScreenWidth() * TEST_PLATFORM.getScreenHeight() * 4];


	//LARGE_INTEGER StartingTime;
	//LARGE_INTEGER EndingTime;
	//LARGE_INTEGER ElapsedMicroseconds;
	//LARGE_INTEGER Frequency;
	//QueryPerformanceFrequency(&Frequency);
	//QueryPerformanceCounter(&StartingTime);

	static HDC hScreen = GetDC(GetDesktopWindow());
	static HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, TEST_PLATFORM.getScreenWidth(), TEST_PLATFORM.getScreenHeight());
	static HDC hdcMem = CreateCompatibleDC(hScreen);

	/*HDC hScreen = GetDC(GetDesktopWindow());
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, TEST_PLATFORM.getScreenWidth(), TEST_PLATFORM.getScreenHeight());
	HDC hdcMem = CreateCompatibleDC(hScreen);*/


	

	HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, TEST_PLATFORM.getScreenWidth(), TEST_PLATFORM.getScreenWidth(), hScreen, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);

	



	

	BITMAPINFOHEADER bmi = { 0 };
	bmi.biSize = sizeof(BITMAPINFOHEADER);
	bmi.biPlanes = 1;
	bmi.biBitCount = 32;
	bmi.biWidth = TEST_PLATFORM.getScreenWidth();
	bmi.biHeight = -int(TEST_PLATFORM.getScreenHeight());
	bmi.biCompression = BI_RGB;
	bmi.biSizeImage = 0;

	GetDIBits(hdcMem, hBitmap, 0, TEST_PLATFORM.getScreenHeight(), screenData, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	for (size_t i = 0; i < 4 * TEST_PLATFORM.getScreenWidth() * TEST_PLATFORM.getScreenHeight(); i += 4)
	{
		std::swap(screenData[i], screenData[i + 2]);
	}


	/*ReleaseDC(GetDesktopWindow(), hScreen);
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);*/



	//QueryPerformanceCounter(&EndingTime);
	//ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	//ElapsedMicroseconds.QuadPart *= 1000000;
	//ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

	//float time = ElapsedMicroseconds.QuadPart / 1000.0f;

	


	//TEST_PLATFORM.setWindowTitle(std::to_string(time).c_str());

	//int error = lodepng::encode("test.png", ScreenData, screenW, screenH);
}

void FETPScreen::getScreenRegion(unsigned char* data, int left, int top, int width, int height, bool updateScreenDataFlag)
{
	if (updateScreenDataFlag)
		updateScreenData();

	if (width <= 0 || height <= 0 || top < 0 || left < 0)
		return;

	if (size_t(top + height) > TEST_PLATFORM.getScreenHeight())
		return;

	if (size_t(left + width) > TEST_PLATFORM.getScreenWidth())
		return;

	size_t destenationIndex = 0;
	for (size_t i = top; i < size_t(top + height); i++)
	{
		for (size_t j = left; j < size_t(left + width); j++)
		{
			size_t sourceIndex = (i * TEST_PLATFORM.getScreenWidth() + j) * 4;
			data[destenationIndex++] = screenData[sourceIndex];
			data[destenationIndex++] = screenData[sourceIndex + 1];
			data[destenationIndex++] = screenData[sourceIndex + 2];
			data[destenationIndex++] = screenData[sourceIndex + 3];
		}
	}
}

size_t FETPScreen::getScreenWidth()
{
	return TEST_PLATFORM.getScreenWidth();
}

size_t FETPScreen::getScreenHeight()
{
	return TEST_PLATFORM.getScreenHeight();
}

int FETPScreen::compare(size_t width, size_t height, unsigned char* firstData, unsigned char* secondData, unsigned char* diffData, int maxColorShift)
{
	if (firstData == nullptr || secondData == nullptr)
		return 0;

	int dataLength = width * height * 4;
	int count = 0;
	for (int i = 0; i < dataLength; i+=4)
	{
		int difference = abs(firstData[i] - secondData[i]);
		diffData[i] = difference;

		if (difference > maxColorShift)
			count++;

		difference = abs(firstData[i + 1] - secondData[i + 1]);
		diffData[i] += difference;

		if (difference > maxColorShift)
			count++;

		difference = abs(firstData[i + 2] - secondData[i + 2]);
		diffData[i] += difference;

		if (difference > maxColorShift)
			count++;

		diffData[i] /= 3;
		diffData[i + 1] = diffData[i];
		diffData[i + 2] = diffData[i];
		diffData[i + 3] = 255;
	}

	int similarity = int(100.0f - float(count) / float(dataLength) * 100.0f);
	return similarity;
}

glm::vec2 FETPScreen::imageSizeInRegion(size_t imageW, size_t imageH, size_t regionW, size_t regionH)
{
	if (imageW == 0.0f || imageH == 0.0f)
		return glm::vec2(1.0f);

	glm::vec2 result;
	float aspectRatio = float(imageW) / float(imageH);
	if (imageW > imageH)
	{
		result.x = float(regionW);
		result.y = result.x / aspectRatio;

		if (result.y > regionH)
		{
			result.y = float(regionH);
			result.x = result.y * aspectRatio;
		}
	}
	else
	{
		result.y = float(regionH);
		result.x = result.y * aspectRatio;

		if (result.x > regionW)
		{
			result.x = float(regionW);
			result.y = result.x / aspectRatio;
		}
	}

	return result;
}

int FETPScreen::simpleCompare(size_t width, size_t height, unsigned char* firstData, unsigned char* secondData, int maxColorShift)
{
	if (firstData == nullptr || secondData == nullptr)
		return 0;

	int dataLength = width * height * 4;
	int count = 0;
	for (int i = 0; i < dataLength; i += 4)
	{
		int difference = abs(firstData[i] - secondData[i]);
		if (difference > maxColorShift)
			count++;

		difference = abs(firstData[i + 1] - secondData[i + 1]);
		if (difference > maxColorShift)
			count++;

		difference = abs(firstData[i + 2] - secondData[i + 2]);
		if (difference > maxColorShift)
			count++;

		// Early way out if more than 5 % difference.
		if (float(count) / float(dataLength) * 100.0f > 5)
			return 0;
	}

	int similarity = int(100.0f - float(count) / float(dataLength) * 100.0f);
	return similarity;
}

FETPImage* FETPScreen::GetScreenDataAsImage()
{
	return FETPScreenCapture::getInstance().GetScreenImage();
}

int convertXYtoIndex(size_t width, int x, int y, int imageDepth)
{
	return (y * width + x) * imageDepth;
}

std::vector<long long> convertToIntegralImage(size_t width, size_t height, unsigned char* data, int colorDepth)
{
	std::vector<long long> result_rgb;
	std::vector<long long> result_r;
	std::vector<long long> result_g;
	std::vector<long long> result_b;

	result_rgb.resize(width * height);
	result_r.resize(width * height);
	result_g.resize(width * height);
	result_b.resize(width * height);

	int indexCounter = 0;
	if (colorDepth > 0)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int index = convertXYtoIndex(width, j, i, 4);
				result_r[indexCounter] = data[index];

				if ((j - 1) >= 0)
				{
					result_r[indexCounter] += result_r[convertXYtoIndex(width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					result_r[indexCounter] += result_r[convertXYtoIndex(width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					result_r[indexCounter] -= result_r[convertXYtoIndex(width, j - 1, i - 1, 1)];
				}

				indexCounter++;
			}
		}
	}

	if (colorDepth > 1)
	{
		indexCounter = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int index = convertXYtoIndex(width, j, i, 4);
				result_g[indexCounter] = data[index + 1];

				if ((j - 1) >= 0)
				{
					result_g[indexCounter] += result_g[convertXYtoIndex(width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					result_g[indexCounter] += result_g[convertXYtoIndex(width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					result_g[indexCounter] -= result_g[convertXYtoIndex(width, j - 1, i - 1, 1)];
				}

				indexCounter++;
			}
		}
	}

	if (colorDepth > 2)
	{
		indexCounter = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int index = convertXYtoIndex(width, j, i, 4);
				result_b[indexCounter] = data[index + 2];

				if ((j - 1) >= 0)
				{
					result_b[indexCounter] += result_b[convertXYtoIndex(width, j - 1, i, 1)];
				}

				if ((i - 1) >= 0)
				{
					result_b[indexCounter] += result_b[convertXYtoIndex(width, j, i - 1, 1)];
				}

				if ((j - 1) >= 0 && (i - 1) >= 0)
				{
					result_b[indexCounter] -= result_b[convertXYtoIndex(width, j - 1, i - 1, 1)];
				}

				indexCounter++;
			}
		}
	}

	for (int i = 0; i < result_r.size(); i++)
	{
		result_rgb[i] = result_r[i] + result_g[i] + result_b[i];
	}

	return result_rgb;
}

long long getSubIntegralImage(int x, int y, int subImageW, int subImageH, int totalW, int totalH, std::vector<long long>& integralImage)
{
	long long result = 0;

	subImageW -= 1;
	subImageH -= 1;

	if (x < 0 || y < 0 || subImageW < 1 || subImageH < 1 || x + subImageW > totalW || y + subImageH > totalH)
		return result;

	long long D = integralImage[convertXYtoIndex(totalW, x + subImageW, y + subImageH, 1)];

	long long B = 0;
	if (y - 1 >= 0)
		B = integralImage[convertXYtoIndex(totalW, x + subImageW, y - 1, 1)];

	long long C = 0;
	if (x - 1 >= 0)
		C = integralImage[convertXYtoIndex(totalW, x - 1, y + subImageH, 1)];

	long long A = 0;
	if (x - 1 >= 0 && y - 1 >= 0)
		A = integralImage[convertXYtoIndex(totalW, x - 1, y - 1, 1)];

	result = D - B - C + A;

	return result;
}

bool FETPScreen::searchOnScreen(size_t width, size_t height, unsigned char* data, size_t& x, size_t& y, float correctnessThreshold, int maxColorShift, int* maxSimilarity)
{
	updateScreenData();

	//int dataLenght = width * height * 3;
	//float maxDifference = 1.0f - (correctnessThreshold / 100.0f);
	//int persentInPixels = int(dataLenght * maxDifference);

	//int maxSimilarity = 0;
	//int maxSimilarityX = -1;
	//int maxSimilarityY = -1;

	//int screenW = TEST_PLATFORM.getScreenWidth();
	//int screenH = TEST_PLATFORM.getScreenHeight();

	//std::vector<long long> screenIntegral = convertToIntegralImage(TEST_PLATFORM.getScreenWidth(), TEST_PLATFORM.getScreenHeight(), screenData, 3);
	//std::vector<long long> regionIntegral = convertToIntegralImage(width, height, data, 3);

	//long long lookingFor = regionIntegral.back();

	//float pixelCount = width * height;
	//float threshold = pixelCount * (1.0f - correctnessThreshold / 100.0f);

	//for (size_t i = 0; i < screenH; i++)
	//{
	//	for (size_t j = 0; j < screenW; j++)
	//	{
	//		if (i + height >= screenH ||
	//			j + width >= screenW)
	//			continue;

	//		long long subImage = getSubIntegralImage(j, i, width, height, screenW, screenH, screenIntegral);
	//		long long diff = abs(lookingFor - subImage);

	//		/*if (diff < maxSimilarity)
	//		{
	//			maxSimilarity = diff;
	//			maxSimilarityX = j;
	//			maxSimilarityY = i;
	//		}*/

	//		if (diff == 0/*< threshold*/)
	//		{
	//			/*x = j;
	//			y = i;
	//			return true;*/

	//			int similarity = 0;
	//			int count = 0;

	//			for (size_t k = i; k < size_t(i + height); k++)
	//			{
	//				for (size_t l = j; l < size_t(j + width); l++)
	//				{
	//					size_t sourceIndex = convertXYtoIndex(TEST_PLATFORM.getScreenWidth(), l, k, 4);
	//					//size_t sourceIndex = (k * TEST_PLATFORM.getScreenWidth() + l) * 4;
	//					size_t Index = convertXYtoIndex(width, l - j, k - i, 4);
	//					//size_t Index = ((k - i) * width + l - j) * 4;

	//					if (abs(data[Index] - screenData[sourceIndex]) > maxColorShift)
	//						count++;

	//					if (abs(data[Index + 1] - screenData[sourceIndex + 1] > maxColorShift))
	//						count++;

	//					if (abs(data[Index + 2] - screenData[sourceIndex + 2] > maxColorShift))
	//						count++;

	//					// Early way out if more than 5 % difference.
	//					if (count > persentInPixels)
	//					{
	//						count = dataLenght;
	//						break;
	//					}
	//				}

	//				// Early way out if more than 5 % difference.
	//				if (count > persentInPixels)
	//				{
	//					count = dataLenght;
	//					break;
	//				}
	//			}

	//			similarity = int(100.0f - float(count) / float(dataLenght) * 100.0f);

	//			if (maxSimilarity < similarity)
	//			{
	//				maxSimilarity = similarity;
	//				maxSimilarityX = j;
	//				maxSimilarityY = i;
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
	int dataLenght = width * height * 3;

	float maxDifference = 1.0f - (correctnessThreshold / 100.0f);
	int persentInPixels = int(dataLenght * maxDifference);

	int localMaxSimilarity = 0;
	int maxSimilarityX = -1;
	int maxSimilarityY = -1;

	DWORD totalTimeRegion = 0;
	int iteration = 0;

	for (size_t i = 0; i < getScreenWidth(); i++)
	{
		for (size_t j = 0; j < getScreenHeight(); j++)
		{
			if (j + height >= getScreenHeight() ||
				i + width >= getScreenWidth())
				continue;

			int similarity = 0;
			int count = 0;

			for (size_t k = j; k < size_t(j + height); k++)
			{
				for (size_t l = i; l < size_t(i + width); l++)
				{
					size_t sourceIndex = (k * TEST_PLATFORM.getScreenWidth() + l) * 4;
					size_t Index = ((k - j) * width + l - i) * 4;

					if (abs(data[Index] - screenData[sourceIndex]) > maxColorShift)
						count++;

					if (abs(data[Index + 1] - screenData[sourceIndex + 1]) > maxColorShift)
						count++;

					if (abs(data[Index + 2] - screenData[sourceIndex + 2]) > maxColorShift)
						count++;

					// Early way out if more than 5 % difference.
					if (count > persentInPixels)
					{
						count = dataLenght;
						break;
					}
				}

				// Early way out if more than 5 % difference.
				if (count > persentInPixels)
				{
					count = dataLenght;
					break;
				}
			}

			similarity = int(100.0f - float(count) / float(dataLenght) * 100.0f);

			if (localMaxSimilarity < similarity)
			{
				localMaxSimilarity = similarity;
				maxSimilarityX = i;
				maxSimilarityY = j;
			}
			
			if (similarity >= correctnessThreshold)
			{
				x = i;
				y = j;

				if (maxSimilarity != nullptr)
					*maxSimilarity = localMaxSimilarity;

				return true;
			}
		}
	}

	if (maxSimilarity != nullptr)
		*maxSimilarity = localMaxSimilarity;

	return false;
}

bool screenSearchInfo::getSearchOnScreenMode()
{
	return searchOnScreen;
}

void screenSearchInfo::setSearchOnScreenMode(bool newValue)
{
	searchOnScreen = newValue;

	screenRegionMin = ImVec2(0, 0);
	screenRegionMax = ImVec2(float(TEST_PLATFORM.getScreenWidth()), float(TEST_PLATFORM.getScreenHeight()));
}

void screenSearchInfo::setScreenRegion(ImVec2 ScreenRegionMin, ImVec2 ScreenRegionMax)
{
	if (ScreenRegionMin.x > ScreenRegionMax.x || ScreenRegionMin.y > ScreenRegionMax.y)
		return;

	screenRegionMin = ScreenRegionMin;
	screenRegionMax = ScreenRegionMax;
}

ImVec2 screenSearchInfo::getScreenMinRegion()
{
	return screenRegionMin;
}

ImVec2 screenSearchInfo::getScreenMaxRegion()
{
	return screenRegionMax;
}

int screenSearchInfo::getXShiftFromFound()
{
	return xShiftFromFound;
}

void screenSearchInfo::setXShiftFromFound(int newValue)
{
	xShiftFromFound = newValue;
}

int screenSearchInfo::getYShiftFromFound()
{
	return yShiftFromFound;
}

void screenSearchInfo::setYShiftFromFound(int newValue)
{
	yShiftFromFound = newValue;
}