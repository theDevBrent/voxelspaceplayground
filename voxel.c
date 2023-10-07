#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "dos/dos.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define SCALE_FACTOR 100

uint8_t* heightmap = NULL;
uint8_t* colormap = NULL;
uint8_t* controls = NULL;

typedef struct {
    float x;
    float y;
    float height;
    float angle;
    float horizon;
    float tilt;
    float zIndex;
} camera_t;

camera_t camera = {
    .x = 512.0,
    .y = 512.0,
    .height = 150.0,
    .angle = 0.0,
    .horizon = 100.0,
    .tilt = 0.0,
    .zIndex = 600.0
};


void processinput() {
  if (keystate(KEY_W)) {
      camera.x += cos(camera.angle);
      camera.y += sin(camera.angle);
      camera.tilt = 0.0;
  }
  if (keystate(KEY_S)) {
      camera.x -= cos(camera.angle);
      camera.y -= sin(camera.angle);
      camera.tilt = 0.0;
  }
  if (keystate(KEY_A)) {
      camera.angle -= 0.01;
      camera.tilt = 2.0;
  }
  if (keystate(KEY_D)) {
      camera.angle += 0.01;
      camera.tilt = -2.0;
  }
  if (keystate(KEY_E)) {
      camera.height++;
  }
  if (keystate(KEY_Q)) {
      camera.height--;
  }
}

int
main(int argc, char* args[])
{
    setvideomode(videomode_320x200);

    uint8_t palette[256 * 3];
    int mapWidth;
    int mapHeight;
    int palCount;



    colormap = loadgif("maps/colormap2.gif", &mapWidth, &mapHeight, &palCount, palette);
    heightmap = loadgif("maps/heightmap2.gif", &mapWidth, &mapHeight, NULL, NULL);

    for (int i = 0; i < palCount; i++) {
        setpal(i, palette[3 * i + 0], palette[3 * i + 1], palette[3 * i + 2]);
    }

    setpal(0, 36, 36, 56);

    setdoublebuffer(1);
    uint8_t* framebuffer = screenbuffer();

    while (!shuttingdown()) {
        waitvbl();
        clearscreen();

        processinput();

        float sinangle = sin(camera.angle);
        float cosangle = cos(camera.angle);

        float plx = cosangle * camera.zIndex + sinangle * camera.zIndex;
        float ply = sinangle * camera.zIndex - cosangle * camera.zIndex;
        float prx = cosangle * camera.zIndex - sinangle * camera.zIndex;
        float pry = sinangle * camera.zIndex + cosangle * camera.zIndex;

        for (int i = 0; i < SCREEN_WIDTH; i++) {
            float deltaX = (plx + (prx - plx) / SCREEN_WIDTH * i) / camera.zIndex;
            float deltaY = (ply + (pry - ply) / SCREEN_WIDTH * i) / camera.zIndex;

            float rx = camera.x;
            float ry = camera.y;

            float max_height = SCREEN_HEIGHT;

            for (int z = 1; z < camera.zIndex; z++) {
                rx += deltaX;
                ry += deltaY;

                int mapoffset = ((1024 * ((int)(ry) & 1023)) + ((int)(rx) & 1023));

                int heightonscreen = (int)((camera.height - heightmap[mapoffset]) / z * SCALE_FACTOR + camera.horizon);

                if (heightonscreen < 0) {
                    heightonscreen = 0;
                }
                if (heightonscreen > SCREEN_HEIGHT) {
                    heightonscreen = SCREEN_HEIGHT - 1;
                }

                if (heightonscreen < max_height) {


                    float lean = (camera.tilt * (i / (float)SCREEN_WIDTH - 0.5) + 0.5) * SCREEN_HEIGHT / 6;

                    for (int y = (heightonscreen + lean); y < (max_height + lean) ; y++) {
                        framebuffer[(SCREEN_WIDTH * y) + i] = (uint8_t)colormap[mapoffset];
                  }
                    max_height = heightonscreen;
                }
            }
        }


            framebuffer = swapbuffers();

        if (keystate(KEY_ESCAPE)) {
            break;
        }
    }

    return 0;
}