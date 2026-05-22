#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <cstdlib>
#include <ctime>

// defining certain colors, where "color" is a struct of {r, g, b, a} with each value in 2 digit hex codes (raylib provides color struct)
#define ACOLOR {0x88, 0xFF, 0x77, 0xBB} // accent color, greenish tint
#define BCOLOR {0x11, 0x11, 0x11, 0xFF} // background color
#define FCOLOR RAYWHITE // foreground color (RAYWHITE is a predefined color in raylib, which is white with very mild gray)
#define PARTICLE_RADIUS 10 // particle radius integer

int PARTICLE_COUNT = 0;

// Generates a random spawn point for a particle within the window boundaries, essentially generates a valid vector and returns it
Vector2 generate_spawn(int xlimit, int ylimit) {
    // srand(time(NULL));
    Vector2 spawnPoint = {(float) (rand() % xlimit), (float) (rand() % ylimit)};
    spawnPoint.x += 20;
    spawnPoint.y += 20;
    return spawnPoint;
}

// Random bright pastel color (low saturation, vibrant colors) generation function
Color generate_color() {
    Color color = ColorFromHSV((float) (rand() % 360), 0.3, 1.0);
    // color.a = 255;
    return color;
}

// struct for the particle, a VerletObject is what we call a particle in the simulation
struct VerletObject
{
    Vector2 current_position;
    Vector2 old_position;
    Vector2 acceleration;
    float radius = PARTICLE_RADIUS;
    Color color = FCOLOR;
};

const Vector2 gravity = {0.0, 981.0};

// continuously running numerical integration, updates position of a particle (given as parameter)
// This is the "Verlet Integration" method
void update_position(VerletObject *obj, float dt) {
    const Vector2 velocity = obj->current_position - obj->old_position;
    obj->old_position = obj->current_position;
    obj->current_position += Vector2Add(velocity, Vector2Scale(obj->acceleration, dt * dt)); // Verlet equation
    obj->acceleration = {0.0, 0.0}; // we dont want accerleration to build up, it is set to correct values every frame
};

// accelerates a particle by give `acc` vector
void accelerate(VerletObject *obj, Vector2 acc) {
    obj->acceleration += acc;
};

// raylib's draw function, simplified, it takes a verlet object, reads its properties needed by DrawCircleV funtion, and draws them. Easier to use with pointers and dereferencing the particle <vector>
void draw_object(VerletObject *obj) {
    DrawCircleV(obj->current_position, obj->radius, obj->color);
};

// just like update_position, but with gravity, continously ran, and uses the accelerate function.
void apply_gravity(VerletObject *obj) {
    accelerate(obj, gravity);
};

// This creates a circular constraint, inside which particle reside and wont fall out of the cirlce. Its like particle in a cross sectioned soda can.
void apply_contraints(VerletObject *obj) {
    const Vector2 position = {800, 450}; // circual constraint's centre
    const float radius = 400.0; // cirle's radius. position and radius essentially defined the contraint within which particles can reside and not fall beyond
    const Vector2 to_obj = Vector2Subtract(obj->current_position, position);
    const float dist = Vector2Length(to_obj);
    if (dist > (radius - PARTICLE_RADIUS)) {
        const Vector2 n = Vector2Normalize(to_obj);
        obj->current_position = Vector2Add(position, Vector2Scale(n, radius - PARTICLE_RADIUS));
    }
}

// new contraint function that uses the same logic as above, but this time the constraint is the whole window.
void apply_window_contraints(VerletObject *obj) {
    const int screenWidth = GetScreenWidth();
    const int screenHeight = GetScreenHeight();
    const Vector2 position = obj->current_position;

    if (position.x - obj->radius < 0) {
        obj->current_position.x = obj->radius;
    } else if (position.x + obj->radius > screenWidth) {
        obj->current_position.x = screenWidth - obj->radius;
    }

    if (position.y - obj->radius < 0) {
        obj->current_position.y = obj->radius;
    } else if (position.y + obj->radius > screenHeight) {
        obj->current_position.y = screenHeight - obj->radius;
    }
}

