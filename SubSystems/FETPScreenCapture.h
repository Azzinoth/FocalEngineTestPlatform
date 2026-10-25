#pragma once

#include "FETPImage.h"

#include <dxgi1_2.h>
#include <d3d11.h>

#pragma comment(lib, "D3D11.lib")

class FETPScreenCapture
{
public:
	SINGLETON_PUBLIC_PART(FETPScreenCapture)

	FETPImage* GetScreenImage(unsigned int MonitorIndex = 0, bool bGenerateMipmaps = true);
private:
	SINGLETON_PRIVATE_PART(FETPScreenCapture)

    struct OutputDuplication
    {
        IDXGIOutputDuplication* outputDuplication = nullptr;
        OutputDuplication(ID3D11Device* Device, unsigned int MonitorIndex = 0);
    };

	struct MonitorCapture
	{
		IDXGIOutputDuplication* Duplication = nullptr;
		ID3D11Texture2D* StagingTexture = nullptr;
		bool bHasFrame = false;
	};

	ID3D11Device* Device = nullptr;
	ID3D11DeviceContext* DeviceContext = nullptr;
	D3D_FEATURE_LEVEL FeatureLevel;

	void InitializeDirectX();

	std::vector<MonitorCapture> Monitors;
	bool InitializeMonitor(unsigned int MonitorIndex);
	void ReleaseMonitor(unsigned int MonitorIndex);
	HRESULT UpdateMonitorFrame(unsigned int MonitorIndex);
};