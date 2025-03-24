#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// #include <cjelly/format/3d/obj.h>
// #include <cjelly/format/3d/mtl.h>
#include <cjelly/format/image.h>
#include <cjelly/format/image/bmp.h>

#ifdef _WIN32
#include <windows.h>
#include <stdint.h>
uint64_t getCurrentTimeInMilliseconds(void) {
  LARGE_INTEGER frequency;
  LARGE_INTEGER counter;
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&counter);
  return (uint64_t)((counter.QuadPart * 1000LL) / frequency.QuadPart);
}
#else
#include <time.h>
#include <stdint.h>
uint64_t getCurrentTimeInMilliseconds(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL;
}
#endif

#include <cjelly/cjelly.h>


void renderSquare(CJellyWindow *win) {
  drawFrameForWindow(win);
}

int main(void) {
  #ifdef _WIN32
  // Windows: hInstance is set in createPlatformWindow.
#else
  // Linux: Open X display.
  display = XOpenDisplay(NULL);
  if (!display) {
    fprintf(stderr, "Failed to open X display\n");
    exit(EXIT_FAILURE);
  }
#endif

  // Try to detect the type of image file.
  CJellyFormatImage * image = NULL;
  CJellyFormatImageError err = cjelly_format_image_load("test/images/bmp/16Color.bmp", &image);
  if (err != CJELLY_FORMAT_IMAGE_SUCCESS) {
    fprintf(stderr, "Error loading image: %s\n", cjelly_format_image_strerror(err));
    exit(EXIT_FAILURE);
  }

  if (image->type == CJELLY_FORMAT_IMAGE_BMP) {
    printf("Detected BMP image\n");
  } else {
    printf("Unknown image type\n");
  }

  CJellyFormatImageBMP * bmpImage = (CJellyFormatImageBMP *)image;
  cjelly_format_image_bmp_dump(bmpImage);

  // // Open the OBJ file.
  // CJellyFormat3dObjModel * model = NULL;
  // CJellyFormat3dObjError obj_err = cjelly_format_3d_obj_load("test/models/violin_case/violin_case.obj", &model);
  // if (obj_err != CJELLY_FORMAT_3D_OBJ_SUCCESS) {
  //   fprintf(stderr, "Error loading OBJ file: %s\n", cjelly_format_3d_obj_strerror(obj_err));
  //   exit(EXIT_FAILURE);
  // }

  // // write the model to "test/models/violin_case/violin_case.comp.obj"
  // FILE * fd = fopen("test/models/violin_case/violin_case.comp.obj", "w");
  // if (!fd) {
  //   fprintf(stderr, "Error opening file for writing\n");
  //   exit(EXIT_FAILURE);
  // }
  // cjelly_format_3d_obj_dump(model, fd);
  // fclose(fd);

  // // Open the MTL file.
  // CJellyFormat3dMtl materials = {0};
  // CJellyFormat3dMtlError mtl_err = cjelly_format_3d_mtl_load("test/models/violin_case/vp.mtl", &materials);
  // if (mtl_err != CJELLY_FORMAT_3D_MTL_SUCCESS) {
  //   fprintf(stderr, "Error loading MTL file: %s\n", cjelly_format_3d_mtl_strerror(mtl_err));
  //   exit(EXIT_FAILURE);
  // }

  // // write the materials to "test/models/violin_case/vp.comp.mtl"
  // fd = fopen("test/models/violin_case/vp.comp.mtl", "w");
  // if (!fd) {
  //   fprintf(stderr, "Error opening file for writing\n");
  //   exit(EXIT_FAILURE);
  // }
  // cjelly_format_3d_mtl_dump(materials.materials, materials.material_count, fd);
  // fclose(fd);

  // Create two windows.
  CJellyWindow win1 = {0}, win2 = {0};

  win1.renderCallback = renderSquare;
  win1.updateMode = CJELLY_UPDATE_MODE_FIXED;
  win1.fixedFramerate = 60;

  win2.renderCallback = renderSquare;
  win2.updateMode = CJELLY_UPDATE_MODE_EVENT_DRIVEN;

  createPlatformWindow(&win1, "Vulkan Square - Window 1", WIDTH, HEIGHT);
  createPlatformWindow(&win2, "Vulkan Square - Window 2", WIDTH, HEIGHT);

  // Global Vulkan initialization.
  initVulkanGlobal();

  // For each window, create the per-window Vulkan objects.
  createSurfaceForWindow(&win1);
  createSwapChainForWindow(&win1);
  createImageViewsForWindow(&win1);
  createFramebuffersForWindow(&win1);
  createCommandBuffersForWindow(&win1);
  createSyncObjectsForWindow(&win1);

  createSurfaceForWindow(&win2);
  createSwapChainForWindow(&win2);
  createImageViewsForWindow(&win2);
  createFramebuffersForWindow(&win2);
  createCommandBuffersForWindow(&win2);
  createSyncObjectsForWindow(&win2);

  // Main render loop.
  CJellyWindow * windows[] = {&win1, &win2};
  while (!shouldClose) {
    processWindowEvents();
    uint64_t currentTime = getCurrentTimeInMilliseconds();

    for (int i = 0; i < 2; ++i) {
      CJellyWindow * win = windows[i];
      // Update window 1 independently:
      switch (win->updateMode) {
        case CJELLY_UPDATE_MODE_VSYNC:
          // For VSync mode, the present call (with FIFO) will throttle rendering.
          if (win->renderCallback) {
            win->renderCallback(win);
          }
          break;
        case CJELLY_UPDATE_MODE_FIXED:
          // In fixed mode, only render if it’s time for the next frame.
          if (currentTime >= win->nextFrameTime) {
            if (win->renderCallback) {
              win->renderCallback(win);
            }
            win->nextFrameTime = currentTime + (1000 / win->fixedFramerate);
          }
          break;
        case CJELLY_UPDATE_MODE_EVENT_DRIVEN:
          // In event-driven mode, only render when needed.
          if (win->needsRedraw) {
            if (win->renderCallback) {
              win->renderCallback(win);
            }
            win->needsRedraw = 0;
          }
          break;
      }
    }

    // Sleep for a short duration to avoid busy waiting.
  #ifdef _WIN32
    Sleep(1);
  #else
    struct timespec req = {0, 1000000}; // 1 millisecond
    nanosleep(&req, NULL);
  #endif
  }
  vkDeviceWaitIdle(device);

  // Clean up per-window resources.
  cleanupWindow(&win1);
  cleanupWindow(&win2);

  // Clean up global Vulkan resources.
  cleanupVulkanGlobal();

#ifndef _WIN32
  XCloseDisplay(display);
#endif
  return 0;
}
