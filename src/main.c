#include "include/raylib.h"

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
#endif // PLATFORM_WEB

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
    int width = EM_ASM_INT({ return window.innerWidth; });
    int height = EM_ASM_INT({ return window.innerHeight; });
    SetWindowSize(width, height);
#endif // PLATFORM_WEB

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
    Player player = (Player){
        .pos = (Vector2){.x = 100.0f, .y = 30.0f},
        .anim = IDLE,
        .anim_frame = 0,
        .anim_timer = 0.0f,
        .idle_tex = LoadTexture("assets/idle.png"),
        .run_tex = LoadTexture("assets/run.png"),
        .jump_tex = LoadTexture("assets/jump.png"),
        .slide_tex = LoadTexture("assets/slide.png"),
        .wall_slide_tex =
            LoadTexture("assets/images/entities/player/wall_slide.png"),
    };

    while (!WindowShouldClose()) {
        // Window
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
                    player.anim_frame++;
                    player.anim_frame %= (player.idle_tex.width / PLAYER_WIDTH);
                }
                break;
            case RUN:
                if (player.anim_timer >= PLAYER_RUN_ANIMATION_TIME) {
                    player.anim_timer = 0.0f;
                    player.anim_frame++;
                    player.anim_frame %= (player.run_tex.width / PLAYER_WIDTH);
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

            switch (player.anim) {
            case IDLE:
                DrawTexturePro(player.idle_tex,
                               (Rectangle){PLAYER_WIDTH * player.anim_frame, 0,
                                           PLAYER_WIDTH, PLAYER_HEIGHT},
                               (Rectangle){player.pos.x, player.pos.y,
                                           PLAYER_WIDTH, PLAYER_HEIGHT},
                               (Vector2){0, 0}, 0.0f, WHITE);
                break;
            case RUN:
                DrawTexturePro(player.run_tex,
                               (Rectangle){PLAYER_WIDTH * player.anim_frame, 0,
                                           PLAYER_WIDTH, PLAYER_HEIGHT},
                               (Rectangle){player.pos.x, player.pos.y,
                                           PLAYER_WIDTH, PLAYER_HEIGHT},
                               (Vector2){0, 0}, 0.0f, WHITE);
                break;
            default:
                break;
            }
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

