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

Vector2 get_block_size(BlockType type);
Block *populate_blocks(size_t side);

int main(void) {
  InitWindow(WIN_W, WIN_H, "Botanic Sweeper");

  Camera3D camera = {
    .position = (Vector3){ 10.f, 10.f, 10.f },
    .target = (Vector3){ 0 },
    .up = (Vector3){ 0.f, 1.f, 0.f },
    .fovy = 45.f,
    .projection = CAMERA_ORTHOGRAPHIC,
  };

  const size_t grid_side = 10;
  Block *blocks = populate_blocks(grid_side);

  SetTargetFPS(60);
  while (!WindowShouldClose()) {
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

Block *populate_blocks(size_t side) {
  size_t group_size = (side * side);

  Block *blocks = (Block *)malloc(group_size * sizeof(Block));
  if (!blocks) {
    printf("ERROR: failed to populate blocks\n");

    exit(1);
  }

  size_t cur_id = 0;
  for (size_t i = 0; i < side; ++i) {
    for (size_t j = 0; j < side; ++j) {
      Block *cur_block = (blocks + cur_id);
      // cur_block->size = get_block_size(cur_block->type);
      cur_block->size = (Vector2){ 4.f, GetRandomValue(2, 5) };

      cur_block->pos = (Vector3){
        .x = (side * 0.5f) - i * BLOCK_W,
        .y = -15.f + (cur_block->size.y * 0.5f),
        .z = (side * 0.5f) - j * BLOCK_W
      };
      cur_block->color = (Color){
        .r = GetRandomValue(0, 255),
        .g = GetRandomValue(0, 255),
        .b = GetRandomValue(0, 255),
        .a = 255,
      };
      cur_block->is_open = false;
      cur_block->type = NORMAL;

      printf("cur_block = %p\n", (void *)cur_block);
      printf("  pos = %.2fx%.2f\n", cur_block->pos.x, cur_block->pos.y);

      ++cur_id;
    }
  }

  return blocks;
}
