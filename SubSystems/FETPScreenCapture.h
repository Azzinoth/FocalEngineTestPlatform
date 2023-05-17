#pragma once

#include <atlbase.h>
#include <dxgi1_2.h>
#include <d3d11.h>

#pragma comment(lib, "D3D11.lib")

#include "FETPImage.h"

class FETPScreenCapture
{
public:
	SINGLETON_PUBLIC_PART(FETPScreenCapture)

	FETPImage* GetScreenImage();
private:
	SINGLETON_PRIVATE_PART(FETPScreenCapture)

    struct OutputDuplication
    {
        CComPtr<IDXGIOutputDuplication> outputDuplication;
        OutputDuplication(ID3D11Device* Device);
    };

	CComPtr<ID3D11Device> Device;
	CComPtr<ID3D11DeviceContext> DeviceContext;
	D3D_FEATURE_LEVEL FeatureLevel;

	void GetDesktopImage(IDXGIOutputDuplication* OutputDuplication);
	CComPtr<ID3D11Texture2D> DesktopImageTexture;

	CComPtr<ID3D11Texture2D> StagingTexture;
};