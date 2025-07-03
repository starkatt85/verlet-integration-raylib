#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cstdlib>
#include <ctime>

#define ACOLOR {0x88, 0xFF, 0x77, 0xBB}
#define BCOLOR {0x11, 0x11, 0x11, 0xFF}
#define FCOLOR RAYWHITE
#define PARTICLE_RADIUS 8

int PARTICLE_COUNT = 50;


Vector2 generate_spawn(int xlimit, int ylimit) {
    // srand(time(0));
    Vector2 spawnPoint = {rand() % xlimit, rand() % ylimit};
    return spawnPoint;
}

struct VerletObject
{
    Vector2 current_position;
    Vector2 old_position;
    Vector2 acceleration;
    float radius = PARTICLE_RADIUS;
    Color color = FCOLOR;
};

const Vector2 gravity = {0.0, 981.0};

void update_position(VerletObject *obj, float dt) {
    const Vector2 velocity = obj->current_position - obj->old_position;
    obj->old_position = obj->current_position;
    obj->current_position += Vector2Add(velocity, Vector2Scale(obj->acceleration, dt * dt));
    obj->acceleration = {0.0, 0.0};
};

void accelerate(VerletObject *obj, Vector2 acc) {
    obj->acceleration += acc;
};

void draw_object(VerletObject *obj) {
    DrawCircleV(obj->current_position, obj->radius, obj->color);
};

void apply_gravity(VerletObject *obj) {
    accelerate(obj, gravity);
};

void apply_contraints(VerletObject *obj) {
    const Vector2 position = {800, 450};
    const float radius = 400.0;
    const Vector2 to_obj = Vector2Subtract(obj->current_position, position);
    const float dist = Vector2Length(to_obj);
    if (dist > (radius - PARTICLE_RADIUS)) {
        const Vector2 n = Vector2Normalize(to_obj);
        obj->current_position = Vector2Add(position, Vector2Scale(n, radius - PARTICLE_RADIUS));
    }
}

// collision_axis based collision system, powered by O(n^2) algorithm and brute force
void solve_collisions(std::vector<VerletObject> *verlet_objects) {
    const uint32_t object_count = verlet_objects->size();
    auto& object_container = *verlet_objects;
    for (uint32_t i = 0; i < object_count; i++) {
        VerletObject& obj = object_container[i];
        for (uint32_t j = i + 1; j < object_count; j++) {
            VerletObject& other_obj = object_container[j];
            Vector2 collision_axis = Vector2Subtract(obj.current_position, other_obj.current_position);
            float dist = Vector2Length(collision_axis);
            if (dist < (obj.radius + other_obj.radius)) {
                Vector2 n = Vector2Normalize(collision_axis);
                float delta = (obj.radius + other_obj.radius) - dist;
                obj.current_position = Vector2Add(obj.current_position, Vector2Scale(n, delta/2));
                other_obj.current_position = Vector2Subtract(other_obj.current_position, Vector2Scale(n, delta/2));
            }
        }
    }
}

void spawn_particle(std::vector<VerletObject> *verlet_objects) {
    VerletObject new_obj;
    Vector2 spawnPoint = generate_spawn(1600, 900);
    do {
        spawnPoint = generate_spawn(1600, 900);
    } while (Vector2Length(Vector2Subtract(spawnPoint, Vector2{800, 450})) < 400);
    new_obj.current_position = spawnPoint;
    new_obj.old_position = new_obj.current_position;
    verlet_objects->push_back(new_obj);
    PARTICLE_COUNT++;

}

std::vector<VerletObject> verlet_objects;

void update(float dt) {
    for (auto& obj : verlet_objects) {
        apply_gravity(&obj);
        update_position(&obj, dt); // Call update_positions here
        apply_contraints(&obj);
    }
    solve_collisions(&verlet_objects);
}

void main() {

    for(int i = 0; i < PARTICLE_COUNT; i++) {
        VerletObject obj;
        Vector2 spawnPoint = {0.0, 0.0};
        do {
            spawnPoint = generate_spawn(1600, 900);
        } while (Vector2Length(Vector2Subtract(spawnPoint, Vector2{800, 450})) < 400);
        obj.current_position = spawnPoint;
        obj.old_position = obj.current_position;
        verlet_objects.push_back(obj);
    }

    SetTargetFPS(60);
    InitWindow(1600, 900, "Verlet Integration");
    

    while(!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            spawn_particle(&verlet_objects);
        };

        BeginDrawing();
    
        ClearBackground(BCOLOR);

        // add substeps for cleaner collison yet do not compromise performance and physics speed (i dont want slow mo result)

        for (int i = 0; i < 10; i++) {
            update(GetFrameTime() / 10);
        }
        
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, ACOLOR);
        DrawText(TextFormat("Particles: %i", PARTICLE_COUNT), 10, 30, 20, ACOLOR);

        for (auto& obj : verlet_objects) {
            draw_object(&obj);
        }
        
        EndDrawing();

        // if (GetFPS() > 30) {
        //     spawn_particle(&verlet_objects);
        // }
    }

    CloseWindow();

}