// Copyright (c) RVBUST, Inc - All rights reserved.
#pragma once
#include <RVC/Camera/CameraBase.h>
#include <pylon/CameraEventHandler.h>
#include <pylon/ConfigurationEventHandler.h>
#include <pylon/DeviceInfo.h>
#include <pylon/EnumParameterT.h>
#include <pylon/ImageEventHandler.h>
#include <pylon/InstantCamera.h>
#include <pylon/ParameterIncludes.h>
#include <pylon/Platform.h>
#include <pylon/PylonIncludes.h>
#include <pylon/SoftwareTriggerConfiguration.h>
#include <pylon/gige/ActionTriggerConfiguration.h>
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/gige/PylonGigE.h>
#include <pylon/gige/PylonGigEIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <pylon/usb/_BaslerUsbCameraParams.h>

#include <opencv2/opencv.hpp>

#include "CImageEventHandler.h"
#include "ImageSize.h"

typedef Pylon::CBaslerUsbInstantCamera Camera_t;

struct BaslerCameraSystem : public CameraSystemBase {
    BaslerCameraSystem();
    ~BaslerCameraSystem();
    int Initialize();
    int Terminate();
    bool IsInitialized();
    CameraBasePtr Create(const char *sn, const char *type);
};

struct BaslerCameraUSB : public CameraBase {
    BaslerCameraUSB(const char *sn);
    ~BaslerCameraUSB();

    int Open();
    int Close();
    bool IsOpen();
    int Capture();
    unsigned char *m_imgdata{nullptr};
    unsigned char *m_last_imgptr{nullptr};
    std::unique_ptr<Camera_t> m_pcam{nullptr};
};