// collision_axis based collision system, powered by O(n^2) algorithm and brute force
// iterates through the verlet object <vector>, chooses two particles and draws an axis between the particles' centre, and then checks if the axis is smaller than the sum of the two particle radii, if yes, boom, they are in collision, move them away along the axis.
void solve_collisions(std::vector<VerletObject> *verlet_objects) {
    const size_t object_count = verlet_objects->size();
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

// Harnesses the generate_spawn is to generate a random spawn point for a particle, while also ensuring that the particle does not overlap with other particles, hence a dedicated function to spawn particles properly.
void spawn_particle(std::vector<VerletObject> *verlet_objects) {
    VerletObject obj;
    Vector2 spawn_point = generate_spawn(GetScreenWidth() - 40, GetScreenHeight() - 40);
    bool overlap = true;
    while (overlap) {
        spawn_point = generate_spawn(GetScreenWidth() - 40, GetScreenHeight() - 40);
        overlap = false;
        for (auto& other_obj : *verlet_objects) {
            Vector2 to_obj = Vector2Subtract(spawn_point, other_obj.current_position);
            float dist = Vector2Length(to_obj);
            if (dist < (obj.radius + other_obj.radius)) {
                overlap = true;
                break;
            }
        }
    }
    // particle creation : default values
    obj.acceleration = {0.0, 0.0};
    obj.radius = (float) (rand() % 10 + 5);
    obj.current_position = spawn_point;
    obj.old_position = obj.current_position;
    obj.color = generate_color();
    verlet_objects->push_back(obj);
    PARTICLE_COUNT++;
}

// THE VERLET OBJECTS <vector>
std::vector<VerletObject> verlet_objects;

// main update loop, this function is called every frame
void update(float dt) {
    for (auto& obj : verlet_objects) {
        apply_gravity(&obj);
        update_position(&obj, dt); // Call update_positions here
        apply_window_contraints(&obj);
    }
    solve_collisions(&verlet_objects);
}

void main() {

    SetTargetFPS(120);
    InitWindow(1600, 900, "Verlet Integration");

    // Initial Particle generation, generates PARTICLE_COUNT number of particles with all those non overlapping safety measures defined as same as in spawn_particle, for some reason, when using pointers and stuff, it felt error free to copy paste the code rather than calling the function.
    for(int i = 0; i < PARTICLE_COUNT; i++) {
        VerletObject obj;
        Vector2 spawn_point = generate_spawn(GetScreenWidth() - 40, GetScreenHeight() - 40);
        bool overlap = true;
        while (overlap) {
            spawn_point = generate_spawn(GetScreenWidth() - 40, GetScreenHeight() - 40);
            overlap = false;
            for (auto& other_obj : verlet_objects) {
                Vector2 to_obj = Vector2Subtract(spawn_point, other_obj.current_position);
                float dist = Vector2Length(to_obj);
                if (dist < (obj.radius + other_obj.radius)) {
                    overlap = true;
                    break;
                }
            }
        }
        obj.acceleration = {0.0, 0.0};
        obj.radius = (float) (rand() % 10 + 5);
        obj.current_position = spawn_point;
        obj.old_position = obj.current_position;
        obj.color = generate_color();
        verlet_objects.push_back(obj);
    }
    
    // WindowShouldClose() is like a safe method to get whether the window should close or not, may be used by raylib internals to handle window closing step by step to avoid bugs and error. We used this to find ourselves when to stop the simulation loop, that is, stop loop when window is about to close.
    while(!WindowShouldClose()) {

        if (IsKeyPressed(KEY_SPACE)) {
            spawn_particle(&verlet_objects); // this is where spawn particle function is called and used :_)
            // update(GetFrameTime());
        };

        BeginDrawing();
    
        ClearBackground(BCOLOR); // clear screen by setting the entire screen with background color, for the next frame to be drawn.

        // add substeps for cleaner collison yet do not compromise performance and physics speed (i dont want slow mo result)

        for (int i = 0; i < 10; i++) {
            update(GetFrameTime() / 10);
        } // this looping and dividing the time by 10 is called the substepping the updation, that is, for every frame, 10 times the physics is updated.
        // or we can say, for every 10 physics update, update frame once, while fps is fixed to 60, physics updation becomes fast.
        // this results in more accurate result.
        // more substeps, more accuracy, less speed
        
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, ACOLOR); // guess what these two lines are ...
        DrawText(TextFormat("Particles: %i", PARTICLE_COUNT), 10, 30, 20, ACOLOR);

        for (auto& obj : verlet_objects) {
            draw_object(&obj);
        }
        
        EndDrawing();

        if (GetFPS() > 60) {
            spawn_particle(&verlet_objects);
        }
    }

    CloseWindow();

}