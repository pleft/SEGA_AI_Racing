#include <genesis.h>
#include "resource.h"

// Animation indices based on actual spritesheet
#define ANIM_DEFAULT   0  // Default driving
#define ANIM_RIGHT_1   1  // Turning right, 1st frame
#define ANIM_LEFT_1    2  // Turning left, 1st frame
#define ANIM_RIGHT_2   3  // Turning right, 2nd frame
#define ANIM_LEFT_2    4  // Turning left, 2nd frame

// Game state constants
#define STATE_PLAYING  0
#define STATE_GAMEOVER 1

// Car constants
#define PLAYER_MIN_SPEED FIX32(1)
#define PLAYER_MAX_SPEED FIX32(8)
#define PLAYER_ACCELERATION FIX32(0.1)
#define PLAYER_DECELERATION FIX32(0.05)
#define PLAYER_BRAKE FIX32(0.2)
#define PLAYER_STEERING FIX32(0.2)
#define PLAYER_STEERING_RETURN FIX32(0.1)
#define PLAYER_MAX_LATERAL_SPEED FIX32(3)

// Game variables
u8 gameState;
u16 score;
u16 frameCounter;

// Player variables
Sprite* playerCar;
fix32 playerSpeed;
fix32 playerPosX;
fix32 playerPosY;
fix32 playerLateralSpeed;
u16 turnAnimationTimer;  // For animating between turn frames

// Function prototypes
static void handleInput();
static void joyEventHandler(u16 joy, u16 changed, u16 state);
static void updateGame();
static void updateHUD();

int main()
{
    // Initialize the Sega Genesis
    VDP_setScreenWidth320();
    
    // Initialize joypad
    JOY_init();
    JOY_setEventHandler(&joyEventHandler);
    
    // Initialize sprite engine
    SPR_init();
    
    // Set palettes - assuming car sprite is using PAL0
    // When you create your car sprite, you may need to update this
    PAL_setPalette(PAL0, car.palette->data, DMA);
    
    // Initialize the player car
    playerCar = SPR_addSprite(&car, 160, 180, TILE_ATTR(PAL0, TRUE, FALSE, FALSE));
    
    // Initialize game variables
    gameState = STATE_PLAYING;
    score = 0;
    frameCounter = 0;
    
    // Initialize player variables
    playerSpeed = PLAYER_MIN_SPEED;
    playerPosX = FIX32(160);
    playerPosY = FIX32(180);
    playerLateralSpeed = FIX32(0);
    turnAnimationTimer = 0;
    
    // Draw initial HUD
    updateHUD();
    
    // Main game loop
    while(1)
    {
        // Handle input
        handleInput();
        
        // Update game logic
        if(gameState == STATE_PLAYING) {
            updateGame();
        }
        
        // Update sprites
        SPR_update();
        
        // Wait for VSync
        SYS_doVBlankProcess();
        
        // Update frame counter
        frameCounter++;
    }
    
    return 0;
}

