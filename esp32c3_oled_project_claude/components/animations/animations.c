#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "animations.h"
#include "esp_log.h"
#include "esp_random.h"

static const char *TAG = "ANIMATIONS";

static animation_type_t current_anim = ANIM_BOUNCING_BALL;

// Bouncing ball state
typedef struct {
    float x, y;
    float vx, vy;
} ball_t;

static ball_t ball;

// Starfield state
#define MAX_STARS 20
typedef struct {
    int x, y;
    int speed;
} star_t;

static star_t stars[MAX_STARS];

// Matrix rain state
#define MAX_DROPS 15
typedef struct {
    int x, y;
    int length;
    char chars[10];
} matrix_drop_t;

static matrix_drop_t drops[MAX_DROPS];

esp_err_t animations_init(void)
{
    animations_reset();
    ESP_LOGI(TAG, "Animations initialized");
    return ESP_OK;
}

void animations_reset(void)
{
    // Initialize bouncing ball
    ball.x = 64;
    ball.y = 32;
    ball.vx = 2.5;
    ball.vy = 1.8;
    
    // Initialize starfield
    for (int i = 0; i < MAX_STARS; i++) {
        stars[i].x = esp_random() % 128;
        stars[i].y = esp_random() % 64;
        stars[i].speed = (esp_random() % 3) + 1;
    }
    
    // Initialize matrix rain
    for (int i = 0; i < MAX_DROPS; i++) {
        drops[i].x = esp_random() % 128;
        drops[i].y = -(esp_random() % 64);
        drops[i].length = (esp_random() % 5) + 3;
        
        for (int j = 0; j < 10; j++) {
            drops[i].chars[j] = '!' + (esp_random() % 94); // Random ASCII
        }
    }
}

void animations_set_type(animation_type_t type)
{
    current_anim = type;
    animations_reset();
}

static void animate_bouncing_ball(ssd1306_handle_t display, uint32_t frame)
{
    // Update position
    ball.x += ball.vx;
    ball.y += ball.vy;
    
    // Bounce off walls
    if (ball.x <= 2 || ball.x >= 125) {
        ball.vx = -ball.vx;
        ball.x = (ball.x <= 2) ? 2 : 125;
    }
    if (ball.y <= 2 || ball.y >= 61) {
        ball.vy = -ball.vy;
        ball.y = (ball.y <= 2) ? 2 : 61;
    }
    
    // Draw ball (3x3 pixels)
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            ssd1306_draw_point(display, (int)ball.x + dx, (int)ball.y + dy, 1);
        }
    }
    
    // Draw walls
    ssd1306_draw_rectangle(display, 0, 0, 127, 63, 1);
}

static void animate_starfield(ssd1306_handle_t display, uint32_t frame)
{
    for (int i = 0; i < MAX_STARS; i++) {
        // Move star
        stars[i].x -= stars[i].speed;
        
        // Reset star if it goes off screen
        if (stars[i].x < 0) {
            stars[i].x = 128;
            stars[i].y = esp_random() % 64;
            stars[i].speed = (esp_random() % 3) + 1;
        }
        
        // Draw star
        ssd1306_draw_point(display, stars[i].x, stars[i].y, 1);
        
        // Draw trail for faster stars
        if (stars[i].speed > 2) {
            ssd1306_draw_point(display, stars[i].x + 1, stars[i].y, 1);
        }
    }
}

static void animate_matrix_rain(ssd1306_handle_t display, uint32_t frame)
{
    for (int i = 0; i < MAX_DROPS; i++) {
        // Move drop down
        drops[i].y++;
        
        // Reset drop if it goes off screen
        if (drops[i].y > 64 + drops[i].length * 8) {
            drops[i].x = esp_random() % 120; // Leave room for characters
            drops[i].y = -(esp_random() % 32);
            drops[i].length = (esp_random() % 4) + 2;
        }
        
        // Draw characters in the drop
        for (int j = 0; j < drops[i].length; j++) {
            int char_y = drops[i].y - j * 8;
            if (char_y >= 0 && char_y < 64) {
                // Brightest character at the head, dimmer towards tail
                int brightness = (j == 0) ? 1 : (esp_random() % 3 == 0 ? 1 : 0);
                if (brightness) {
                    char c = drops[i].chars[j % 10];
                    ssd1306_show_char(display, drops[i].x, char_y, c, 16, 1);
                }
            }
        }
    }
}

static void animate_wave(ssd1306_handle_t display, uint32_t frame)
{
    for (int x = 0; x < 128; x++) {
        int y1 = 32 + 20 * sin((x + frame * 2) * 0.1);
        int y2 = 32 + 15 * sin((x + frame * 3) * 0.08 + 1);
        
        ssd1306_draw_point(display, x, y1, 1);
        ssd1306_draw_point(display, x, y2, 1);
    }
    
    // Add some floating particles
    for (int i = 0; i < 5; i++) {
        int x = (frame * 2 + i * 25) % 128;
        int y = 32 + 10 * sin((x + frame) * 0.15 + i);
        ssd1306_draw_point(display, x, y, 1);
    }
}

static void animate_spiral(ssd1306_handle_t display, uint32_t frame)
{
    float center_x = 64;
    float center_y = 32;
    
    for (float angle = 0; angle < 6.28 * 3; angle += 0.2) {
        float radius = angle * 3 + frame * 0.5;
        if (radius > 40) continue;
        
        int x = center_x + radius * cos(angle);
        int y = center_y + radius * sin(angle);
        
        if (x >= 0 && x < 128 && y >= 0 && y < 64) {
            ssd1306_draw_point(display, x, y, 1);
        }
    }
}

void animations_update(ssd1306_handle_t display, uint32_t frame)
{
    switch (current_anim) {
        case ANIM_BOUNCING_BALL:
            animate_bouncing_ball(display, frame);
            break;
        case ANIM_STARFIELD:
            animate_starfield(display, frame);
            break;
        case ANIM_MATRIX_RAIN:
            animate_matrix_rain(display, frame);
            break;
        case ANIM_WAVE:
            animate_wave(display, frame);
            break;
        case ANIM_SPIRAL:
            animate_spiral(display, frame);
            break;
        default:
            ssd1306_show_string(display, 20, 28, "No Animation", 16, 1);
            break;
    }
}
