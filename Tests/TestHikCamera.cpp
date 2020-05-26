// Copyright (c) RVBUST, Inc - All rights reserved.

#include <RVC/Camera/CameraBase.h>
#include <stdio.h>

#include <opencv2/opencv.hpp>

int main(int argc, char* agrv[]) {
    CameraSystemBasePtr pcs = GetCameraSystem("Hik");
    if (pcs) {
        pcs->Initialize();
    }
    if (pcs->IsInitialized()) {
        CameraBasePtr cam = pcs->Create("00E12896549", "USB3.0");
        if (0 == cam->Open()) {
            if (cam->IsOpen()) {
                cam->Capture();
            }
        } else {
            printf("cam open failed!\n");
        }
        cam->Close();
        pcs->Terminate();
    }
    return 0;
}
