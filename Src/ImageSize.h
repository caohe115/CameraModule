#pragma once

#include <fstream>
#include <iostream>

struct CamConfig {
    CamConfig() {
        img_width = 1280;
        img_height = 1024;
        max_num_frames = 21;
        pixel_size = 1;
        camera_exposuretime = 6000;
        projector_brightness = 50;
        img_num_bytes = img_width * img_height * pixel_size;

        // Print();
    }

    ~CamConfig() {}

    void Print() const {
        printf("img_width: %d\nimg_height:%d\nmax_num_frames:%d\npixel_size:%d\n", img_width,
               img_height, max_num_frames, pixel_size);
    }

    int img_width;
    int img_height;
    int max_num_frames;
    int pixel_size;     // in byte
    int img_num_bytes;  //
    int camera_exposuretime;
    int projector_brightness;
    bool if_save_images;
};

static CamConfig sg_config;
