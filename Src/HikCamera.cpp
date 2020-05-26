#include "HikCamera.h"

#include <CameraParams.h>
#include <MvCameraControl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <atomic>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace MvCamCtrl;

void static RVC_ERROR(const char* msg) {
    printf("%s\n", msg);
}

void static RVC_DEBUG(const char* msg) {
    printf("%s\n", msg);
}
void static RVC_INFO(const char* msg) {
    printf("%s\n", msg);
}
static struct { std::atomic<bool> is_initialized{false}; } sg_stat;
HikCameraSystem::HikCameraSystem() {
    RVC_DEBUG("Construct of HikCameraSystem!");
}

HikCameraSystem::~HikCameraSystem() {
    RVC_DEBUG("Deconstruct of HikCameraSystem!");
}
int HikCameraSystem::Initialize() {
    if (sg_stat.is_initialized.load() == false) {
        sg_stat.is_initialized.store(true);
        RVC_DEBUG("Success Initialize the Hik system!");
        return 0;
    } else {
        RVC_ERROR("Failed Initialize, cause has been Initialize!");
        return -1;
    }
}
int HikCameraSystem::Terminate() {
    if (sg_stat.is_initialized.load() == true) {
        sg_stat.is_initialized.store(false);
        RVC_DEBUG("Success Terminate the Hik system!");
        return 0;
    } else {
        RVC_ERROR("Failed Terminate the Hik system!");
        return -1;
    }
}

bool HikCameraSystem::IsInitialized() {
    return sg_stat.is_initialized.load();
}

CameraBasePtr HikCameraSystem::Create(const char* sn, const char* type) {
    if (strcmp(type, "USB3.0") == 0) {
        RVC_DEBUG("Success Create Hik USB3.0!");
        return std::make_shared<HikCameraUSB>(sn);
    } else {
        RVC_DEBUG("Failed Create Hik USB3.0! type only support 'USB3.0' 'GigE'");
        return nullptr;
    }
}
unsigned char* twenty_Data = (unsigned char*)malloc(sizeof(unsigned char) * 1080 * 1440 * 20);
HikCameraUSB::HikCameraUSB(const char* sn) {
    f_isOpen = false;
    int imgSize = 1440 * 1080;
    m_imgdata = (unsigned char*)malloc(sizeof(unsigned char) * imgSize * 21);
    m_last_imgptr = m_imgdata + imgSize * 20;
    // enum device
    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));
    if (MV_CC_EnumDevices(MV_USB_DEVICE, &stDeviceList) == MV_OK) {
        RVC_DEBUG("Success Enum USB Devices!");
    } else {
        RVC_ERROR("Failed Enum USB Devices!");
    }

    if (stDeviceList.nDeviceNum > 0) {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++) {
            // printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            RVC_DEBUG("Success obtain DeviceInfo!");
        }
        // RVC_INFO("Please Intput camera index:");
        unsigned int nIndex = 0;
        // scanf("%d", &nIndex);
        if (nIndex >= stDeviceList.nDeviceNum) {
            RVC_ERROR("Error, input index is not valid!");
        }
        // select device and create handle
        if (MV_CC_CreateHandle(&hik_handle, stDeviceList.pDeviceInfo[nIndex]) == MV_OK) {
            // RVC_DEBUG("Success create handle!");
        } else {
            RVC_ERROR("Failed create handle!");
        }
        // RVC_DEBUG("Construct of HikCameraUSB!");
    } else {
        RVC_INFO("Find No Devices!\n");
    }
}
HikCameraUSB::~HikCameraUSB() {
    free(m_imgdata);
    m_imgdata = nullptr;
    // RVC_DEBUG("Deconstruct of HikCameraUSB!");
}

bool g_bExit = false;

// wait for user to input enter to stop grabbing or end the sample program
void PressEnterToExit(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
    fprintf(stderr, "\nPress enter to exit.\n");
    while (getchar() != '\n')
        ;
    g_bExit = true;
    sleep(1);
}

