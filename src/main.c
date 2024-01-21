#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"

#define WIN_W 800
#define WIN_H 450

#define BLOCK_W 4.f

typedef enum Page {
  MENU, PLAYING, WIN, LOSE
} Page;

typedef enum BlockType {
  DIRT, FLOWER, ROTTEN
} BlockType;

typedef struct Block {
  Vector3 pos;
  BlockType type;
  bool is_open;
  // x = width, y = height
  Vector2 size;
  Color color;
} Block;

typedef enum Difficulty {
  EASY, MEDIUM, HARD
} Difficulty;

size_t get_grid_size(Difficulty diff);
Vector2 get_block_size(BlockType type);
float get_rotten_pct(Difficulty diff);
float get_block_y_diff(Difficulty diff);
Block *populate_blocks(size_t side, Difficulty diff);
Color get_block_color(BlockType type);

int main(void) {
  InitWindow(WIN_W, WIN_H, "Botanic Sweeper");

  Camera3D camera = {
    .position = (Vector3){ 10.f, 10.f, 10.f },
    .target = (Vector3){ 0 },
    .up = (Vector3){ 0.f, 1.f, 0.f },
    .projection = CAMERA_ORTHOGRAPHIC,
  };

  Page cur_page = MENU;

  Difficulty diff = EASY;
  const size_t grid_side = get_grid_size(diff);
  Block *blocks = populate_blocks(grid_side, diff);

  switch (diff) {
  case MEDIUM:
    camera.fovy = 60.f;

    break;
  case HARD:
    camera.fovy = 85.f;
    
    break;
  case EASY:
  default:
    camera.fovy = 45.f;

    break;
  }

  Ray mouse_ray = { 0 };
  RayCollision mouse_coll = { 0 };

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    if (cur_page == MENU) {
      if (IsKeyDown(KEY_SPACE)) {
        cur_page = PLAYING;
      }
    } else {
      for (size_t i = 0; i < (grid_side * grid_side); ++i) {
        Block *b = (blocks + i);
        b->pos.y = get_block_y_diff(diff) + (b->size.y * 0.5f);
        if (b->is_open) {
          b->color = get_block_color(b->type);
          b->size = get_block_size(b->type);
        } else {
          b->color = BEIGE;
          b->size.y = 2.f;
        }
      }

      mouse_ray = GetMouseRay(GetMousePosition(), camera);
      for (size_t i = 0; i < (grid_side * grid_side); ++i) {
        Block *b = (blocks + i);
        mouse_coll = GetRayCollisionBox(
          mouse_ray,
          (BoundingBox){
            (Vector3){
              .x = b->pos.x - b->size.x * 0.5f,
              .y = b->pos.y - b->size.y * 0.5f,
              .z = b->pos.z - b->size.x * 0.5f
            },
            (Vector3){
              .x = b->pos.x + b->size.x * 0.5f,
              .y = b->pos.y + b->size.y * 0.5f,
              .z = b->pos.z + b->size.x * 0.5f
            }
          }
          );

        if (mouse_coll.hit) {
          b->color = Fade(WHITE, 0.2);

          if (!b->is_open) {
            b->size.y += 1.f;
          }

          if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            b->is_open = true;
          }

          break;
        }
      }
    }

    BeginDrawing(); {
      ClearBackground(GRAY);

      switch (cur_page) {
      case LOSE:
        // TODO
        break;
      case WIN:
        // TODO
        break;
      case PLAYING:
        BeginMode3D(camera); {
          size_t cur_id = 0;
          for (size_t i = 0; i < grid_side; ++i) {
            for (size_t j = 0; j < grid_side; ++j) {
              Block *cur_block = (blocks + cur_id);
              DrawCube(
                cur_block->pos,
                cur_block->size.x, cur_block->size.y, cur_block->size.x,
                cur_block->color
                );

              ++cur_id;
            }
          }
        }
        EndMode3D();

        break;
      case MENU:
      default:
        DrawText("Botanic Sweeper", 40, WIN_H * 0.2f, 40, GREEN);
        DrawText("Press space...", 40, WIN_H * 0.3f, 20, GREEN);
        
        break;
      }
    }

    EndDrawing();
  }

  free(blocks);
  CloseWindow();
  
  return 0;
}

size_t get_grid_size(Difficulty diff) {
  size_t qtt;
  switch (diff) {
  case MEDIUM:
    qtt = 15;
    
    break;
  case HARD:
    qtt = 20;

    break;
  case EASY:
  default:
    qtt = 10;

    break;
  }

  return qtt;
}

Vector2 get_block_size(BlockType type) {
  float height;
  switch (type) {
  case FLOWER:
    height = 4.5f;

    break;
  case ROTTEN:
    height = 3.f;

    break;
  case DIRT:
  default:
    height = 2.f;
    break;
  }

  return (Vector2){
    .x = BLOCK_W,
    .y = height
  };
}

float get_rotten_pct(Difficulty diff) {
  float pct;
  switch (diff) {
  MEDIUM:
    pct = 0.2f;

    break;
  HARD:
    pct = 0.3f;
    
    break;
  EASY:
  default:
    pct = 0.1f;

    break;
  }

  return pct;
}

float get_block_y_diff(Difficulty diff) {
  float block_y_diff;
  switch (diff) {
  case MEDIUM:
    block_y_diff = -22.f;

    break;
  case HARD:
    block_y_diff = -28.f;
    
    break;
  case EASY:
  default:
    block_y_diff = -14.f;

    break;
  }

  return block_y_diff;
}

Block *populate_blocks(size_t side, Difficulty diff) {
  size_t group_size = (side * side);

  Block *blocks = (Block *)malloc(group_size * sizeof(Block));
  if (!blocks) {
    printf("ERROR: failed to populate blocks\n");

    exit(1);
  }

  float block_y_diff = get_block_y_diff(diff);
  
  size_t cur_id = 0;
  for (size_t i = 0; i < side; ++i) {
    for (size_t j = 0; j < side; ++j) {
      Block *cur_block = (blocks + cur_id);

      cur_block->type = DIRT;
      cur_block->size = get_block_size(DIRT);

      cur_block->pos = (Vector3){
        .x = (side * 0.5f) - i * BLOCK_W,
        .y = block_y_diff + (cur_block->size.y * 0.5f),
        .z = (side * 0.5f) - j * BLOCK_W
      };
      cur_block->is_open = false;

      ++cur_id;
    }
  }

  float rotten_pct = get_rotten_pct(diff);
  size_t rotten_qtt = (size_t)(group_size * rotten_pct);

// not of the total, but of the remainder from the rotten
#define FLOWER_PCT 0.6f
  size_t healthy_qtt = group_size - rotten_qtt;
  size_t flower_qtt = healthy_qtt * FLOWER_PCT;

  for (size_t i = 0; i < rotten_qtt; ++i) {
    Block *rand_block = (blocks + GetRandomValue(0, group_size - 1));
    if (rand_block->type == ROTTEN) {
      --i;
      continue;
    }

    rand_block->type = ROTTEN;
  }

  for (size_t i = 0; i < flower_qtt; ++i) {
    Block *rand_block = (blocks + GetRandomValue(0, group_size - 1));
    if (rand_block->type != DIRT) {
      --i;
      continue;
    }

    rand_block->type = FLOWER;
  }

  return blocks;
}

Color get_block_color(BlockType type) {
  Color color;

  switch (type) {
  case FLOWER:
    color = GREEN;

    break;
  case ROTTEN:
    color = BLACK;

    break;
  case DIRT:
  default:
    color = DARKGRAY;

    break;
  }

  return color;
}
