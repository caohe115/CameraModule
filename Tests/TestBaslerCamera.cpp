#include <RVC/Camera/CameraBase.h>

#include "../Src/ImageSize.h"

int main(int argc, char *argv[])
{
    int retNNN = 0;
    CameraSystemBasePtr pcs = GetCameraSystem("Basler");
    if (pcs)
    {
        pcs->Initialize();
        if (pcs->IsInitialized())
        {
            CameraBasePtr cam = pcs->Create("22500041", "USB3.0");
            if (cam->Open() == 0)
            {
                if (cam->IsOpen())
                {
                    cam->Capture();
                }
                else
                {
                    retNNN = -1;
                    printf("Failed, cam is not open in main function!\n");
                }
            }
            else
            {
                printf("Camera open failed in main function!\n");
                retNNN = -1;
            }
            cam->Close();
        }
        else
        {
            printf("pcs is not initilized\n");
            retNNN = -1;
        }
        pcs->Terminate();
    }
    else
    {
        retNNN = -1;
    }
    return retNNN;
}
