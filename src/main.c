#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "raylib.h"

#define WIN_W 800
#define WIN_H 450

#define BLOCK_W 4.f

typedef enum BlockType {
  NORMAL, FLOWER, ROTTEN
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
Block *populate_blocks(size_t side, Difficulty diff);

int main(void) {
  InitWindow(WIN_W, WIN_H, "Botanic Sweeper");

  Camera3D camera = {
    .position = (Vector3){ 10.f, 10.f, 10.f },
    .target = (Vector3){ 0 },
    .up = (Vector3){ 0.f, 1.f, 0.f },
    .projection = CAMERA_ORTHOGRAPHIC,
  };

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
    for (size_t i = 0; i < (grid_side * grid_side); ++i) {
      Block *b = (blocks + i);
      b->color = BEIGE;
      b->size = get_block_size(b->type);
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

    BeginDrawing(); {
      ClearBackground(GRAY);

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
  case NORMAL:
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

Block *populate_blocks(size_t side, Difficulty diff) {
  size_t group_size = (side * side);

  Block *blocks = (Block *)malloc(group_size * sizeof(Block));
  if (!blocks) {
    printf("ERROR: failed to populate blocks\n");

    exit(1);
  }

  size_t rotten_qtt = 0;
  float rotten_pct = get_rotten_pct(diff);
  size_t max_rotten_qtt = (size_t)(group_size * rotten_pct);

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
  
  size_t cur_id = 0;
  for (size_t i = 0; i < side; ++i) {
    for (size_t j = 0; j < side; ++j) {
      Block *cur_block = (blocks + cur_id);

      cur_block->type = NORMAL;
      cur_block->size = get_block_size(cur_block->type);

      cur_block->pos = (Vector3){
        .x = (side * 0.5f) - i * BLOCK_W,
        .y = block_y_diff + (cur_block->size.y * 0.5f),
        .z = (side * 0.5f) - j * BLOCK_W
      };
      cur_block->is_open = false;

      printf("cur_block = %p\n", (void *)cur_block);
      printf("  pos = %.2fx%.2f\n", cur_block->pos.x, cur_block->pos.y);

      ++cur_id;
    }
  }

  return blocks;
}
