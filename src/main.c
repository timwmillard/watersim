#include <stdio.h>

#include <raylib.h>

#define WIDTH 800
#define HEIGHT 400

#define TILE_SIZE 10
#define TILE_WIDTH WIDTH / TILE_SIZE
#define TILE_HEIGHT HEIGHT / TILE_SIZE

typedef struct {
    float volume;
    int size;
} Droplet;

typedef struct {
    int width;
    int height;
    Droplet state[TILE_WIDTH * TILE_HEIGHT];
    // int tileSize;
} Game;


// Fill transfers water between two droplets at a controlled rate
void fill(Droplet *current, Droplet *target, float max_volume, float flow_rate)
{
    // Calculate how much water can be transferred
    float transfer = max_volume - target->volume; // remaider
    
    // Limit transfer to the flow rate (prevents instant teleportation)
    if (transfer > flow_rate) {
        transfer = flow_rate;
    }

    // Move water from source to target
    current->volume -= transfer;
    target->volume += transfer;
}

bool can_flow_down(int x, int y, Droplet *state)
{
    return y+1 < TILE_HEIGHT && state[(y+1)*TILE_WIDTH+x].volume < 1.0;
}

void try_horizontal_flow(int x, int y, Droplet *state)
{
    Droplet *current = &state[y*TILE_WIDTH+x];

    // Only cascade if there's water below
    bool has_water_below = y + 1 < TILE_HEIGHT && state[(y+1)*TILE_WIDTH+x].volume > 0.5;
    if (!has_water_below) return;

    // Cascade right - distribute to multiple cells
    for (int offset = 1; offset <= 3 && x+offset < TILE_WIDTH; offset++) {
        Droplet *target = &state[y*TILE_WIDTH+(x+offset)];
        if (target->volume < current->volume) {
            float flow_rate = (current->volume - target->volume) * 0.1 / offset;
            fill(current, target, 1.0, flow_rate);
        }
    }

    // Cascade left - distribute to multiple cells
    for (int offset = 1; offset <= 3 && x-offset >= 0; offset++) {
        Droplet *target = &state[y*TILE_WIDTH+(x-offset)];
        if (target->volume < current->volume) {
            float flow_rate = (current->volume - target->volume) * 0.1 / offset;
            fill(current, target, 1.0, flow_rate);
        }
    }
}

void create_water_generator(int x, int y, Droplet *state)
{
    state[y*TILE_WIDTH+x] = (Droplet){.size = TILE_SIZE, .volume = 1.0};
}

void droplet_draw(Droplet *d, int x, int y, bool has_water_above)
{
     // Convert grid coordinates to pixel coordinates
    int pix_x = x * TILE_SIZE;
    int pix_y = y * TILE_SIZE;
    if (d->volume > 0) {

        // Calculate visual height based on water volume
        // Full volume (1.0) = full tile height, half volume (0.5) = half height
        int height = TILE_SIZE * d->volume;

        // Fill up from the bottom
        int offset_y = TILE_SIZE - height;

        // If water above, fill from the top instead
        if (has_water_above) {
            offset_y = 0;
        }

        DrawRectangle(pix_x, pix_y+offset_y, TILE_SIZE, height, BLUE);
    }
}

void process_water_cell(int x, int y, Droplet *new_state)
{
    // Try to flow downwards, as if by gravity
    fill(&new_state[y*TILE_WIDTH+x],&new_state[(y+1)*TILE_WIDTH+x], 1.0, 0.5);

    // If all water flowed down, no need to continue
    if (new_state[y*TILE_WIDTH+x].volume == 0) return;

    // If water can still flow down, don't try other directions yet
    if (can_flow_down(x, y, new_state)) return;

    // Water spreads sideways when blocked below
    try_horizontal_flow(x, y, new_state);
}

void game_update(Game *g) 
{
    // Create a new state to avoid modifying the current state while reading it
    Droplet new_state[TILE_WIDTH * TILE_HEIGHT];

    // Copy current state to new state
    for (int y = 0; y < TILE_HEIGHT; y++) {
        for (int x = 0; x < TILE_WIDTH; x++) {
            new_state[y*TILE_WIDTH+x] = g->state[y*TILE_WIDTH+x];
        }
    }

    // Process the simulation from the bottom upwards
    for (int y = TILE_HEIGHT-1; y >= 0; y--) {
        for (int x = 0; x < TILE_WIDTH; x++) {
            // Only process cells that contain water
            if (g->state[y*TILE_WIDTH+x].volume > 0) {
                // Check if we are at the bottom boundary
                if (y+1 < TILE_HEIGHT) {
                    process_water_cell(x, y, new_state);
                }
            }
        }
    }

    // Copy new state back to current state
    for (int y = 0; y < TILE_HEIGHT; y++) {
        for (int x = 0; x < TILE_WIDTH; x++) {
            g->state[y*TILE_WIDTH+x] = new_state[y*TILE_WIDTH+x];
        }
    }
}

void game_draw(Game *g) 
{
    for (int y = 0; y < TILE_HEIGHT; y++) {
        for (int x = 0; x < TILE_WIDTH; x++) {
            // Check if there is water above this cell
            bool has_water_above = y > 0 && g->state[(y-1)*TILE_WIDTH+x].volume > 0;
            droplet_draw(&g->state[y*TILE_WIDTH+x], x, y, has_water_above);
        }
    }
}

int main()
{
    Game game = {.width = WIDTH, .height = HEIGHT};
    for (int y = 0; y < TILE_HEIGHT; y++) {
        for (int x = 0; x < TILE_WIDTH; x++) {
            game.state[y*TILE_WIDTH+x].volume = 0.0;
            game.state[y*TILE_WIDTH+x].size = TILE_SIZE;
        }
    }

    int frame_count = 0;
    int flow_start_x = 100 / TILE_SIZE;
    int flow_start_y = 100 / TILE_SIZE;

    SetTargetFPS(20);
    InitWindow(WIDTH, HEIGHT, "Water Simulator");

    while (!WindowShouldClose()) {
        frame_count++;
        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Add new water every 5 frames (creates continuous water stream)
        if (frame_count % 5 == 0) {
            create_water_generator(flow_start_x, flow_start_y, game.state);
            create_water_generator(flow_start_x+1, flow_start_y, game.state);
            create_water_generator(flow_start_x-1, flow_start_y, game.state);
        }

        game_draw(&game);
        game_update(&game);

        EndDrawing();
    }
    CloseWindow();

    return 0;
}

