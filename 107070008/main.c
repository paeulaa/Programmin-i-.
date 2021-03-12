#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */

// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 800;
// Display (screen) height.
const int SCREEN_H = 600;
// At most 4 audios can be played at a time.
const int RESERVE_SAMPLES = 4;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
    SCENE_MENU = 1,
    SCENE_START = 2
    // [HACKATHON 3-7]
    // TODO: Declare a new scene id.
    , SCENE_SETTINGS = 3
    , SCENE_GAMEOVER = 4
    , SCENE_VICTORY = 5
};

/* Input states */

// The active scene id.
int active_scene;
bool dir = false;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...
int score=0;
int HP=100;
int mad=0;
int hit=0;
int which=1;
/* Variables for allegro basic routines. */

ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_pirulen_18;
// TODO: More shared resources or data that needed to be accessed
// across different scenes.

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
// [HACKATHON 3-1]
// TODO: Declare 2 variables for storing settings images.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* setting_img_background;
ALLEGRO_BITMAP* gameover_img_background;
ALLEGRO_BITMAP* victory_img_background;
//ALLEGRO_BITMAP* img_back;
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;
ALLEGRO_SAMPLE* setting_bgm;
ALLEGRO_SAMPLE_ID setting_bgm_id;
ALLEGRO_SAMPLE* vic_bgm;
ALLEGRO_SAMPLE_ID vic_bgm_id;
ALLEGRO_SAMPLE* go_bgm;
ALLEGRO_SAMPLE_ID go_bgm_id;


/* Start Scene resources*/
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_plane;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_BITMAP* img_boss;
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
// [HACKATHON 2-1]
// TODO: Declare a variable to store your bullet's image.
// Uncomment and fill in the code below.
ALLEGRO_BITMAP* img_bullet;

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.
    ALLEGRO_BITMAP* img;
} MovableObject;
void draw_movable_object(MovableObject obj);
#define MAX_ENEMY 5
// [HACKATHON 2-2]
// TODO: Declare the max bullet count that will show on screen.
// You can try max 4 bullets here and see if you needed more.
// Uncomment and fill in the code below.
#define MAX_BULLET 4
MovableObject plane;
MovableObject enemies[MAX_ENEMY];
MovableObject boss;
// [HACKATHON 2-3]
// TODO: Declare an array to store bullets with size of max bullet count.
// Uncomment and fill in the code below.
MovableObject bullets[MAX_BULLET];
// [HACKATHON 2-4]
// TODO: Set up bullet shooting cool-down variables.
// 1) Declare your shooting cool-down time as constant. (0.2f will be nice)
// 2) Declare your last shoot timestamp.
// Uncomment and fill in the code below.
const float MAX_COOLDOWN = 0.2;
double last_shoot_timestamp;

/* Declare function prototypes. */

// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
//void textprintf_ex(BITMAP*bmp,const*FONT*f,int x,int y,int color,int bg,const char*fmt);
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void enemies_run(void);
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// [HACKATHON 3-2]
// TODO: Declare a function.
// Determines whether the point (px, py) is in rect (x, y, w, h).
// Uncomment the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */

// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
    // Set random seed for better random outcome.
    srand(time(NULL));
    allegro5_init();
    game_log("Allegro5 initialized");
    game_log("Game begin");
    // Initialize game variables.
    game_init();
    game_log("Game initialized");
    // Draw the first frame.
    game_draw();
    game_log("Game start event loop");
    // This call blocks until the game is finished.
    game_start_event_loop();
    game_log("Game end");
    game_destroy();
    return 0;
}