static void* WorkThread(void* pUser) {
    int nRet = MV_OK;
    // Get payload size
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    if (MV_CC_GetIntValue(pUser, "PayloadSize", &stParam) == MV_OK) {
        RVC_DEBUG("Success PayloadSize!");
    } else {
        RVC_ERROR("Failed PayloadSize!");
        return nullptr;
    }
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    unsigned char* pData = (unsigned char*)malloc(sizeof(unsigned char) * stParam.nCurValue);
    unsigned int nDataSize = stParam.nCurValue;
    // obtain 20 images in hardware triggle mode
    RVC_INFO("You can start camera hardware triggle!");
    nRet = MV_CC_SetCommandValue(pUser, "TriggerHardware");
    int imgCount = 0;
    while (imgCount < 20) {
        if (MV_CC_GetOneFrameTimeout(pUser, pData, nDataSize, &stImageInfo, 1000) == MV_OK) {
            printf("GetFrame %d, Width[%d], Height[%d], nFrameNum[%d]\n", imgCount,
                   stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);
            memcpy(twenty_Data + imgCount * stParam.nCurValue, pData,
                   (sizeof(unsigned char) * stParam.nCurValue));
            imgCount++;
        }
    }
    free(pData);
    pData = nullptr;
    RVC_DEBUG("\n\nGrabbed 20 images! Please press Enter!");
    return nullptr;
}

int HikCameraUSB::Open() {
    // open device
    if (hik_handle == nullptr) {
        RVC_ERROR("Failed Open, handle is nullptr!");
        return -1;
    }
    if (MV_CC_OpenDevice(hik_handle) == MV_OK) {
        // RVC_DEBUG("Success Open Hik device!");
        f_isOpen = true;
        return 0;
    } else {
        RVC_ERROR("Failed Open!");
        return -1;
    }
}

int HikCameraUSB::Close() {
    // close device
    if (hik_handle == nullptr) {
        RVC_ERROR("Failed Close, handle is nulltptr!\n");
        return -1;
    }
    if (MV_CC_CloseDevice(hik_handle) == MV_OK) {
        RVC_DEBUG("Success Close!");
    } else {
        RVC_ERROR("Failed Close!");
        return -1;
    }
    // destroy handle
    if (MV_CC_DestroyHandle(hik_handle) == MV_OK) {
        RVC_DEBUG("Success DestroyHandle!");
        f_isOpen = false;
        return 0;
    } else {
        RVC_DEBUG("Failed DestroyHandle!");
        return -1;
    }
}

bool HikCameraUSB::IsOpen() {
    return f_isOpen;
}

int HikCameraUSB::Capture() {
    printf("Please look at here!\n");
    if (!f_isOpen) {
        printf("Failed Capture, cause device is not open!\n");
        return -1;
    }
    if (hik_handle == nullptr) {
        RVC_ERROR("Failed Capture, cause handle is nullptr!");
        return -1;
    }
    printf("Ready set trigger!\n");
    // set trigger mode as off
    if (MV_CC_SetEnumValue(hik_handle, "TriggerMode", MV_TRIGGER_MODE_OFF) == MV_OK) {
        RVC_DEBUG("Success Set TriggerMode!");
    } else {
        RVC_ERROR("Failed Set TriggerMode!");
        return -1;
    }
    // start grab image
    if (MV_CC_StartGrabbing(hik_handle) == MV_OK) {
        RVC_DEBUG("Success StartGrabbing!");
    } else {
        RVC_ERROR("Failed StartGrabbing!");
        return -1;
    }
    // Get payload size
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    if (MV_CC_GetIntValue(hik_handle, "PayloadSize", &stParam) == MV_OK) {
        RVC_DEBUG("Success GetIntValue of PayloadSize!");
    } else {
        RVC_ERROR("Failed GetIntValue of PayloadSize!");
        return -1;
    }
    MV_FRAME_OUT_INFO_EX stImageInfo = {0};
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    printf("stParam.nCurValue = %d\n", (int)(stParam.nCurValue));
    if (NULL == m_last_imgptr) {
        RVC_ERROR("ERROR m_last_imgptr is nullptr!");
        return -1;
    }
    unsigned int nDataSize = stParam.nCurValue;
    printf("nDataSize is %d\n", (int)nDataSize);
    if (MV_CC_GetOneFrameTimeout(hik_handle, m_last_imgptr, nDataSize, &stImageInfo, 1000) ==
        MV_OK) {
        RVC_DEBUG("Success GetOneFrameTimeout!");
        printf("Now you GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n", stImageInfo.nWidth,
               stImageInfo.nHeight, stImageInfo.nFrameNum);
    } else {
        RVC_ERROR("Failed GetOneFrameTimeout!");
        return -1;
    }
    if (MV_CC_StopGrabbing(hik_handle) == MV_OK) {
        RVC_DEBUG("Success StopGrabbing!");
        return 0;
    } else {
        RVC_ERROR("Failed StopGrabbing!");
        return -1;
    }
}
