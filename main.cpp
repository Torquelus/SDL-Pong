#include <iostream>
#include <string>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;

//Define Constant Variables
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Define Classes
class Paddle{                                       //Paddle Class
    public:
        Paddle(int init_x = 0,
               int init_y = 0){             //Constructor With Initial Position
            x = init_x;
            y = init_y;
            score = 0;
        }
    private:
        SDL_Surface *surface;       //Image of Paddle
        int x, y;                   //Coordinates
        int score;                  //Score
};
class Ball{                                         //Class of the Ball
    public:
        Ball(int init_x = 0, int init_y = 0,
             int init_dir = 0){             //Constructor With Initial Position and Direction
            x = init_x;
            y = init_y;
            dir = init_dir;
        }
    private:
        SDL_Surface *surface;       //Image of Ball
        int x, y;                   //Coordinates
        int dir;                    //Direction of Ball's Movement
        bool out_of_bounds;         //Is the Ball Out of Bounds?
        bool hit_paddle;            //Did the Ball Hit the Paddle?
};

//Define Screen and Font
SDL_Window *window = NULL;
SDL_Surface *screen = NULL;
TTF_Font *scoreFont = NULL;

//Define Paddles and Ball
Paddle lPaddle, rPaddle;
Ball pongBall;

//Define Simplifying Functions
int randBetween(int minimum, int maximum){          //Generate Random Number Between Two Numbers
    return rand() % (maximum - minimum + 1) + minimum;
}

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

    //Return Success Flag
    return success;
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
    }
    else{
        //Get Window Surface
        screen = SDL_GetWindowSurface(window);
        //Fill Screen Blue & Update
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x00, 0x00, 0xAA));
        SDL_UpdateWindowSurface(window);
    }

    //Return Success Flag
    return success;
}
void close(){                                       //Close All Systems
    //Destroy Window
    SDL_DestroyWindow(window);
    window = NULL;

    //Deallocate Screen
    SDL_FreeSurface(screen);
    screen = NULL;

    //Quit SDL
    SDL_Quit();
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

    SDL_Delay(2000);                //Test

    close();                        //Cleanup and Exit
    return 0;
}
