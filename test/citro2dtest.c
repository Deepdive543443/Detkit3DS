#include <stdio.h>
#include <stdlib.h>
#include "3ds.h"
#include "citro2d.h"
#include "sections.h"

int main(int argc, char** argv)
{
    // Targets for left and right eye respectively
    C3D_RenderTarget* left;
    C2D_SpriteSheet   sheet;
    C2D_Image         image;

    int keysD, keysH;

    // Initialize libraries
    romfsInit();
    gfxInitDefault();
    gfxSet3D(false);  // Activate stereoscopic 3D
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();
    consoleInit(GFX_BOTTOM, NULL);

    printf("\x1b[1;1H Citro2D test\nPress A");

    left = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    void* generated_img = malloc(WIDTH_BTM * WIDTH_TOP * 4);
    memset(generated_img, 255, WIDTH_BTM * WIDTH_TOP * 4);
    sheet = C2D_SpriteSheetLoadFromMem(generated_img, WIDTH_BTM * WIDTH_TOP * 4);
    image = C2D_SpriteSheetGetImage(sheet, 0);

    while (aptMainLoop())
    {
        // Handle user input
        hidScanInput();
        keysD = hidKeysDown();
        keysH = hidKeysHeld();

        if (keysD & KEY_START) break;

        // Render the scene
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        {
            if (keysH & KEY_A)
            {
                C2D_TargetClear(left, C2D_Color32(0xe0, 0xb0, 0xff, 0xff));  // RGBA
                C2D_SceneBegin(left);

                C2D_DrawRectangle(100, 100, 0, 200, 200, 0xb0, 0xc4, 0xde, 0xff);
            } else
            {
                C2D_TargetClear(left, C2D_Color32(0xb0, 0xc4, 0xde, 0xff));  // RGBA
                C2D_SceneBegin(left);

                C2D_DrawImageAt(image, 100, 140, 0, NULL, 1.0f, 1.0f);
            }
        }

        C3D_FrameEnd(0);
    }

    // De-initialize libraries
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