void allegro5_init(void) {
    if (!al_init())
        game_abort("failed to initialize allegro");
    
    // Initialize add-ons.
    if (!al_init_primitives_addon())
        game_abort("failed to initialize primitives add-on");
    if (!al_init_font_addon())
        game_abort("failed to initialize font add-on");
    if (!al_init_ttf_addon())
        game_abort("failed to initialize ttf add-on");
    if (!al_init_image_addon())
        game_abort("failed to initialize image add-on");
    if (!al_install_audio())
        game_abort("failed to initialize audio add-on");
    if (!al_init_acodec_addon())
        game_abort("failed to initialize audio codec add-on");
    if (!al_reserve_samples(RESERVE_SAMPLES))
        game_abort("failed to reserve samples");
    if (!al_install_keyboard())
        game_abort("failed to install keyboard");
    if (!al_install_mouse())
        game_abort("failed to install mouse");
    // TODO: Initialize other addons such as video, ...
    
    // Setup game display.
    game_display = al_create_display(SCREEN_W, SCREEN_H);
    if (!game_display)
        game_abort("failed to create display");
    al_set_window_title(game_display, "I2P(I)_2018_Yang Final Project <student_id>");
    
    // Setup update timer.
    game_update_timer = al_create_timer(1.0f / FPS);
    if (!game_update_timer)
        game_abort("failed to create timer");
    
    // Setup event queue.
    game_event_queue = al_create_event_queue();
    if (!game_event_queue)
        game_abort("failed to create event queue");
    
    // Malloc mouse buttons state according to button counts.
    const unsigned m_buttons = al_get_mouse_num_buttons();
    game_log("There are total %u supported mouse buttons", m_buttons);
    // mouse_state[0] will not be used.
    mouse_state = malloc((m_buttons + 1) * sizeof(bool));
    memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));
    
    // Register display, timer, keyboard, mouse events to the event queue.
    al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
    al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
    al_register_event_source(game_event_queue, al_get_keyboard_event_source());
    al_register_event_source(game_event_queue, al_get_mouse_event_source());
    // TODO: Register other event sources such as timer, video, ...
    
    // Start the timer to update and draw the game.
    al_start_timer(game_update_timer);
}

void game_init(void) {
    /* Shared resources*/
    font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
    if (!font_pirulen_32)
        game_abort("failed to load font: pirulen.ttf with size 32");
    
    font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
    if (!font_pirulen_24)
        game_abort("failed to load font: pirulen.ttf with size 24");
    
    font_pirulen_18 = al_load_font("pirulen.ttf", 12, 0);
    if (!font_pirulen_18)
        game_abort("failed to load font: pirulen.ttf with size 12");
    /* Menu Scene resources*/
    main_img_background = load_bitmap_resized("menu_background.png", SCREEN_W, SCREEN_H);
    
    main_bgm = al_load_sample("m_set_94.ogg");
    if (!main_bgm)
        game_abort("failed to load audio: m_set_94.ogg");
    
    setting_bgm = al_load_sample("onceagain.ogg");
    if (!setting_bgm)
        game_abort("failed to load audio: onceagain.ogg");
    
    vic_bgm = al_load_sample("funnysong.ogg");
    if (!vic_bgm)
        game_abort("failed to load audio: funnysong.ogg");
    
    go_bgm = al_load_sample("sadday.ogg");
    if (!go_bgm)
        game_abort("failed to load audio: sadday.ogg");
    
    // [HACKATHON 3-4]
    // TODO: Load settings images.
    // Don't forget to check their return values.
    // Uncomment and fill in the code below.
    setting_img_background = load_bitmap_resized("IMG_0081.png", SCREEN_W, SCREEN_H);
    gameover_img_background = load_bitmap_resized("gameover.png", SCREEN_W, SCREEN_H);
    victory_img_background = load_bitmap_resized("vic.png", SCREEN_W, SCREEN_H);
    /*img_back= load_bitmap_resized("back.png",100,100);
     if (!img_back)
     game_abort("failed to load image: settings.png");*/
    img_settings = load_bitmap_resized("settings2.png",50,50);
    if (!img_settings)
        game_abort("failed to load image: settings.png");
    img_settings2 = load_bitmap_resized("settings.png",50,50);
    if (!img_settings2)
        game_abort("failed to load image: settings2.png");
    /* Start Scene resources*/
    
    start_img_background = load_bitmap_resized("start.png", SCREEN_W, SCREEN_H);
    
    start_img_plane = al_load_bitmap("plane2.png");
    if (!start_img_plane)
        game_abort("failed to load image: plane2.png");
    
    start_img_enemy = al_load_bitmap("smallfighter.png");
    if (!start_img_enemy)
        game_abort("failed to load image: smallfighter.png");
    
    img_boss = al_load_bitmap("boss1.png");
    if (!img_boss)
        game_abort("failed to load image: boss1.png");
    
    start_bgm = al_load_sample("creepy.ogg");
    if (!start_bgm)
        game_abort("failed to load audio: creepy.ogg");
    
    // [HACKATHON 2-5-1]
    // TODO: Initialize bullets.
    // 1) Search for a bullet image online and put it in your project.
    //    You can use the image we provided.
    // 2) Load it in by 'al_load_bitmap' or 'load_bitmap_resized'.
    // 3) If you use 'al_load_bitmap', don't forget to check its return value.
    // Uncomment and fill in the code below.
    img_bullet = al_load_bitmap("burger.png");
    if (!img_bullet)
        game_abort("failed to load image: burger.png");
    // Change to first scene.
    game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
    bool done = false;
    ALLEGRO_EVENT event;
    int redraws = 0;
    while (!done) {
        al_wait_for_event(game_event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            // Event for clicking the window close button.
            game_log("Window close button clicked");
            done = true;
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            // Event for redrawing the display.
            if (event.timer.source == game_update_timer)
                // The redraw timer has ticked.
                redraws++;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Event for keyboard key down.
            game_log("Key with keycode %d down", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = true;
            on_key_down(event.keyboard.keycode);
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            // Event for keyboard key up.
            game_log("Key with keycode %d up", event.keyboard.keycode);
            key_state[event.keyboard.keycode] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Event for mouse key down.
            game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = true;
            on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            // Event for mouse key up.
            game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
            mouse_state[event.mouse.button] = false;
        } else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            if (event.mouse.dx != 0 || event.mouse.dy != 0) {
                // Event for mouse move.
                game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
                mouse_x = event.mouse.x;
                mouse_y = event.mouse.y;
            } else if (event.mouse.dz != 0) {
                // Event for mouse scroll.
                game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
            }
        }
        // TODO: Process more events and call callbacks by adding more
        // entries inside Scene.
        
        // Redraw
        if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
            if (redraws > 1)
                game_log("%d frame(s) dropped", redraws - 1);
            // Update and draw the next frame.
            game_update();
            game_draw();
            redraws = 0;
        }
    }
}

