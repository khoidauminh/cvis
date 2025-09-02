#include "program.h"
#include "render.h"
#include "renderer-private.h" // IWYU pragma: keep.

#include <assert.h>
#include <raylib.h>
#include <stdlib.h>

typedef struct raylib_renderer {
    Color color;
    bool is_drawing;
    RenderTexture2D target;
} RLRenderer;

static void start_drawing(RLRenderer *rlr) {
    if (rlr->is_drawing)
        return;

    rlr->is_drawing = true;
    BeginTextureMode(rlr->target);
}

static void raylib_set_color(Renderer *r, APIParameter *param) {
    RLRenderer *rlr = r->renderer;
    rlr->color = (Color){
        .r = param->color.r,
        .g = param->color.g,
        .b = param->color.b,
        .a = param->color.a,
    };
}

static void raylib_rect(Renderer *r, APIParameter *param) {
    RLRenderer *rlr = r->renderer;
    start_drawing(rlr);
    DrawRectangle(param->rect[0], param->rect[1], param->rect[2],
                  param->rect[3], rlr->color);
}

static void raylib_plot(Renderer *r, APIParameter *param) {
    RLRenderer *rlr = r->renderer;
    start_drawing(rlr);
    DrawPixelV((Vector2){param->plot[0], param->plot[1]}, rlr->color);
}

static void raylib_fill(Renderer *r, APIParameter *) {
    RLRenderer *rlr = r->renderer;
    start_drawing(rlr);
    ClearBackground(rlr->color);
}

static void raylib_clear(Renderer *r, APIParameter *) {
    raylib_set_color(r, &(APIParameter){.color = r->cfg->background});
    raylib_fill(r, nullptr);
}

static void raylib_present(Renderer *r, APIParameter *) {
    RLRenderer *rlr = r->renderer;
    rlr->is_drawing = false;
    EndTextureMode();

    BeginDrawing();

    auto target = rlr->target;

    float w = (float)r->cfg->width * r->cfg->scale;
    float h = (float)r->cfg->height * r->cfg->scale;

    DrawTexturePro(target.texture,
                   (Rectangle){0.0f, 0.0f, (float)target.texture.width,
                               -(float)target.texture.height},
                   (Rectangle){0.0f, 0.0f, w, h}, (Vector2){0.0f, 0.0f}, 0.0f,
                   WHITE);

    EndDrawing();
}

static void raylib_autoresize(Renderer *r, APIParameter *) {
    r->cfg->width = GetRenderWidth() / r->cfg->scale;
    r->cfg->height = GetRenderWidth() / r->cfg->scale;
}

static DrawFunc *RAYLIB_FUNC_MAP[] = {
    [renderapi_plot] = &raylib_plot,
    [renderapi_rect] = &raylib_rect,
    [renderapi_fill] = &raylib_fill,
    [renderapi_clear] = &raylib_clear,
    [renderapi_color] = &raylib_set_color,
    [renderapi_flush] = &raylib_present,
    [renderapi_resize] = &raylib_autoresize,
};

void raylib_init(Renderer *r) {
    RLRenderer *rlr = malloc(sizeof(RLRenderer));
    assert(rlr);

    if (r->cfg->refreshmode == refreshmode_sync) {
        SetConfigFlags(FLAG_VSYNC_HINT);
    } else {
        SetTargetFPS(r->cfg->refreshrate);
    }

    InitWindow(r->cfg->width * r->cfg->scale, r->cfg->height * r->cfg->scale,
               "cvis");

    rlr->target = LoadRenderTexture(r->cfg->width, r->cfg->height);
    SetTextureFilter(rlr->target.texture, TEXTURE_FILTER_POINT);

    DisableEventWaiting();

    rlr->color = WHITE;
    rlr->is_drawing = false;

    r->renderer = rlr;
    r->api = RAYLIB_FUNC_MAP;
}

void raylib_end(Renderer *r) {
    RLRenderer *rlr = r->renderer;
    UnloadRenderTexture(rlr->target);
    CloseWindow();
}

void pg_eventloop_raylib(Program *p) {
    assert(renderer_get_type(pg_renderer(p)) == renderertype_raylib);

    RNDR_SET_TARGET(pg_renderer(p));

    while (!WindowShouldClose()) {
        RNDR_AUTORESIZE();

        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            vm_next(pg_vismanager(p));
        }

        RNDR_FLUSH();

        vm_perform(p);
    }
}
