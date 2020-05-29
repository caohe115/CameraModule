// Copyright (c) RVBUST, Inc - All rights reserved.

#include "BaslerCamera.h"

#include <atomic>
#include <iostream>

using namespace std;
using namespace Pylon;
using namespace GenApi;

static struct
{
    std::atomic<bool> is_initialized{false};
} sg_stat;

BaslerCameraSystem::BaslerCameraSystem()
{
    RVC_DEBUG("Construct BaslerCameraSystem!");
}

BaslerCameraSystem::~BaslerCameraSystem()
{
    RVC_DEBUG("Deconstruct BaslerCameraSystem!");
}

int BaslerCameraSystem::Initialize()
{
    if (sg_stat.is_initialized.load() == false)
    {
        PylonInitialize();
        RVC_DEBUG("Success Initialize!");
        sg_stat.is_initialized.store(true);
        return 0;
    }
    else
    {
        RVC_ERROR("Failed Initialize! Please use Terminate, before you use Initialize!");
        return -1;
    }
}

bool BaslerCameraSystem::IsInitialized()
{
    return sg_stat.is_initialized.load();
}

int BaslerCameraSystem::Terminate()
{
    if (sg_stat.is_initialized.load() == true)
    {
        PylonTerminate();
        RVC_DEBUG("Success PylonTerminate!");
        sg_stat.is_initialized.store(false);
        return 0;
    }
    else
    {
        RVC_ERROR("Failed Terminate! Please use Initialize, before you use Terminate!");
        return -1;
    }
}

CameraBasePtr BaslerCameraSystem::Create(const char *sn, const char *type)
{
    if (strcmp(type, "USB3.0") == 0)
    {
        RVC_DEBUG("Success Create BaslerCameraUSB ptr!");
        return std::make_shared<BaslerCameraUSB>(sn);
    }
    else
    {
        RVC_ERROR("Failed create BaslerCameraSystem! type only support 'USB3.0' or 'GigE'!");
        return nullptr;
    }
}

BaslerCameraUSB::BaslerCameraUSB(const char *sn)
{
    try
    {
        CBaslerUsbDeviceInfo info;
        info.SetSerialNumber(sn);
        IPylonDevice *pDevice = CTlFactory::GetInstance().CreateFirstDevice(info);
        if (pDevice == nullptr)
        {
            m_pcam = nullptr;
            RVC_ERROR("Failed Create BaslerCameraUSB Device!");
        }
        else
        {
            m_pcam = std::make_unique<Camera_t>(pDevice);
            RVC_DEBUG("Success Create BaslerCameraUSB Device!");
        }
        m_imgdata = (unsigned char *)malloc(sg_config.img_num_bytes * sg_config.max_num_frames);
        m_last_imgptr = m_imgdata + sg_config.img_num_bytes * (sg_config.max_num_frames - 1);
        RVC_DEBUG("Success CreateFirstDevice of BaslerCameraUSB!");
    }
    catch (const GenericException &e)
    {
        cout << "Failed CreateFirstDevice of BaslerCameraUSB, cause by: " << e.GetDescription()
             << endl;
        m_pcam = nullptr;
    }
}

BaslerCameraUSB::~BaslerCameraUSB()
{
    free(m_imgdata);
    m_imgdata = nullptr;
    if (m_pcam != nullptr)
    {
        if (m_pcam->IsOpen())
        {
            RVC_DEBUG("When Deconstruct BaslerCameraUSB, cause m_pcam is not open!");
            m_pcam->Close();
        }
    }
    RVC_DEBUG("Deconstruction of BaslerCameraUSB!");
}

int BaslerCameraUSB::Open()
{
    try
    {
        if (m_pcam == nullptr)
        {
            RVC_ERROR("Failed Open BaslerCameraUSB, cause camera is nullptr!");
            return -1;
        }
        if (m_pcam->IsOpen())
        {
            RVC_ERROR("Failed Open BaslerCameraUSB, cause camera is already open!");
            return -1;
        }
        m_pcam->Open();
        if (m_pcam->IsOpen())
        {
            RVC_DEBUG("Success Opened BaslerCameraUSB!");
            return 0;
        }
        else
        {
            RVC_ERROR("Failed Opened BaslerCameraUSB!");
            return -1;
        }
    }
    catch (const GenericException &e)
    {
        cout << "Failed Open of BaslerCameraUSB, cause by:" << e.GetDescription() << endl;
        m_pcam = nullptr;
        return -1;
    }
}

int BaslerCameraUSB::Close()
{
    if (m_pcam == nullptr)
    {
        RVC_ERROR("Failed Close BaslerCameraUSB, cause m_pcam is nullptr!");
        return -1;
    }
    else
    {
        if (m_pcam->IsOpen())
        {
            m_pcam->Close();
            RVC_DEBUG("Success Close BaslerCameraUSB!");
            return 0;
        }
        else
        {
            RVC_ERROR("Failed Close BaslerCameraUSB, cause camera is already close!");
            return -1;
        }
    }
}

bool BaslerCameraUSB::IsOpen()
{
    if (m_pcam == nullptr)
    {
        RVC_ERROR("Failed execute IsOpen function, cause camera is nullptr!");
        return false;
    }
    if (m_pcam->IsOpen())
    {
        RVC_DEBUG("Is Opened of BaslerCameraUSB!");
        return true;
    }
    else
    {
        RVC_DEBUG("Is not Opened of BaslerCameraUSB!");
        return false;
    }
}

int BaslerCameraUSB::Capture()
{
    if (m_pcam == nullptr)
    {
        RVC_ERROR("Failed Capture BaslerCameraUSB, cause camera is nullptr!");
        return -1;
    }
    int imgHeight = 0;
    int imgWidth = 0;
    if (m_pcam->IsOpen())
    {
        CGrabResultPtr ptrGrabResult;
        m_pcam->StartGrabbing(1);
        while (m_pcam->IsGrabbing())
        {
            m_pcam->RetrieveResult(5000, ptrGrabResult, TimeoutHandling_Return);
            if (ptrGrabResult.IsValid())
            {
                RVC_DEBUG("ptrGrabResult is valid!\n");
            }
            else
            {
                RVC_DEBUG("ptrGrabResult is not valid!\n");
            }
            if (ptrGrabResult->GrabSucceeded())
            {
                imgHeight = ptrGrabResult->GetHeight();
                imgWidth = ptrGrabResult->GetWidth();
                const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
                memcpy(m_last_imgptr, pImageBuffer, sizeof(uint8_t) * imgHeight * imgWidth);
                std::cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0]
                          << std::endl;
            }
            else
            {
                RVC_ERROR("Failed Grab!");
                return -1;
            }
        }
    }
    RVC_DEBUG("Success Capture!");
    return 0;
}