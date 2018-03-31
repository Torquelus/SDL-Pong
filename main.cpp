#include <iostream>
#include <string>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

using namespace std;

//Define Simplifying Functions
int randBetween(int minimum, int maximum){          //Generate Random Number Between Two Numbers
    return rand() % (maximum - minimum + 1) + minimum;
}

//Define Constant Variables
const float MULTIPLIER = 4;
const int SCREEN_WIDTH = 160 * MULTIPLIER;
const int SCREEN_HEIGHT = 120 * MULTIPLIER;
const int RECTANGLE_Y = SCREEN_HEIGHT / 8;
const int SCREEN_FRAMERATE = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FRAMERATE;
const float PADDLE_SPEED = 1.0 * MULTIPLIER;
const float BALL_SPEED_INITIAL = 0.3 * MULTIPLIER;
const float BALL_SPEED_INCREASE = 1.05;
const float BALL_SPEED_MAX = 2.0 * MULTIPLIER;

//Time Variables
uint32_t currentTick;
uint32_t lastTick;
uint32_t deltaTime;

//Game Flag
bool gameState = false;
bool gameWin = false;
float BALL_SPEED = BALL_SPEED_INITIAL;

//Key Press Buttons
const int RPADDLE_UP = SDL_SCANCODE_UP;
const int RPADDLE_DOWN = SDL_SCANCODE_DOWN;
const int LPADDLE_UP = SDL_SCANCODE_W;
const int LPADDLE_DOWN = SDL_SCANCODE_S;
const int START_GAME1 = SDL_SCANCODE_RETURN;
const int START_GAME2 = SDL_SCANCODE_SPACE;
const int RESET_GAME = SDL_SCANCODE_R;
const int EXIT_GAME = SDL_SCANCODE_ESCAPE;

//Sound Effects
Mix_Chunk *ballCollideSound = NULL;
Mix_Chunk *paddleCollideSound = NULL;
Mix_Chunk *scoreSound = NULL;
Mix_Chunk *resetSound = NULL;

//Define Screen and Font
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *scoreFont = NULL;
TTF_Font *logoFont = NULL;

//Define Classes
class Paddle{                                       //Paddle Class
    public:
        SDL_Texture *texture;       //Image of Paddle
        SDL_Rect rect;              //Rectangle of Paddle
        int w, h;                   //Width and Height
        float x, y;                 //Coordinates

        Paddle(){                           //Constructor
            score = 0;
        }
        ~Paddle(){}                          //Destructor
        void setPos(int init_x, int init_y){//Set Initial Position
            x = init_x;
            y = init_y;
        }
        void scoreUp(){                     //Increment Score By 1
            score += 1;
            Mix_PlayChannel(-1, scoreSound, 0);
        }
        void resetScore(){                  //Reset Score to 0
            score = 0;
        }
        void moveUp(){                      //Move Paddle Up
            if(y > PADDLE_SPEED + RECTANGLE_Y){
                y -= PADDLE_SPEED;
            }
            else{
                y = RECTANGLE_Y;
            }
        }
        void moveDown(){                    //Move Paddle Down
            if(y < SCREEN_HEIGHT - PADDLE_SPEED - h){
                y += PADDLE_SPEED;
            }
            else{
                y = SCREEN_HEIGHT - h;
            }
        }
        int returnScore(){                  //Return Score of Paddle
            return score;
        }
        void multiply(){                    //Resize Texture
            w *= MULTIPLIER;
            h *= MULTIPLIER;
        }
        void updateRect(){                  //Update Rectangle Position
            rect.w = w;
            rect.h = h;
            rect.x = x;
            rect.y = y;
        }
    private:
        int score;                  //Score
};
class Ball{                                         //Class of the Ball
    public:
        SDL_Texture *texture;       //Image of Ball
        SDL_Rect rect;              //Rectangle of Ball
        int w, h;                   //Width and Height
        float x, y;                 //Coordinates