void game_update(void) {
    if (active_scene == SCENE_START) {
        //if(score<100){
        for(int j=0;j<MAX_ENEMY;j++){
            for (int i=0; i < MAX_BULLET; i++) {
                enemies[j].y += 0.5;
                if(enemies[j].y - (enemies[j].h)/2 > SCREEN_H){
                    enemies[j].x = enemies[j].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[j].w);
                    enemies[j].y = 80;
                }else if(abs(enemies[j].x - bullets[i].x) <= 40 && abs(enemies[j].y- bullets[i].y) <= 50){
                    enemies[j].x = enemies[j].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[j].w);
                    enemies[j].y = 80;
                    bullets[i].hidden =true;
                    bullets[i].x = plane.x;
                    bullets[i].y = plane.y;
                    score+=10;
                }
            }if(abs(enemies[j].x - plane.x) <= 40 && abs(enemies[j].y- plane.y) <= 90){
                enemies[j].x = enemies[j].w  + (float)rand() / RAND_MAX * (SCREEN_W - enemies[j].w);
                enemies[j].y = 80;
                HP-=10;
            }
        }
        /*}else if(score>=100){
         for(int j=0;j<MAX_ENEMY;j++){
         enemies[j].hidden = true;
         }
         boss.hidden = false;
         boss.y += 0.5;
         for (int i=0; i < MAX_BULLET; i++) {
         if(boss.x - bullets[i].x <= 10 && boss.x + bullets[i].x >=50 && boss.y- bullets[i].y <= 5){
         hit+=1;
         }
         }
         if(hit >= 5){
         boss.hidden = true;
         game_change_scene(SCENE_VICTORY);
         hit=0;
         HP=100;
         score=0;
         }if(boss.y - (boss.h)/2 > SCREEN_H){
         game_change_scene(SCENE_GAMEOVER);
         HP=100;
         score=0;
         }
         }*/
        plane.vx = plane.vy = 0;
        if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
            plane.vy -= 1;
        if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
            plane.vy += 1;
        if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
            plane.vx -= 1;
        if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
            plane.vx += 1;
        // 0.71 is (1/sqrt(2)).
        plane.y += plane.vy * 4 * (plane.vx ? 0.71f : 1);
        plane.x += plane.vx * 4 * (plane.vy ? 0.71f : 1);
        // [HACKATHON 1-1]
        // TODO: Limit the plane's collision box inside the frame.
        //       (x, y axes can be separated.)
        // Uncomment and fill in the code below.
        if (plane.x-(plane.w)/2 < 0)
            plane.x = (plane.w)/2 ;
        else if (plane.x+(plane.w) > SCREEN_W)
            plane.x = SCREEN_W-(plane.w);
        if (plane.y-(plane.h)/2 < 0)
            plane.y = (plane.h)/2;
        else if (plane.y + (plane.h) > SCREEN_H)
            plane.y = SCREEN_H-78;//(plane.h)/2;
        // [HACKATHON 2-6]
        // TODO: Update bullet coordinates.
        // 1) For each bullets, if it's not hidden, update x, y
        // according to vx, vy.
        // 2) If the bullet is out of the screen, hide it.
        // Uncomment and fill in the code below.
        int i;
        for (i=0;i< MAX_BULLET;i++) {
            if (bullets[i].hidden == true)
                continue;
            bullets[i].x += bullets[i].vx;
            bullets[i].y += bullets[i].vy;
            if (bullets[i].y < 0)
                bullets[i].hidden = true;
        }
        
        // [HACKATHON 2-7]
        // TODO: Shoot if key is down and cool-down is over.
        // 1) Get the time now using 'al_get_time'.
        // 2) If Space key is down in 'key_state' and the time
        //    between now and last shoot is not less that cool
        //    down time.
        // 3) Loop through the bullet array and find one that is hidden.
        //    (This part can be optimized.)
        // 4) The bullet will be found if your array is large enough.
        // 5) Set the last shoot time to now.
        // 6) Set hidden to false (recycle the bullet) and set its x, y to the
        //    front part of your plane.
        // Uncomment and fill in the code below.
        double now = al_get_time();
        if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
            for (i=0; i < MAX_BULLET; i++) {
                if (bullets[i].hidden == true)
                    break;
            }
            if (i == MAX_BULLET)
                return;
            last_shoot_timestamp = now;
            bullets[i].hidden = false;
            mad+=20;
            bullets[i].x = plane.x;
            bullets[i].y = plane.y;
        }if(HP == 0){
            game_change_scene(SCENE_GAMEOVER);
            HP=100;
            score=0;
        }else if(score >= 100){
            game_change_scene(SCENE_VICTORY);
            HP=100;
            score=0;
        }if(mad >= 100){
            //al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 112, 112, 0, "Press enter key to kill enemies");
            if (key_state[ALLEGRO_KEY_ENTER]){
                for(int j=0;j<MAX_ENEMY;j++){
                    enemies[j].y = -200;
                    score+=10;
                    mad=0;
                }
            }
        }
    }else if(active_scene == SCENE_SETTINGS){
        if (key_state[ALLEGRO_KEY_1]){
            img_bullet = al_load_bitmap("burger.png");
            if (!img_bullet)
                game_abort("failed to load image: burger.png");
        }else if (key_state[ALLEGRO_KEY_2]){
            img_bullet = al_load_bitmap("pizza.png");
            if (!img_bullet)
                game_abort("failed to load image: pizza.png");
        }
    }
}

