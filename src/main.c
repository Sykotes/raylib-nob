#include "include/raylib.h"

#include "include/gen/idle.h"
#include "include/gen/run.h"
#include "include/gen/jump.h"
#include "include/gen/slide.h"
#include "include/gen/wall_slide.h"

#define VIRTUAL_WIDTH 320
#define VIRTUAL_HEIGHT 240
#define INITIAL_SCREEN_WIDTH 800
#define INITIAL_SCREEN_HEIGHT 600
#define PLAYER_WIDTH 14
#define PLAYER_HEIGHT 18
#define PLAYER_IDLE_ANIMATION_TIME 0.2f
#define PLAYER_RUN_ANIMATION_TIME 0.1f

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

enum player_animation {
    IDLE,
    RUN,
    JUMP,
    SLIDE,
    WALL_SLIDE,
};

typedef struct {
    Vector2 pos;
    enum player_animation anim;
    enum player_animation anim_last;
    int anim_frame;
    float anim_timer;
    Texture2D idle_tex;
    Texture2D run_tex;
    Texture2D jump_tex;
    Texture2D slide_tex;
    Texture2D wall_slide_tex;
} Player;

int main(void) {
    InitWindow(INITIAL_SCREEN_WIDTH, INITIAL_SCREEN_HEIGHT, "ninjagame");
    SetWindowMinSize(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetWindowState(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);

#ifdef PLATFORM_WEB
    SetWindowSize(EM_ASM_INT({ return window.innerWidth; }), EM_ASM_INT({ return window.innerHeight; }));
#endif

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    Image idle_img = LoadImageFromMemory(".png", __assets_idle_png, __assets_idle_png_len);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
    Image run_img = LoadImageFromMemory(".png", __assets_run_png, __assets_run_png_len);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
    Image jump_img = LoadImageFromMemory(".png", __assets_jump_png, __assets_jump_png_len);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
    Image slide_img = LoadImageFromMemory(".png", __assets_slide_png, __assets_slide_png_len);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
    Image wall_slide_img = LoadImageFromMemory(".png", __assets_wall_slide_png, __assets_wall_slide_png_len);      // Load image from memory buffer, fileType refers to extension: i.e. '.png'
    Player player = {
        .pos = (Vector2){.x = 100.0f, .y = 30.0f},
        .anim = IDLE,
        .anim_frame = 0,
        .anim_timer = 0.0f,
        .idle_tex = LoadTextureFromImage(idle_img),
        .run_tex = LoadTextureFromImage(run_img),
        .jump_tex = LoadTextureFromImage(jump_img),
        .slide_tex = LoadTextureFromImage(slide_img),
        .wall_slide_tex = LoadTextureFromImage(wall_slide_img),
    };

    while (!WindowShouldClose()) {
        // Window
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Calculate integer scaling for clean pixel art
        int scaleX = GetRenderWidth() / VIRTUAL_WIDTH;
        int scaleY = GetRenderHeight() / VIRTUAL_HEIGHT;
        int scale = (scaleX < scaleY) ? scaleX : scaleY;
        int offsetX = (GetRenderWidth() - VIRTUAL_WIDTH * scale) / 2;
        int offsetY = (GetRenderHeight() - VIRTUAL_HEIGHT * scale) / 2;

        if (IsKeyPressed(KEY_F1) && !IsWindowFullscreen()) {
            SetWindowSize(VIRTUAL_WIDTH * scale, VIRTUAL_HEIGHT * scale);
        }

        // Animation
        if (IsKeyDown(KEY_RIGHT)) {
            player.anim = RUN;
        } else {
            player.anim = IDLE;
        }
        if (player.anim != player.anim_last) {
            player.anim_frame = 0;
            player.anim_timer = 0.0;
            player.anim_last = player.anim;
        }
        if (player.anim == IDLE || player.anim == RUN) {
            player.anim_timer += GetFrameTime();
            switch (player.anim) {
            case IDLE:
                if (player.anim_timer >= PLAYER_IDLE_ANIMATION_TIME) {
                    player.anim_timer = 0.0f;
                    player.anim_frame = (player.anim_frame + 1) % (player.idle_tex.width / PLAYER_WIDTH);
                }
                break;
            case RUN:
                if (player.anim_timer >= PLAYER_RUN_ANIMATION_TIME) {
                    player.anim_timer = 0.0f;
                    player.anim_frame = (player.anim_frame + 1) % (player.run_tex.width / PLAYER_WIDTH);
                }
                break;
            default:
                break;
            }
        }

        // Render
        BeginTextureMode(target);
        {
            ClearBackground(DARKGRAY);
            DrawText("Ninja Game", 100, 50, 20, RED);

            // Draw player
            Rectangle texture = {PLAYER_WIDTH * player.anim_frame, 0, PLAYER_WIDTH, PLAYER_HEIGHT};
            Rectangle source = {player.pos.x, player.pos.y, PLAYER_WIDTH, PLAYER_HEIGHT};
            Vector2 pos = {0, 0};
            double rotation = 0.0;
            Color colour = WHITE;
            switch (player.anim) {
            case IDLE:
                DrawTexturePro(player.idle_tex, texture, source, pos, rotation, colour);
                break;
            case RUN:
                DrawTexturePro(player.run_tex, texture, source, pos, rotation, colour);
                break;
            default:
                break;
            }
        }
        EndTextureMode();

        BeginDrawing();
        {
            ClearBackground(BLACK);

            // Render target scaled
            DrawTexturePro(
                target.texture,
                (Rectangle){0, 0, (float)VIRTUAL_WIDTH, (float)-VIRTUAL_HEIGHT}, // flip vertically
                (Rectangle){offsetX, offsetY, VIRTUAL_WIDTH * scale, VIRTUAL_HEIGHT * scale},
                (Vector2){0, 0}, 
                0.0f, 
                WHITE);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

