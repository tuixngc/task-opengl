#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include "GameLevel.h"
#include "BallObject.h"
#include  <tuple>
#include <irrKlang.h>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSE
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

class Game
{
public:
    GameState              State;
    GLboolean              Keys[1024];
    GLuint                 Width, Height;
    std::vector<GameLevel> Levels;
    unsigned int           Level;
    Game(GLuint width, GLuint height);
    ~Game();
    void Init();
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions();
private:
    void ResetLevel();
    void ResetPlayer();
};

#endif