void game_draw(void) {
    if (active_scene == SCENE_MENU) {
        al_draw_bitmap(main_img_background, 0, 0, 0);
        al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Give me burgers");
        al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start");
        // [HACKATHON 3-5]
        // TODO: Draw settings images.
        // The settings icon should be located at (x, y, w, h) =
        // (SCREEN_W - 48, 10, 38, 38).
        // Change its image according to your mouse position.
        // Uncomment and fill in the code below.
        if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
            al_draw_bitmap(img_settings2, 10, 38, 38);
        else
            al_draw_bitmap(img_settings, 10, 38, 38);
    } else if (active_scene == SCENE_START) {
        int i;
        al_draw_bitmap(start_img_background, 0, 0, 0);
        //al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 100, 30, ALLEGRO_ALIGN_CENTER, "Score : ");
        
        al_draw_textf(font_pirulen_24,al_map_rgb(255,255,255),100,60,-1,"Score:%d",score);
        /*else if(score>100){
         al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 100, 60, -1, "Score:100");
         }*/
        //al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 68, 60, ALLEGRO_ALIGN_CENTER, "HP : ");
        al_draw_textf(font_pirulen_24,al_map_rgb(255,255,255),100,90,-1,"HP:%d",HP);
        al_draw_textf(font_pirulen_24,al_map_rgb(255,255,255),100,120,-1,"mad:%d",mad);
        if(mad >= 100){
            al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to kill enemies");
        }
        //printf("%d",HP);
        // [HACKATHON 2-8]
        // TODO: Draw all bullets in your bullet array.
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_BULLET; i++)
            draw_movable_object(bullets[i]);
        draw_movable_object(plane);
        draw_movable_object(boss);
        for (i = 0; i < MAX_ENEMY; i++)
            draw_movable_object(enemies[i]);
    }
    // [HACKATHON 3-9]
    // TODO: If active_scene is SCENE_SETTINGS.
    // Draw anything you want, or simply clear the display.
    else if (active_scene == SCENE_SETTINGS) {
        al_draw_bitmap(setting_img_background, 0, 0, 0);
        //al_clear_to_color(al_map_rgb(0, 0, 0));
        //al_map_rgb(255, 255, 255);
        //al_draw_bitmap(img_back, 10, SCREEN_H-100, 38);
    }else if (active_scene == SCENE_GAMEOVER) {
        al_draw_bitmap(gameover_img_background, 0, 0, 0);
        //al_draw_bitmap(img_back, 10, SCREEN_H-100, 38);
    }else if (active_scene == SCENE_VICTORY) {
        al_draw_bitmap(victory_img_background, 0, 0, 0);
        //al_draw_bitmap(img_back, 10, SCREEN_H-100, 38);
    }
    al_flip_display();
}

