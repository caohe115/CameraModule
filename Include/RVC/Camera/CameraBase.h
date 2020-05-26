// Copyright (c) RVBUST, Inc - All rights reserved.
#pragma once

#include <memory>

struct CameraBase {
    virtual ~CameraBase() {}
    virtual int Open() = 0;
    virtual int Close() = 0;
    virtual bool IsOpen() = 0;
    virtual int Capture() = 0;
};

using CameraBasePtr = std::shared_ptr<CameraBase>;

struct CameraSystemBase {
    virtual ~CameraSystemBase() {}
    virtual int Initialize() = 0;
    virtual int Terminate() = 0;
    virtual bool IsInitialized() = 0;
    virtual CameraBasePtr Create(const char* sn, const char* type) = 0;
};

using CameraSystemBasePtr = std::shared_ptr<CameraSystemBase>;

CameraSystemBasePtr GetCameraSystem(const char* brand);
