#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include "GameObject.h"
#include <irrKlang.h>

typedef void (*callback)(irrklang::ISoundSource*);

class BallObject : public GameObject
{
public:	
    float     Radius;
    bool      Stuck;

    BallObject();
    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D sprite);

    glm::vec2 Move(float dt, unsigned int window_width, callback p, irrklang::ISoundSource* source);
    void      Reset(glm::vec2 position, glm::vec2 velocity);
};

#endif