void game_destroy(void) {
    // Destroy everything you have created.
    // Free the memories allocated by malloc or allegro functions.
    // Destroy shared resources.
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);
    al_destroy_font(font_pirulen_18);
    
    /* Menu Scene resources*/
    al_destroy_bitmap(main_img_background);
    al_destroy_sample(main_bgm);
    al_destroy_sample(setting_bgm);
    al_destroy_sample(vic_bgm);
    al_destroy_sample(go_bgm);
    // [HACKATHON 3-6]
    // TODO: Destroy the 2 settings images.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(setting_img_background);
    al_destroy_bitmap(gameover_img_background);
    al_destroy_bitmap(victory_img_background);
    //al_destroy_bitmap(img_back);
    al_destroy_bitmap(img_settings);
    al_destroy_bitmap(img_settings2);
    
    /* Start Scene resources*/
    al_destroy_bitmap(start_img_background);
    al_destroy_bitmap(start_img_plane);
    al_destroy_bitmap(start_img_enemy);
    al_destroy_bitmap(img_boss);
    al_destroy_sample(start_bgm);
    // [HACKATHON 2-9]
    // TODO: Destroy your bullet image.
    // Uncomment and fill in the code below.
    al_destroy_bitmap(img_bullet);
    
    al_destroy_timer(game_update_timer);
    al_destroy_event_queue(game_event_queue);
    al_destroy_display(game_display);
    free(mouse_state);
}

