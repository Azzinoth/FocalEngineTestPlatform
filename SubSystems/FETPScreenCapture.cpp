#include "FETPScreenCapture.h"

FETPScreenCapture* FETPScreenCapture::Instance = nullptr;

FETPScreenCapture::FETPScreenCapture()
{
    static const D3D_DRIVER_TYPE DriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };

    static const D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };

    for (const auto& driverType : DriverTypes)
    {
        const auto OperationResult = D3D11CreateDevice(
            nullptr,
            driverType,
            nullptr,
            0,
            FeatureLevels,
            static_cast<UINT>(std::size(FeatureLevels)),
            D3D11_SDK_VERSION,
            &Device,
            &FeatureLevel,
            &DeviceContext
        );

        if (SUCCEEDED(OperationResult))
            break;

        Device->Release();
        DeviceContext->Release();
    }

    // Create StagingTexture which represents duplication image of desktop.
    //Duplication = new OutputDuplication(Device);
    OutputDuplication Duplication(Device);
    IDXGIOutputDuplication* outputDuplication = Duplication.outputDuplication;
    if (outputDuplication == nullptr)
        return;

    GetDesktopImage(outputDuplication);
    ID3D11Texture2D* AcquiredDesktopImage = DesktopImageTexture;
    if (AcquiredDesktopImage == nullptr)
        return;

    DXGI_OUTDUPL_DESC DuplicationDescription;
    outputDuplication->GetDesc(&DuplicationDescription);
    //Duplication->outputDuplication->GetDesc(&DuplicationDescription);

    const auto CurrentFormat = static_cast<int>(DuplicationDescription.ModeDesc.Format);
    const auto AcceptableFormat =
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    if (!AcceptableFormat)
        return;

    D3D11_TEXTURE2D_DESC TextureDescription{};
    TextureDescription.Width = DuplicationDescription.ModeDesc.Width;
    TextureDescription.Height = DuplicationDescription.ModeDesc.Height;
    TextureDescription.Format = DuplicationDescription.ModeDesc.Format;
    TextureDescription.ArraySize = 1;
    TextureDescription.BindFlags = 0;
    TextureDescription.MiscFlags = 0;
    TextureDescription.SampleDesc.Count = 1;
    TextureDescription.SampleDesc.Quality = 0;
    TextureDescription.MipLevels = 1;
    TextureDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    TextureDescription.Usage = D3D11_USAGE_STAGING;

    const auto hr = Device->CreateTexture2D(&TextureDescription, nullptr, &StagingTexture);
    if (FAILED(hr))
        return;
    
    if (StagingTexture == nullptr)
        return;

    Duplication.outputDuplication->Release();
}

FETPScreenCapture::OutputDuplication::OutputDuplication(ID3D11Device* Device, UINT MonitorIndex)
{
    HRESULT OperationResult;

    IDXGIDevice* dxgiDevice = nullptr;
    OperationResult = Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(OperationResult))
        return;

    IDXGIAdapter* dxgiAdapter;
    OperationResult = dxgiDevice->GetParent(__uuidof(dxgiAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    if (FAILED(OperationResult))
    {
        dxgiDevice->Release();
        return;
    }

    IDXGIOutput* dxgiOutput;
    OperationResult = dxgiAdapter->EnumOutputs(MonitorIndex, &dxgiOutput);
    if (FAILED(OperationResult))
    {
        dxgiAdapter->Release();
        dxgiDevice->Release();
        return;
    }

    IDXGIOutput1* dxgiOutput1;
    OperationResult = dxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&dxgiOutput1));
    if (FAILED(OperationResult))
    {
        dxgiOutput->Release();
        dxgiAdapter->Release();
        dxgiDevice->Release();
        return;
    }

    OperationResult = dxgiOutput1->DuplicateOutput(Device, &outputDuplication);
    if (FAILED(OperationResult))
    {
        dxgiOutput1->Release();
        dxgiOutput->Release();
        dxgiAdapter->Release();
        dxgiDevice->Release();
        return;
    }

    dxgiOutput1->Release();
    dxgiOutput->Release();
    dxgiAdapter->Release();
    dxgiDevice->Release();
}

void FETPScreenCapture::GetDesktopImage(IDXGIOutputDuplication* OutputDuplication)
{
    IDXGIResource* DesktopResource;
    HRESULT OperationResult = E_FAIL;
    for (int i = 0; i < 10; i++)
    {
        DXGI_OUTDUPL_FRAME_INFO fi{};
        const int timeoutMsec = 500; // Milliseconds.
        OperationResult = OutputDuplication->AcquireNextFrame(timeoutMsec, &fi, &DesktopResource);
        if (SUCCEEDED(OperationResult) && (fi.LastPresentTime.QuadPart == 0))
        {
            // If AcquireNextFrame() returns S_OK and
            // fi.LastPresentTime.QuadPart == 0, it means
            // AcquireNextFrame() didn't acquire next frame yet.
            // We must wait next frame sync timing to retrieve
            // actual frame data.
            //
            // Since method is successfully completed,
            // we need to release the resource and frame explicitly.
            DesktopResource->Release();
            OutputDuplication->ReleaseFrame();
            Sleep(1);
            continue;
        }
        else
        {
            break;
        }
    }

    if (FAILED(OperationResult))
        return;
    
    if (DesktopImageTexture != nullptr)
        DesktopImageTexture->Release();

    OperationResult = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&DesktopImageTexture));
    if (FAILED(OperationResult))
        return;
}

FETPImage* FETPScreenCapture::GetScreenImage(UINT MonitorIndex)
{
    FETPImage* Result = nullptr;

    OutputDuplication Duplication(Device, MonitorIndex);
    IDXGIOutputDuplication* outputDuplication = Duplication.outputDuplication;
    if (outputDuplication == nullptr)
        return Result;

    GetDesktopImage(outputDuplication);
    ID3D11Texture2D* AcquiredDesktopImage = DesktopImageTexture;
    if (AcquiredDesktopImage == nullptr)
        return Result;

    DXGI_OUTDUPL_DESC DuplicationDescription;
    outputDuplication->GetDesc(&DuplicationDescription);
    //Duplication->outputDuplication->GetDesc(&DuplicationDescription);

    const auto CurrentFormat = static_cast<int>(DuplicationDescription.ModeDesc.Format);
    const auto AcceptableFormat =
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    if (!AcceptableFormat)
        return Result;

    DeviceContext->CopyResource(StagingTexture, AcquiredDesktopImage);
    AcquiredDesktopImage->Release();

    D3D11_TEXTURE2D_DESC TextureDescription;
    StagingTexture->GetDesc(&TextureDescription);

    D3D11_MAPPED_SUBRESOURCE Resource;
    const auto OperationResult = DeviceContext->Map(StagingTexture, D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &Resource);
    if (FAILED(OperationResult))
        return Result;

    int Width = static_cast<int>(TextureDescription.Width);
    int Height = static_cast<int>(TextureDescription.Height);
    int RawDataSize = Resource.RowPitch * TextureDescription.Height;
    unsigned char* RawData = new unsigned char[RawDataSize];
    memcpy(RawData, Resource.pData, RawDataSize);
    DeviceContext->Unmap(StagingTexture, 0);

    // Swap R and B channels.
    for (int i = 0; i < RawDataSize / 4; i++)
    {
        unsigned char* Pixel = RawData + i * 4;
        unsigned char Blue = Pixel[0];
        Pixel[0] = Pixel[2];
        Pixel[2] = Blue;
    }

    Result = new FETPImage(RawData, Width, Height);
    delete[] RawData;
    Duplication.outputDuplication->Release();

    return Result;
}