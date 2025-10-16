#include "../include/raylib.h"

const int VIRTUAL_WIDTH = 640;
const int VIRTUAL_HEIGHT = 480;

int main(int argc, char **argv) {
    const int initialScreenWidth = 800;
    const int initialScreenHeight = 600;
    InitWindow(initialScreenWidth, initialScreenHeight, "ninjagame");
    SetWindowMinSize(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        int scaleX = GetRenderWidth() / VIRTUAL_WIDTH;
        int scaleY = GetRenderHeight() / VIRTUAL_HEIGHT;
        int scale = (scaleX < scaleY) ? scaleX : scaleY;
        int offsetX = (GetRenderWidth() - VIRTUAL_WIDTH * scale) / 2;
        int offsetY = (GetRenderHeight() - VIRTUAL_HEIGHT * scale) / 2;

        if (IsKeyPressed(KEY_F1)) {
            if (!IsWindowFullscreen()) {
                SetWindowSize(VIRTUAL_WIDTH * scale, VIRTUAL_HEIGHT * scale);
            }
        }

        BeginTextureMode(target);
        {
            ClearBackground(DARKGRAY);
            DrawText("Ninja Game", 100, 50, 20, RED);
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawTexturePro(
                target.texture,
                (Rectangle){0, 0, (float)VIRTUAL_WIDTH,
                            (float)-VIRTUAL_HEIGHT}, // flip vertically
                (Rectangle){offsetX, offsetY, VIRTUAL_WIDTH * scale,
                            VIRTUAL_HEIGHT * scale},
                (Vector2){0, 0}, 0.0f, WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