        Ball(int init_x = 1, int init_y = 1){//Constructor With Initial Direction
            dirX = init_x;
            dirY = init_y;
            moving = false;
            out_of_bounds = false;
            hit_paddle = false;
        }
        ~Ball(){}                           //Destructor
        void reset(){                       //Reset Ball Position
            x = SCREEN_WIDTH / 2 - w / 2;
            y = randBetween(RECTANGLE_Y + 1, SCREEN_HEIGHT - h - 1);
            moving = false;
        }
        void multiply(){                    //Resize Texture
            w *= MULTIPLIER;
            h *= MULTIPLIER;
        }
        void randDirection(){               //Set Direction of Ball as Random
            //Set Random X Direction
            if(rand() % 2 == 0){
                dirX = -1;
            }
            else{
                dirX = 1;
            }
            //Set Random Y Direction
            if(rand() % 2 == 0){
                dirY = -1;
            }
            else{
                dirY = 1;
            }
        }
        void flipX(){                       //Flip Direction on X
            dirX *= -1;
        }
        void flipY(){                       //Flip Direction on Y
            dirY *= -1;
        }
        int returnDirX(){                   //Return dirX
            return dirX;
        }
        void movingOn(){                    //Start Moving Ball
            moving = true;
        }
        void movingOff(){                   //Stop Moving Ball
            moving = false;
        }
        void moveBall(){                    //Move the Ball
            if(moving){
                x += BALL_SPEED * dirX;
                y += BALL_SPEED * dirY;
            }
        }
        void updateRect(){                  //Update Rectangle Position
            rect.w = w;
            rect.h = h;
            rect.x = x;
            rect.y = y;
        }
    private:
        int dirX;                   //Direction of Ball's Movement in X
        int dirY;                   //Direction of Ball's Movement in Y
        bool moving;                //Is the Ball Moving?
        bool out_of_bounds;         //Is the Ball Out of Bounds?
        bool hit_paddle;            //Did the Ball Hit the Paddle?
};
class Score{                                        //Scores and Texts
    public:
        //Variables
        SDL_Texture *text;          //Texture of Text
        SDL_Rect rect;              //Rectangle of Ball
        SDL_Colour textColour = {134, 122, 228};
        int w, h;                   //Width and Height
        float x, y;                 //Coordinates

        Score(){}                           //Constructor
        ~Score(){};                         //Destructor

        void setPos(int init_x, int init_y){//Set Initial Position
            x = init_x;
            y = init_y;
        }
        void updateRect(){                  //Update Rectangle Position
            rect.w = w;
            rect.h = h;
            rect.x = x;
            rect.y = y;
        }
        void updateText(string score, TTF_Font *font){      //Update the Texture
            //Create Surface from Text
            SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(font, score.c_str(), textColour, 400 );
            if(textSurface == NULL){
                cerr << "Unable to Create Text Surface! SDL_ttf Error: " << TTF_GetError() << endl;
            }
            //Create Texture from Surface
            text = SDL_CreateTextureFromSurface(renderer, textSurface);
            if(text == NULL){
                cerr << "Unable to Create Texture from textSurface! SDL_ttf Error: " << TTF_GetError() << endl;
            }

            //Define Width and Height
            w = textSurface->w;
            h = textSurface->h;

            //Free Surface
            SDL_FreeSurface(textSurface);
        }
};

//Define UI Elements
SDL_Rect rectangle = {0, RECTANGLE_Y - 2 * MULTIPLIER, SCREEN_WIDTH, 2 * MULTIPLIER};
Score lScore;
Score rScore;
Score logo;
Score winText;

//Define Paddles and Ball
Paddle lPaddle;
Paddle rPaddle;
Ball pongBall;

