#pragma once

#include "ImageSize.h"
// #include "RawImageIO.h"
// #include <RVC/API/OpenCV.h>
//#include <RVC/Common/Logger.h>
#include <pylon/Platform.h>

#ifdef _MSC_VER
#pragma pack(push, PYLON_PACKING)
#endif /* _MSC_VER */

#include <pylon/InstantCamera.h>

#include <chrono>
#include <sstream>
void static RVC_ERROR(const char* msg) {
    printf("%s\n", msg);
}

void static RVC_INFO(const char* msg) {
    printf("%s\n", msg);
}

void static RVC_WARN(const char* msg) {
    printf("%s\n", msg);
}

void static RVC_DEBUG(const char* msg) {
    printf("%s\n", msg);
}

namespace Pylon {

// Example of an image event handler.
class CSaveImageHandler : public CImageEventHandler {
public:
    explicit CSaveImageHandler(unsigned char* imgdata) : imgcnt{0} {
        image_max_num = sg_config.max_num_frames;
        counter = 0;
        failed_counter = 0;
        max_counter = -1;
        max_duration = 0;

        img_ptrs.resize(image_max_num);
        for (int i = 0; i < image_max_num; ++i) {
            img_ptrs[i] = imgdata + i * sg_config.img_num_bytes;
        }
    }
    virtual ~CSaveImageHandler() {}

    void CopyData(char* images_data, char* output_image) {
        RVC_INFO("Copying images...");
        for (int i = 0; i < image_max_num - 1; i++) {
            memcpy(images_data + i * sg_config.img_num_bytes, img_ptrs[i], sg_config.img_num_bytes);
        }
        memcpy(output_image, img_ptrs[image_max_num - 1], sg_config.img_num_bytes);
    }
    void CombineImage(char* images_data, char* output_image) {
        RVC_INFO("Combing images...");
        for (int j = 0; j < sg_config.img_num_bytes; j++) {
            int pixel_value = 0;
            pixel_value = (unsigned char)*(images_data + j) +
                          (unsigned char)*(images_data + sg_config.img_num_bytes + j) +
                          (unsigned char)*(images_data + 2 * sg_config.img_num_bytes + j) +
                          (unsigned char)*(images_data + 3 * sg_config.img_num_bytes + j);
            pixel_value = pixel_value / 4;
            output_image[j] = (char)pixel_value;
        }
    }

    bool CheckBufferCounter() {
        if (counter == image_max_num) {
            return true;
        } else {
            return false;
        }
    }

    void ResetCounter() {
        counter = 0;
        failed_counter = 0;
        max_counter = -1;
        max_duration = 0;
    }

    virtual void OnImageGrabbed(CInstantCamera& camera, const CGrabResultPtr& ptrGrabResult) {
        printf("Start OnImageGrabbed...\n");
        try {
            if (ptrGrabResult->GrabSucceeded()) {
                printf("GrabSucceeded!\n");
                if (counter == image_max_num) {
                    counter = 0;
                    imgcnt = 0;
                }
                if (counter < image_max_num) {
                    auto rt_1 = std::chrono::high_resolution_clock::now();
                    memcpy(img_ptrs[counter], (const char*)ptrGrabResult->GetBuffer(),
                           sg_config.img_num_bytes);
                    RVC_DEBUG("Copied  image into buffer.");
                    auto rt_2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span =
                        std::chrono::duration_cast<std::chrono::duration<double>>(rt_2 - rt_1);
                    const double duration = (double)time_span.count();
                    if (duration > max_duration) {
                        max_duration = duration;
                        max_counter = counter;
                    }
                }

                counter++;
            } else {
                printf("error!\n");
                RVC_ERROR("Error in camera ptrGrabResult!");
                failed_counter++;
            }
        } catch (const GenericException& e) {
            std::cerr << "An exception occurred." << std::endl << e.GetDescription() << std::endl;
        }
    }

private:
    int counter;
    int failed_counter;
    int max_counter;
    double max_duration;
    int imgcnt;
    int image_max_num;
    std::vector<unsigned char*> img_ptrs;
    std::vector<double> timestamps;
};

// Example of an image event handler.
class CConfigurationEventPrinter : public CConfigurationEventHandler {
public:
    void OnAttach(CInstantCamera& /*camera*/) { std::cout << "OnAttach event" << std::endl; }

    void OnAttached(CInstantCamera& camera) {
        std::cout << "OnAttached event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnOpen(CInstantCamera& camera) {
        std::cout << "OnOpen event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnOpened(CInstantCamera& camera) {
        std::cout << "OnOpened event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnGrabStart(CInstantCamera& camera) {
        std::cout << "OnGrabStart event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnGrabStarted(CInstantCamera& camera) {
        std::cout << "OnGrabStarted event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnGrabStop(CInstantCamera& camera) {
        std::cout << "OnGrabStop event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnGrabStopped(CInstantCamera& camera) {
        std::cout << "OnGrabStopped event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnClose(CInstantCamera& camera) {
        std::cout << "OnClose event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnClosed(CInstantCamera& camera) {
        std::cout << "OnClosed event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnDestroy(CInstantCamera& camera) {
        std::cout << "OnDestroy event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnDestroyed(CInstantCamera& /*camera*/) { std::cout << "OnDestroyed event" << std::endl; }

    void OnDetach(CInstantCamera& camera) {
        std::cout << "OnDetach event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnDetached(CInstantCamera& camera) {
        std::cout << "OnDetached event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
    }

    void OnGrabError(CInstantCamera& camera, const char* errorMessage) {
        std::cout << "OnGrabError event for device " << camera.GetDeviceInfo().GetModelName()
                  << std::endl;
        std::cout << "Error Message: " << errorMessage << std::endl;
    }

    void OnCameraDeviceRemoved(CInstantCamera& camera) {
        std::cout << "OnCameraDeviceRemoved event for device "
                  << camera.GetDeviceInfo().GetModelName() << std::endl;
    }
};
}  // namespace Pylon
