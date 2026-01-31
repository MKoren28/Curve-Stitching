#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define CENTER_X (WINDOW_WIDTH / 2)
#define CENTER_Y (WINDOW_HEIGHT / 2)
#define SCALE 1.8

typedef struct {
    float x, y;
} Point;

typedef struct {
    unsigned char r, g, b;
} Color;

void world_to_screen(Point world, int *screen_x, int *screen_y) {
    *screen_x = CENTER_X + (int)(world.x * SCALE);
    *screen_y = CENTER_Y - (int)(world.y * SCALE);
}

// Color based on angle: vertical = red, horizontal = teal
// Red -> Orange -> Yellow -> Green -> Teal
Color get_color_for_angle(Point start, Point end) {
    Color c;
    
    // Calculate the angle of the line
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float angle = atan2(fabs(dy), fabs(dx)); // Angle from horizontal (0 to PI/2)
    
    // angle = 0 means horizontal (teal)
    // angle = PI/2 means vertical (red)
    float t = angle / (M_PI / 2.0); // Normalize to 0-1, where 1 = vertical, 0 = horizontal
    
    // Create rainbow gradient from teal (t=0) to red (t=1)
    if (t < 0.25) {
        // Teal to Green: t = 0 to 0.25
        float local_t = t / 0.25;
        c.r = 0;
        c.g = 255;
        c.b = (unsigned char)(255 * (1 - local_t));
    } else if (t < 0.5) {
        // Green to Yellow: t = 0.25 to 0.5
        float local_t = (t - 0.25) / 0.25;
        c.r = (unsigned char)(255 * local_t);
        c.g = 255;
        c.b = 0;
    } else if (t < 0.75) {
        // Yellow to Orange: t = 0.5 to 0.75
        float local_t = (t - 0.5) / 0.25;
        c.r = 255;
        c.g = (unsigned char)(255 * (1 - local_t * 0.5)); // Keep some green
        c.b = 0;
    } else {
        // Orange to Red: t = 0.75 to 1.0
        float local_t = (t - 0.75) / 0.25;
        c.r = 255;
        c.g = (unsigned char)(128 * (1 - local_t)); // Fade out remaining green
        c.b = 0;
    }
    
    return c;
}

void draw_line(SDL_Renderer *renderer, Point p1, Point p2, Color color) {
    int x1, y1, x2, y2;
    world_to_screen(p1, &x1, &y1);
    world_to_screen(p2, &x2, &y2);
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    SDL_Window *window = SDL_CreateWindow(
        "Star Pattern Animation - 20 Lines",
        100,
        100,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event e;
    bool quit = false;
    int line_count = 0;
    int total_lines = 84; // 20 lines per quadrant * 4 quadrants + 4 axis lines
    
    unsigned int last_time = SDL_GetTicks();
    
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        int current_line = 0;
        Color color;
        Point start, end;
        
        // Quadrant 1: Top-right - from (0,200) to (10,0), then (0,190) to (20,0), etc.
        for (int i = 0; i < 20 && current_line < line_count; i++, current_line++) {
            start.x = 0;
            start.y = 200 - i * 10;
            end.x = (i + 1) * 10;
            end.y = 0;
            color = get_color_for_angle(start, end);
            draw_line(renderer, start, end, color);
        }
        
        // Perfect vertical line (0,0) to (0,200) - RED
        if (current_line < line_count) {
            start.x = 0; start.y = 0;
            end.x = 0; end.y = 200;
            color.r = 255; color.g = 0; color.b = 0;
            draw_line(renderer, start, end, color);
            current_line++;
        }
        
        // Perfect horizontal line (0,0) to (200,0) - TEAL
        if (current_line < line_count) {
            start.x = 0; start.y = 0;
            end.x = 200; end.y = 0;
            color.r = 0; color.g = 255; color.b = 255;
            draw_line(renderer, start, end, color);
            current_line++;
        }
        
        // Quadrant 2: Top-left - from (0,200) to (-10,0), then (0,190) to (-20,0), etc.
        for (int i = 0; i < 20 && current_line < line_count; i++, current_line++) {
            start.x = 0;
            start.y = 200 - i * 10;
            end.x = -(i + 1) * 10;
            end.y = 0;
            color = get_color_for_angle(start, end);
            draw_line(renderer, start, end, color);
        }
        
        // Perfect horizontal line (0,0) to (-200,0) - TEAL
        if (current_line < line_count) {
            start.x = 0; start.y = 0;
            end.x = -200; end.y = 0;
            color.r = 0; color.g = 255; color.b = 255;
            draw_line(renderer, start, end, color);
            current_line++;
        }
        
        // Quadrant 3: Bottom-left - from (0,-200) to (-10,0), then (0,-190) to (-20,0), etc.
        for (int i = 0; i < 20 && current_line < line_count; i++, current_line++) {
            start.x = 0;
            start.y = -200 + i * 10;
            end.x = -(i + 1) * 10;
            end.y = 0;
            color = get_color_for_angle(start, end);
            draw_line(renderer, start, end, color);
        }
        
        // Perfect vertical line (0,0) to (0,-200) - RED
        if (current_line < line_count) {
            start.x = 0; start.y = 0;
            end.x = 0; end.y = -200;
            color.r = 255; color.g = 0; color.b = 0;
            draw_line(renderer, start, end, color);
            current_line++;
        }
        
        // Quadrant 4: Bottom-right - from (0,-200) to (10,0), then (0,-190) to (20,0), etc.
        for (int i = 0; i < 20 && current_line < line_count; i++, current_line++) {
            start.x = 0;
            start.y = -200 + i * 10;
            end.x = (i + 1) * 10;
            end.y = 0;
            color = get_color_for_angle(start, end);
            draw_line(renderer, start, end, color);
        }
        
        SDL_RenderPresent(renderer);
        
        unsigned int current_time = SDL_GetTicks();
        if (current_time - last_time > 30 && line_count < total_lines) {
            line_count++;
            last_time = current_time;
        }
        
        SDL_Delay(16);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
