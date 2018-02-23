#include <iostream>
#include <string>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

//Define Simplifying Functions
int randBetween(int minimum, int maximum){          //Generate Random Number Between Two Numbers
    return rand() % (maximum - minimum + 1) + minimum;
}

//Define Constant Variables
const int MULTIPLIER = 8;
const int SCREEN_WIDTH = 160 * MULTIPLIER;
const int SCREEN_HEIGHT = 120 * MULTIPLIER;

//Key Press Constants
enum KeyPresses{
    KEY_PRESS_UP
};

//Define Classes
class Paddle{                                       //Paddle Class
    public:
        SDL_Texture *texture;       //Image of Paddle
        int w, h;                   //Width and Height
        int x, y;                   //Coordinates

        Paddle(){                           //Constructor
            score = 0;
        }
        void setPos(int init_x, int init_y){//Set Initial Position
            x = init_x;
            y = init_y;
        }
        void scoreUp(){                     //Increment Score By 1
            score += 1;
        }
        void resetScore(){                  //Reset Score to 0
            score = 0;
        }
        int returnScore(){                  //Return Score of Paddle
            return score;
        }
        void multiply(){                    //Resize Texture
            w *= MULTIPLIER;
            h *= MULTIPLIER;
        }
    private:
        int score;                  //Score
};
class Ball{                                         //Class of the Ball
    public:
        SDL_Texture *texture;       //Image of Ball
        int w, h;                   //Width and Height
        int x, y;                   //Coordinates

        Ball(int init_dir = 0){             //Constructor With Initial Direction
            dir = init_dir;
            moving = false;
            out_of_bounds = false;
            hit_paddle = false;
        }
        void reset(){                       //Reset Ball Position
            x = SCREEN_WIDTH / 2 - w / 2;
            y = randBetween(0, SCREEN_HEIGHT - h);
            moving = false;
        }
        void multiply(){                    //Resize Texture
            w *= MULTIPLIER;
            h *= MULTIPLIER;
        }
    private:
        int dir;                    //Direction of Ball's Movement
        bool moving;                //Is the Ball Moving?
        bool out_of_bounds;         //Is the Ball Out of Bounds?
        bool hit_paddle;            //Did the Ball Hit the Paddle?
};

//Define Screen and Font
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *scoreFont = NULL;

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
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        cout << "SDL failed to initialize! SDL_Error: " << SDL_GetError() << endl;
        success = false;
    }

    //Initialize TTF
    if(TTF_Init() != 0){
        cout << "TTF failed to initialize! SDL_Error: " << SDL_GetError() << endl;
        success = false;
    }

    //Initialize PNG Loading
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)){
        cout << "SDL_image failed to initialize! SDL_Error: " << SDL_GetError() << endl;
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
bool initPong(){                                    //Initialize the Program
    //Success Flag
    bool success = true;

    //Create Font
    scoreFont = TTF_OpenFont("fonts/C64.ttf", 32);
    if(scoreFont == NULL){
        cout << "Font could not be opened! SDL_Error: " << SDL_GetError() << endl;
        success = false;
    }

    //Create Window
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == NULL){
        cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        success = false;
        return success;
    }
    //Create Renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL){
        cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        success = false;
        return success;
    }

    //Initialize Renderer Colour
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xAA, 0x00);

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

    //Set Initial Positions
    pongBall.reset();
    lPaddle.setPos(0, SCREEN_HEIGHT / 2 - lPaddle.h / 2);
    rPaddle.setPos(SCREEN_WIDTH - rPaddle.w, SCREEN_HEIGHT / 2 - rPaddle.h / 2);

    //Return Success Flag
    return success;
}
void blitImage(SDL_Texture *texture, int x,         //Blit Image
               int y, int w, int h){
    //Rectangle to Blit Onto
    SDL_Rect destR;

    //Rectangle Coordinates
    destR.x = x;
    destR.y = y;
    destR.w = w;
    destR.h = h;

    //Blitting
    SDL_RenderCopy(renderer, texture, NULL, &destR);
}
void updateScreen(){                                //Update Screen
    //Clear Renderer
    SDL_RenderClear(renderer);

    //Update Positions
    blitImage(pongBall.texture, pongBall.x, pongBall.y, pongBall.w, pongBall.h);
    blitImage(lPaddle.texture, lPaddle.x, lPaddle.y, lPaddle.w, lPaddle.h);
    blitImage(rPaddle.texture, rPaddle.x, rPaddle.y, rPaddle.w, rPaddle.h);

    //Update Screen
    SDL_RenderPresent(renderer);
}
void gameLoop(){                                    //Main Game Loop
    //Main Loop Flag
    bool quit = false;

    //Event Handler
    SDL_Event e;

    //While Application is Running
    while(!quit){
        //Handle Events on Queue
        while(SDL_PollEvent(&e) != 0){
            //User Requests to Quit
            if(e.type == SDL_QUIT){
                quit = true;
            }
        }

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

    //Free Graphics
    SDL_DestroyTexture(pongBall.texture);
    SDL_DestroyTexture(lPaddle.texture);
    SDL_DestroyTexture(rPaddle.texture);
    pongBall.texture = NULL;
    lPaddle.texture = NULL;
    rPaddle.texture = NULL;

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
