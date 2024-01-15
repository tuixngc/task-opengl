#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"


class GameLevel
{
public:
    std::vector<GameObject> Bricks;

    GameLevel() { }

    void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

    void Draw(SpriteRenderer& renderer);
    
    bool IsCompleted();

    void ClearBricks();

private:
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);

    std::vector<std::vector<unsigned int>> TileData;
};

#endif