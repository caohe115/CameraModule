// Copyright (c) RVBUST, Inc - All rights reserved.
#pragma once
#include <MvDeviceBase.h>
#include <RVC/Camera/CameraBase.h>

#include <memory>

struct HikCameraSystem : public CameraSystemBase {
    HikCameraSystem();
    ~HikCameraSystem();
    int Initialize();
    int Terminate();
    bool IsInitialized();
    CameraBasePtr Create(const char* sn, const char* type);
};

struct HikCameraUSB : public CameraBase {
    HikCameraUSB(const char* sn);
    ~HikCameraUSB();
    int Open();
    int Close();
    bool IsOpen();
    int Capture();
    void* hik_handle{nullptr};
    unsigned char* m_imgdata{nullptr};
    unsigned char* m_last_imgptr{nullptr};
    bool f_isOpen = false;
};