// Handle input based on game state
static void handleInput()
{
    u16 joy = JOY_readJoypad(JOY_1);
    
    if(gameState == STATE_PLAYING) {
        // Accelerate/brake
        if(joy & BUTTON_B) {
            playerSpeed = playerSpeed + PLAYER_ACCELERATION;
            if(playerSpeed > PLAYER_MAX_SPEED) playerSpeed = PLAYER_MAX_SPEED;
        } else if(joy & BUTTON_A) {
            playerSpeed = playerSpeed - PLAYER_BRAKE;
            if(playerSpeed < PLAYER_MIN_SPEED) playerSpeed = PLAYER_MIN_SPEED;
            
            // For braking, we can still use the default animation
            // since your description doesn't mention a brake animation
            SPR_setAnim(playerCar, ANIM_DEFAULT);
        } else {
            // Natural deceleration
            playerSpeed = playerSpeed - PLAYER_DECELERATION;
            if(playerSpeed < PLAYER_MIN_SPEED) playerSpeed = PLAYER_MIN_SPEED;
        }
        
        // Steering
        if(joy & BUTTON_RIGHT) {
            playerLateralSpeed = playerLateralSpeed + PLAYER_STEERING;
            if(playerLateralSpeed > PLAYER_MAX_LATERAL_SPEED) {
                playerLateralSpeed = PLAYER_MAX_LATERAL_SPEED;
            }
            
            // Alternate between first and second frame of right turn animation
            if (turnAnimationTimer < 15) {
                SPR_setAnim(playerCar, ANIM_RIGHT_1);
            } else {
                SPR_setAnim(playerCar, ANIM_RIGHT_2);
            }
        } else if(joy & BUTTON_LEFT) {
            playerLateralSpeed = playerLateralSpeed - PLAYER_STEERING;
            if(playerLateralSpeed < -PLAYER_MAX_LATERAL_SPEED) {
                playerLateralSpeed = -PLAYER_MAX_LATERAL_SPEED;
            }
            
            // Alternate between first and second frame of left turn animation
            if (turnAnimationTimer < 15) {
                SPR_setAnim(playerCar, ANIM_LEFT_1);
            } else {
                SPR_setAnim(playerCar, ANIM_LEFT_2);
            }
        } else {
            // Return to straight when no steering input
            if(playerLateralSpeed > FIX32(0)) {
                playerLateralSpeed = playerLateralSpeed - PLAYER_STEERING_RETURN;
                if(playerLateralSpeed < FIX32(0)) playerLateralSpeed = FIX32(0);
            } else if(playerLateralSpeed < FIX32(0)) {
                playerLateralSpeed = playerLateralSpeed + PLAYER_STEERING_RETURN;
                if(playerLateralSpeed > FIX32(0)) playerLateralSpeed = FIX32(0);
            }
            
            // Set standard driving animation
            SPR_setAnim(playerCar, ANIM_DEFAULT);
        }
        
        // Update turn animation timer (cycles between 0-29)
        turnAnimationTimer = (turnAnimationTimer + 1) % 30;
    }
    else if(gameState == STATE_GAMEOVER) {
        // Press start to restart
        if(joy & BUTTON_START) {
            gameState = STATE_PLAYING;
            score = 0;
            playerSpeed = PLAYER_MIN_SPEED;
            playerPosX = FIX32(160);
            playerLateralSpeed = FIX32(0);
            updateHUD();
        }
    }
}

static void joyEventHandler(u16 joy, u16 changed, u16 state)
{
    // Can be used for immediate button responses
    if(joy == JOY_1) {
        // Check for start button to pause/unpause
        if((changed & BUTTON_START) && (state & BUTTON_START)) {
            if(gameState == STATE_PLAYING) {
                // Toggle pause state or similar function
            }
        }
    }
}

// Update game logic
static void updateGame()
{
    // Apply lateral speed to position
    playerPosX = playerPosX + playerLateralSpeed;
    
    // Clamp player position to screen boundaries
    // Assuming the car sprite is 64 pixels wide
    // Left boundary: allow the car to move further left, but keep it mostly visible
    if (fix32ToInt(playerPosX) < 0) {  // Reduced from 40
        playerPosX = FIX32(0);
        playerLateralSpeed = FIX32(0);
    } 
    // Right boundary: prevent the car from going off the right side of the screen
    // Since the sprite is anchored at the top-left, we need to account for its width
    else if (fix32ToInt(playerPosX) >256) {  // Changed from 280 to 320 - 60
        playerPosX = FIX32(256);
        playerLateralSpeed = FIX32(0);
    }
    // Update player sprite position
    SPR_setPosition(playerCar, fix32ToInt(playerPosX), fix32ToInt(playerPosY));
    
    // Update score
    score += fix32ToInt(playerSpeed);
    
    // Update HUD (score, speed, etc.)
    updateHUD();
}

// Update the heads-up display
static void updateHUD()
{
    // Clear the top rows for HUD
    VDP_clearTextLine(0);
    VDP_clearTextLine(1);
    
    // Display score
    char scoreStr[16];
    sprintf(scoreStr, "SCORE: %d", score);
    VDP_drawText(scoreStr, 1, 0);
    
    // Display speed as percentage
    char speedStr[16];
    u8 speedPercent = (fix32ToInt(playerSpeed) * 100) / fix32ToInt(PLAYER_MAX_SPEED);
    sprintf(speedStr, "SPEED: %d%%", speedPercent);
    VDP_drawText(speedStr, 25, 0);
}