void game_change_scene(int next_scene) {
    game_log("Change scene from %d to %d", active_scene, next_scene);
    // TODO: Destroy resources initialized when creating scene.
    if (active_scene == SCENE_MENU) {
        al_stop_sample(&main_bgm_id);
        game_log("stop audio (bgm)");
    } else if (active_scene == SCENE_START) {
        al_stop_sample(&start_bgm_id);
        game_log("stop audio (bgm)");
    }else if (active_scene == SCENE_SETTINGS) {
        al_stop_sample(&setting_bgm_id);
        game_log("stop audio (bgm)");
    }else if (active_scene == SCENE_GAMEOVER) {
        al_stop_sample(&go_bgm_id);
        game_log("stop audio (bgm)");
    }else if (active_scene == SCENE_VICTORY) {
        al_stop_sample(&vic_bgm_id);
        game_log("stop audio (bgm)");
    }
    active_scene = next_scene;
    // TODO: Allocate resources before entering scene.
    if (active_scene == SCENE_MENU) {
        if (!al_play_sample(main_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
            game_abort("failed to play audio (bgm)");
    } else if (active_scene == SCENE_START) {
        int i;
        plane.img = start_img_plane;
        plane.x = 400;
        plane.y = 500;
        plane.w = plane.h = 51;
        for (i = 0; i < MAX_ENEMY; i++) {
            enemies[i].w = 28;
            enemies[i].h = 68;
            enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
            enemies[i].y = 80;
            enemies[i].img = start_img_enemy;
        }
        boss.img = img_boss;
        boss.x=SCREEN_W/2;
        boss.y=80;
        boss.hidden=true;
        // [HACKATHON 2-5-2]
        // TODO: Initialize bullets.
        // For each bullets in array, set their w and h to the size of
        // the image, and set their img to bullet image, hidden to true,
        // (vx, vy) to (0, -3).
        // Uncomment and fill in the code below.
        for (i = 0; i < MAX_BULLET; i++) {
            bullets[i].w = 28;
            bullets[i].h = 68;
            bullets[i].img = img_bullet;
            bullets[i].vx = 0;
            bullets[i].vy = -3;
            bullets[i].hidden = true;
        }
        if (!al_play_sample(start_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
            game_abort("failed to play audio (bgm)");
    }else if (active_scene == SCENE_SETTINGS) {
        if (!al_play_sample(setting_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &setting_bgm_id))
            game_abort("failed to play audio (bgm)");
    }else if (active_scene == SCENE_GAMEOVER) {
        if (!al_play_sample(go_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &go_bgm_id))
            game_abort("failed to play audio (bgm)");
    }else if (active_scene == SCENE_VICTORY) {
        if (!al_play_sample(vic_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &vic_bgm_id))
            game_abort("failed to play audio (bgm)");
    }
}

void on_key_down(int keycode) {
    if (active_scene == SCENE_GAMEOVER) {
        if (keycode == ALLEGRO_KEY_ESCAPE)
            game_change_scene(SCENE_MENU);
    }if (active_scene == SCENE_VICTORY) {
        if (keycode == ALLEGRO_KEY_ESCAPE)
            game_change_scene(SCENE_MENU);
    }if (active_scene == SCENE_MENU) {
        if (keycode == ALLEGRO_KEY_ENTER)
            game_change_scene(SCENE_START);
    }if(active_scene == SCENE_SETTINGS){
        if (keycode == ALLEGRO_KEY_BACKSPACE)
            game_change_scene(SCENE_MENU);
    }
}

void on_mouse_down(int btn, int x, int y) {
    // [HACKATHON 3-8]
    // TODO: When settings clicked, switch to settings scene.
    // Uncomment and fill in the code below.
    if (active_scene == SCENE_MENU) {
        if  (btn ==  1) {
            if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38))
                game_change_scene(SCENE_SETTINGS);
        }
    }
}

void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
    ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
    if (!loaded_bmp)
        game_abort("failed to load image: %s", filename);
    ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
    ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();
    
    if (!resized_bmp)
        game_abort("failed to create bitmap when creating resized image: %s", filename);
    al_set_target_bitmap(resized_bmp);
    al_draw_scaled_bitmap(loaded_bmp, 0, 0,
                          al_get_bitmap_width(loaded_bmp),
                          al_get_bitmap_height(loaded_bmp),
                          0, 0, w, h, 0);
    al_set_target_bitmap(prev_target);
    al_destroy_bitmap(loaded_bmp);
    
    game_log("resized image: %s", filename);
    
    return resized_bmp;
}

// [HACKATHON 3-3]
// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
// Uncomment and fill in the code below.
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
    if(px<w && py<h+w && py>h)
        return true;
    else
        return false;
}


// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
    fprintf(stderr, "error occured, exiting after 2 secs");
    // Wait 2 secs before exiting.
    al_rest(2);
    // Force exit program.
    exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
    va_list arg;
    va_start(arg, format);
    game_vlog(format, arg);
    va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
    static bool clear_file = true;
    vprintf(format, arg);
    printf("\n");
    // Write log to file for later debugging.
    FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
    if (pFile) {
        vfprintf(pFile, format, arg);
        fprintf(pFile, "\n");
        fclose(pFile);
    }
    clear_file = false;
#endif
}
