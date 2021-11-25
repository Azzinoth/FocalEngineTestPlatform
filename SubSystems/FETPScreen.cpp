#include "FETPScreen.h"

FETPScreen* FETPScreen::_instance = nullptr;

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

bool FETPScreen::searchOnScreen(size_t width, size_t height, unsigned char* data, size_t& x, size_t& y, float correctnessThreshold, int maxColorShift)
{
	updateScreenData();
	// We compare only rgb.
	int dataLenght = width * height * 3;

	float maxDifference = 1.0f - (correctnessThreshold / 100.0f);
	int persentInPixels = int(dataLenght * maxDifference);

	int maxSimilarity = 0;
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

			/*DWORD beginTime = GetTickCount();
			getScreenRegion(screenRegion, i, j, width, height, false);
			totalTimeRegion += GetTickCount() - beginTime;*/

			//unsigned char* start = screenData + (j * TEST_PLATFORM.getScreenWidth() + i) * 4;
			

			
			
			//int similarity = simpleCompare(width, height, screenRegion/*start*/, data);
			int similarity = 0;

			

			int count = 0;

			long long checksumSource = 0;
			long long checksumTarget = 0;

			float difference = 0.0f;
			int localIteraions = 0;
			//std::vector<unsigned char> source;
			//std::vector<unsigned char> lookingFor;
			for (size_t k = j; k < size_t(j + height); k++)
			{
				for (size_t l = i; l < size_t(i + width); l++)
				{
					size_t sourceIndex = (k * TEST_PLATFORM.getScreenWidth() + l) * 4;
					size_t Index = ((k - j) * width + l - i) * 4;

					/*source.push_back(screenData[sourceIndex]);
					source.push_back(screenData[sourceIndex + 1]);
					source.push_back(screenData[sourceIndex + 2]);
					source.push_back(screenData[sourceIndex + 3]);

					lookingFor.push_back(data[Index]);
					lookingFor.push_back(data[Index + 1]);
					lookingFor.push_back(data[Index + 2]);
					lookingFor.push_back(data[Index + 3]);*/


					/*if (data[Index] != screenData[sourceIndex])
						count++;

					if (data[Index + 1] != screenData[sourceIndex + 1])
						count++;

					if (data[Index + 2] != screenData[sourceIndex + 2])
						count++;*/


					if (abs(data[Index] - screenData[sourceIndex]) > maxColorShift)
						count++;

					if (abs(data[Index + 1] - screenData[sourceIndex + 1] > maxColorShift))
						count++;

					if (abs(data[Index + 2] - screenData[sourceIndex + 2] > maxColorShift))
						count++;

					
					/*checksumSource += screenData[sourceIndex];
					checksumSource += screenData[sourceIndex + 1];
					checksumSource += screenData[sourceIndex + 2];

					checksumTarget += data[Index];
					checksumTarget += data[Index + 1];
					checksumTarget += data[Index + 2];

					iteration++;*/
					
					//difference = abs(checksumSource - checksumTarget) / (float(dataLenght) * 255);
					//if (difference > 0.005f)
					//{
					//	break;
					//}

					// Early way out if more than 5 % difference.
					if (count > persentInPixels)
					{
						count = dataLenght;
						break;
					}

					/*localIteraions++;
					if (localIteraions > 10000)
					{
						int y = 0;
						y++;
					}*/
				}

				/*difference = abs(checksumSource - checksumTarget) / (float(dataLenght) * 255);
				if (difference > 0.0005f)
				{
					break;
				}*/

				// Early way out if more than 5 % difference.
				if (count > persentInPixels)
				{
					count = dataLenght;
					break;
				}
			}

			similarity = int(100.0f - float(count) / float(dataLenght) * 100.0f);
			/*float testSimilarity = (1.0f - difference) * 100.0f;
			if (testSimilarity > 99.95f)
			{
				similarity = 99;
			}*/

			//lodepng::encode("C:/Users/Kindr/Desktop/Tests/source.png", source.data(), width, height);
			//lodepng::encode("C:/Users/Kindr/Desktop/Tests/lookingFor.png", lookingFor.data(), width, height);

			//source.clear();
			//lookingFor.clear();

			if (maxSimilarity < similarity)
			{
				maxSimilarity = similarity;
				maxSimilarityX = i;
				maxSimilarityY = j;
			}
				
			//totalTimeCompare += GetTickCount() - beginTime;

			if (similarity >= correctnessThreshold)
			{
				/*int count = 0;
				for (size_t k = j; k < size_t(j + height); k++)
				{
					for (size_t l = i; l < size_t(i + width); l++)
					{
						size_t sourceIndex = (k * TEST_PLATFORM.getScreenWidth() + l) * 4;
						size_t Index = ((k - j) * width + l - i) * 4;

						int difference = abs(data[Index] - screenData[sourceIndex]);
						if (difference > 2)
							count++;

						difference = abs(data[Index + 1] - screenData[sourceIndex + 1]);
						if (difference > 2)
							count++;

						difference = abs(data[Index + 2] - screenData[sourceIndex + 2]);
						if (difference > 2)
							count++;
					}
				}*/

				//int similarity = simpleCompare(width, height, /*screenRegion*/start, data);
				x = i;
				y = j;

				//TEST_PLATFORM.setWindowTitle(std::to_string(totalTimeCompare));

				//std::string text = std::to_string(maxSimilarity) + " " + std::to_string(maxSimilarityX) + " " + std::to_string(maxSimilarityY) + " " + std::to_string(maxDifference);
				//TEST_PLATFORM.setWindowTitle(text);

				//TEST_PLATFORM.setWindowTitle(std::to_string(iteration));
				return true;
			}
		}
	}

	//TEST_PLATFORM.setWindowTitle(std::to_string(totalTimeCompare));
	//std::string text = std::to_string(maxSimilarity) + " " + std::to_string(maxSimilarityX) + " " + std::to_string(maxSimilarityY) + " " + std::to_string(maxDifference);
	//TEST_PLATFORM.setWindowTitle(text);
	//TEST_PLATFORM.setWindowTitle(std::to_string(iteration));
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