//Define Functions
bool initSDL(){                                     //Initialize SDL
    //Success Flag
    bool success = true;

    //New Random Seed
    srand(time(NULL));

    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0){
        cerr << "SDL failed to initialize! SDL_Error: " << SDL_GetError() << endl;
        success = false;
    }

    //Initialize TTF
    if(TTF_Init() != 0){
        cerr << "TTF failed to initialize! TTF Error: " << TTF_GetError() << endl;
        success = false;
    }

    //Initialize PNG Loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)){
        cerr << "SDL_image failed to initialize! SDL_image Error: " << IMG_GetError() << endl;
        success = false;
    }

    //Initialize SDL_mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0){
        cerr << "SDL_mixer failed to initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        success = false;
    }

    //Return Success Flag
    return success;
}
SDL_Texture *loadTexture(string path){              //Load an Image into a Texture
    //The Final Texture
    SDL_Texture *newTexture;

    //Load Image at Path
    SDL_Surface *loadedImage = IMG_Load(path.c_str());
    if(loadedImage == NULL){
        cout << "Failed to Load Image " << path.c_str() << ". SDL_image Error: " << IMG_GetError() << endl;
        return NULL;
    }

    //Create New Texture from Surface
    newTexture = SDL_CreateTextureFromSurface(renderer, loadedImage);
    if(newTexture == NULL){
        cout << "Failed to Load Texture " << path.c_str() << ". SDL Error: " << SDL_GetError() << endl;
        return NULL;
    }
    //Get Rid of Surface
    SDL_FreeSurface(loadedImage);

    return newTexture;
}
void blitImage(SDL_Rect rect, SDL_Texture *texture, //Blit Image
               int x,int y, int w, int h){
    //Blitting
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}
bool initPong(){                                    //Initialize the Program
    //Success Flag
    bool success = true;

    //Create Score Font
    scoreFont = TTF_OpenFont("fonts/C64.ttf", 8 * MULTIPLIER);
    if(scoreFont == NULL){
        cerr << "Font could not be opened! SDL_Error: " << SDL_GetError() << endl;
        success = false;
    }

    //Create Logo Font
    logoFont = TTF_OpenFont("fonts/C64.ttf", 12 * MULTIPLIER);
    if(logoFont == NULL){
        cerr << "Font could not be opened! SDL_Error: " << TTF_GetError() << endl;
        success = false;
    }

    //Create Window
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL){
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        success = false;
        return success;
    }

    //Create Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        success = false;
        return success;
    }

    //Load Sound Effects
    ballCollideSound = Mix_LoadWAV("sounds/ball_collide.wav");
    if(ballCollideSound == NULL){
        cerr << "ballCollideSound could not be loaded! Mix_Error: " << Mix_GetError() << endl;
        success = false;
        return success;
    }
    paddleCollideSound = Mix_LoadWAV("sounds/paddle_collide.wav");
    if(paddleCollideSound == NULL){
        cerr << "paddleCollideSound could not be loaded! Mix_Error: " << Mix_GetError() << endl;
        success = false;
        return success;
    }
    scoreSound = Mix_LoadWAV("sounds/score.wav");
    if(scoreSound == NULL){
        cerr << "scoreSound could not be loaded! Mix_Error: " << Mix_GetError() << endl;
        success = false;
        return success;
    }
    resetSound = Mix_LoadWAV("sounds/reset.wav");
    if(resetSound == NULL){
        cerr << "resetSound could not be loaded! Mix_Error: " << Mix_GetError() << endl;
        success = false;
        return success;
    }

    //Update Window
    SDL_UpdateWindowSurface(window);

    //Apply Graphics to Paddle and Ball
    pongBall.texture = loadTexture("images/Ball.png");
    lPaddle.texture = loadTexture("images/Paddle.png");
    rPaddle.texture = loadTexture("images/Paddle.png");

    //Set Texture Width and Height
    SDL_QueryTexture(pongBall.texture, NULL, NULL, &pongBall.w, &pongBall.h);
    SDL_QueryTexture(lPaddle.texture, NULL, NULL, &lPaddle.w, &lPaddle.h);
    SDL_QueryTexture(rPaddle.texture, NULL, NULL, &rPaddle.w, &rPaddle.h);
    pongBall.multiply();
    lPaddle.multiply();
    rPaddle.multiply();

    //Set Initial Score & Logo
    lScore.updateText("0", scoreFont);
    rScore.updateText("0", scoreFont);
    logo.updateText("PONG", logoFont);

    //Set Initial Positions
    pongBall.reset();
    lPaddle.setPos(0, SCREEN_HEIGHT / 2 - lPaddle.h / 2);
    rPaddle.setPos(SCREEN_WIDTH - rPaddle.w, SCREEN_HEIGHT / 2 - rPaddle.h / 2);
    logo.setPos(SCREEN_WIDTH / 2 - logo.w / 2, RECTANGLE_Y / 2 - logo.h / 2);
    logo.updateRect();

    //Start Timer at 0
    lastTick = 0;
    deltaTime = 0;

    //Return Success Flag
    return success;
}
void updateScreen(){                                //Update Screen
    //Clear Renderer
    SDL_RenderClear(renderer);

    //Update Right Score
    rScore.setPos(SCREEN_WIDTH - lScore.w - 2 * MULTIPLIER, RECTANGLE_Y / 2 - rScore.h / 2);
    rScore.updateRect();
    rScore.updateText(to_string(rPaddle.returnScore()), scoreFont);

    //Update Left Score
    lScore.setPos(2 * MULTIPLIER, RECTANGLE_Y / 2 - lScore.h / 2);
    lScore.updateRect();
    lScore.updateText(to_string(lPaddle.returnScore()), scoreFont);

    //Blit Scores and Logo
    blitImage(rScore.rect, rScore.text, rScore.x, rScore.y, rScore.w, rScore.h);
    blitImage(lScore.rect, lScore.text, lScore.x, lScore.y, lScore.w, lScore.h);
    blitImage(logo.rect, logo.text, logo.x, logo.y, logo.w, logo.h);

    //Blit Win Text
    if(gameWin == 1){
        blitImage(winText.rect, winText.text, winText.x, winText.y, winText.w, winText.h);
    }

    //Update Positions
    if(gameWin == 0){
        pongBall.updateRect();
        lPaddle.updateRect();
        rPaddle.updateRect();
        blitImage(pongBall.rect, pongBall.texture, pongBall.x, pongBall.y, pongBall.w, pongBall.h);
        blitImage(lPaddle.rect, lPaddle.texture, lPaddle.x, lPaddle.y, lPaddle.w, lPaddle.h);
        blitImage(rPaddle.rect, rPaddle.texture, rPaddle.x, rPaddle.y, rPaddle.w, rPaddle.h);
    }

    //Create UI
    SDL_SetRenderDrawColor(renderer, 134, 122, 228, 255);
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_RenderPresent(renderer);

    //Update Screen
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xAA, 0x00);
    SDL_RenderPresent(renderer);

    //Check Timer
    currentTick = SDL_GetTicks();
    deltaTime = currentTick - lastTick;
    lastTick = currentTick;

    //Limit Framerate
    if(deltaTime < SCREEN_TICKS_PER_FRAME){
        SDL_Delay(SCREEN_TICKS_PER_FRAME - deltaTime);
    }
}
bool rectCollision(SDL_Rect A, SDL_Rect B){         //Check Rectangle Collision
    //Collision Flag
    bool collides = true;

    //Rectangle Sides
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate Sides of A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //Calculate Sides of B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //Check Collision
    if(bottomA <= topB){
        collides = false;
    }
    if(topA >= bottomB){
        collides = false;
    }
    if(rightA <= leftB){
        collides = false;
    }
    if(leftA >= rightB){
        collides = false;
    }

    return collides;
}
void checkCollisions(){                             //Check Collisions
    if(pongBall.y <= BALL_SPEED + RECTANGLE_Y|| pongBall.y >= SCREEN_HEIGHT - pongBall.h - BALL_SPEED){
        pongBall.flipY();
        Mix_PlayChannel(-1, ballCollideSound, 0);
    }
    if((rectCollision(pongBall.rect, rPaddle.rect) && (pongBall.returnDirX() == 1)) || (rectCollision(pongBall.rect, lPaddle.rect) && (pongBall.returnDirX() == -1))){
        pongBall.flipX();
        Mix_PlayChannel(-1, paddleCollideSound, 0);
        if(BALL_SPEED < BALL_SPEED_MAX){    //Increment Ball Speed
            BALL_SPEED *= BALL_SPEED_INCREASE;
        }
    }
}
void checkScore(){                                  //Check If Ball is Scored
    if(pongBall.x <= 0){
        rPaddle.scoreUp();
        pongBall.reset();
        gameState = false;
    }
    if(pongBall.x >= SCREEN_WIDTH){
        lPaddle.scoreUp();
        pongBall.reset();
        gameState = false;
    }
}
void checkWin(){                                    //Check If Anyone Has Won (At 7 Points)
    if(lPaddle.returnScore() == 7){
        gameWin = true;
        //Create Win Text at Center of Screen
        winText.updateText("Player 1 Wins!", logoFont);
        winText.setPos(SCREEN_WIDTH/2 - winText.w/2, SCREEN_HEIGHT/2 - winText.h/2);
        winText.updateRect();
    }
    if(rPaddle.returnScore() == 7){
        gameWin = true;
        //Create Win Text at Center of Screen
        winText.updateText("Player 2 Wins!", logoFont);
        winText.setPos(SCREEN_WIDTH/2 - winText.w/2, SCREEN_HEIGHT/2 - winText.h/2);
        winText.updateRect();
    }
}
void startGame(){                                   //Start the Game
    gameState = true;
    pongBall.randDirection();
    pongBall.movingOn();
    BALL_SPEED = BALL_SPEED_INITIAL;
}
void resetGame(){                                   //Reset the Game
    gameState = false;
    gameWin = false;
    pongBall.movingOff();
    pongBall.reset();
    lPaddle.resetScore();
    rPaddle.resetScore();
    Mix_PlayChannel(-1, resetSound, 0);
}
void gameLoop(){                                    //Main Game Loop
    //Main Loop Flag
    bool quit = false;

    //Event Handler
    SDL_Event e;

    //While Application is Running
    while(!quit){
        //Read Keyboard Input
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        //Handle Events on Queue
        while(SDL_PollEvent(&e) != 0){
            //User Requests to Quit
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }

        //Key Press
        if(currentKeyStates[RPADDLE_UP]){                                                   // Paddle Up
            rPaddle.moveUp();
        }
        if(currentKeyStates[RPADDLE_DOWN]){                                                 //Right Paddle Down
            rPaddle.moveDown();
        }
        if(currentKeyStates[LPADDLE_UP]){                                                   //Left Paddle Up
            lPaddle.moveUp();
        }
        if(currentKeyStates[LPADDLE_DOWN]){                                                 //Left Paddle Down
            lPaddle.moveDown();
        }
        if((currentKeyStates[START_GAME1] || currentKeyStates[START_GAME2]) && !gameState && !gameWin){ //Start Game
            startGame();
        }
        if(currentKeyStates[RESET_GAME] && (gameState || gameWin)){                         //Reset Game
            resetGame();
        }
        if(currentKeyStates[EXIT_GAME]){                                                    //Quit
            quit = true;
        }

        pongBall.moveBall();
        checkScore();
        checkWin();
        checkCollisions();

        updateScreen();
    }
}
void close(){                                       //Close All Systems
    //Destroy Window
    SDL_DestroyWindow(window);
    window = NULL;

    //Destroy Renderer
    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    //Free Sounds
    Mix_FreeChunk(ballCollideSound);
    Mix_FreeChunk(paddleCollideSound);
    Mix_FreeChunk(scoreSound);
    Mix_FreeChunk(resetSound);
    ballCollideSound = NULL;
    paddleCollideSound = NULL;
    scoreSound = NULL;
    resetSound = NULL;

    //Free Graphics
    SDL_DestroyTexture(pongBall.texture);
    SDL_DestroyTexture(lPaddle.texture);
    SDL_DestroyTexture(rPaddle.texture);
    SDL_DestroyTexture(logo.text);
    SDL_DestroyTexture(lScore.text);
    SDL_DestroyTexture(rScore.text);
    SDL_DestroyTexture(winText.text);
    pongBall.texture = NULL;
    lPaddle.texture = NULL;
    rPaddle.texture = NULL;
    logo.text = NULL;
    lScore.text = NULL;
    rScore.text = NULL;
    winText.text = NULL;

    //Quit SDL
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}

int main(int argc, char* args[]){                   //Main Code

    if(!initSDL()){                 //Check SDL Initialization
        SDL_Quit();
        return 1;
    }

    if(!initPong()){                //Check Pong Initialization
        SDL_Quit();
        return 1;
    }

    gameLoop();

    close();                        //Cleanup and Exit
    return 0;
}
