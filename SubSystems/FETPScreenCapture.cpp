#include "FETPScreenCapture.h"

FETPScreenCapture::FETPScreenCapture()
{
	InitializeDirectX();

    // Duplications are kept alive, so desktop updates that happen before the first screenshot are already accumulated.
    Monitors.resize(FocalEngine::APPLICATION.GetMonitors().size());
    for (size_t i = 0; i < Monitors.size(); i++)
        InitializeMonitor(static_cast<unsigned int>(i));
}

bool FETPScreenCapture::InitializeMonitor(unsigned int MonitorIndex)
{
    MonitorCapture& Capture = Monitors[MonitorIndex];

    OutputDuplication Duplication(Device, MonitorIndex);
    Capture.Duplication = Duplication.outputDuplication;
    if (Capture.Duplication == nullptr)
        return false;

    DXGI_OUTDUPL_DESC DuplicationDescription;
    Capture.Duplication->GetDesc(&DuplicationDescription);

    const auto CurrentFormat = static_cast<int>(DuplicationDescription.ModeDesc.Format);
    const auto AcceptableFormat =
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
        CurrentFormat == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

    if (!AcceptableFormat)
    {
        ReleaseMonitor(MonitorIndex);
        return false;
    }

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

    HRESULT OperationResult = Device->CreateTexture2D(&TextureDescription, nullptr, &Capture.StagingTexture);
    if (FAILED(OperationResult))
    {
        ReleaseMonitor(MonitorIndex);
        return false;
    }

    return true;
}

void FETPScreenCapture::ReleaseMonitor(unsigned int MonitorIndex)
{
    MonitorCapture& Capture = Monitors[MonitorIndex];

    if (Capture.StagingTexture != nullptr)
    {
        Capture.StagingTexture->Release();
        Capture.StagingTexture = nullptr;
    }

    if (Capture.Duplication != nullptr)
    {
        Capture.Duplication->Release();
        Capture.Duplication = nullptr;
    }

    Capture.bHasFrame = false;
}

void FETPScreenCapture::InitializeDirectX()
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

    for (const auto& DriverType : DriverTypes)
    {
        const auto OperationResult = D3D11CreateDevice(
            nullptr,
            DriverType,
            nullptr,
            0,
            FeatureLevels,
            static_cast<unsigned int>(std::size(FeatureLevels)),
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
}

FETPScreenCapture::OutputDuplication::OutputDuplication(ID3D11Device* Device, unsigned int MonitorIndex)
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

HRESULT FETPScreenCapture::UpdateMonitorFrame(unsigned int MonitorIndex)
{
    MonitorCapture& Capture = Monitors[MonitorIndex];

    // Once the staging texture holds a frame, only copy when the desktop changed (LastPresentTime != 0).
    // Until then wait, because the first frame of a new duplication is black and has LastPresentTime == 0.
    for (int i = 0; i < 10; i++)
    {
        DXGI_OUTDUPL_FRAME_INFO FrameInfo{};
        IDXGIResource* DesktopResource = nullptr;
        const unsigned int TimeoutInMilliseconds = Capture.bHasFrame ? 0 : 500;
        HRESULT OperationResult = Capture.Duplication->AcquireNextFrame(TimeoutInMilliseconds, &FrameInfo, &DesktopResource);
        if (OperationResult == DXGI_ERROR_WAIT_TIMEOUT && Capture.bHasFrame)
            return S_OK;

        if (FAILED(OperationResult))
            return OperationResult;

        if (FrameInfo.LastPresentTime.QuadPart != 0)
        {
            ID3D11Texture2D* DesktopTexture = nullptr;
            if (SUCCEEDED(DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&DesktopTexture))))
            {
                DeviceContext->CopyResource(Capture.StagingTexture, DesktopTexture);
                DesktopTexture->Release();
                Capture.bHasFrame = true;
            }
        }

        DesktopResource->Release();
        Capture.Duplication->ReleaseFrame();

        if (Capture.bHasFrame)
            return S_OK;
    }

    return DXGI_ERROR_WAIT_TIMEOUT;
}

FETPImage* FETPScreenCapture::GetScreenImage(unsigned int MonitorIndex, bool bGenerateMipmaps)
{
    if (MonitorIndex >= Monitors.size())
        return nullptr;

    MonitorCapture& Capture = Monitors[MonitorIndex];
    if (Capture.Duplication == nullptr && !InitializeMonitor(MonitorIndex))
        return nullptr;

    HRESULT OperationResult = UpdateMonitorFrame(MonitorIndex);
    if (OperationResult == DXGI_ERROR_ACCESS_LOST)
    {
        // Happens on mode changes, fullscreen applications and desktop switches.
        ReleaseMonitor(MonitorIndex);
        if (!InitializeMonitor(MonitorIndex))
            return nullptr;

        OperationResult = UpdateMonitorFrame(MonitorIndex);
    }

    if (FAILED(OperationResult))
        return nullptr;

    D3D11_TEXTURE2D_DESC TextureDescription;
    Capture.StagingTexture->GetDesc(&TextureDescription);

    D3D11_MAPPED_SUBRESOURCE Resource;
    OperationResult = DeviceContext->Map(Capture.StagingTexture, D3D11CalcSubresource(0, 0, 0), D3D11_MAP_READ, 0, &Resource);
    if (FAILED(OperationResult))
        return nullptr;

    // Desktop is BGRA, GL swaps the channels during the upload.
    FETPImage* Result = new FETPImage();
    Result->Initialize(static_cast<unsigned char*>(Resource.pData), static_cast<int>(TextureDescription.Width), static_cast<int>(TextureDescription.Height), GL_BGRA, static_cast<int>(Resource.RowPitch), bGenerateMipmaps);
    DeviceContext->Unmap(Capture.StagingTexture, 0);

    return Result;
}