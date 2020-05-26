#include <string.h>

#include <iostream>

#include "BaslerCamera.h"
#include "HikCamera.h"

CameraSystemBasePtr GetCameraSystem(const char *brand)
{
    if (strcmp(brand, "Basler") == 0)
    {
        return std::make_shared<BaslerCameraSystem>();
    }
    else if (strcmp(brand, "Hik") == 0)
    {
        return std::make_shared<HikCameraSystem>();
    }
    else
    {
        printf("Brand error, only support 'Basler' 'Hik'!\n");
        return nullptr;
    }
    return nullptr;
}
