#include "GameLevel.h"

#include <fstream>
#include <sstream>

glm::vec3 LAYER1_COLOR = glm::vec3(0.2f, 0.6f, 1.0f);
glm::vec3 LAYER2_COLOR = glm::vec3(0.0f, 0.7f, 0.0f);
glm::vec3 LAYER3_COLOR = glm::vec3(0.8f, 0.8f, 0.4f);
glm::vec3 LAYER4_COLOR = glm::vec3(1.0f, 0.5f, 0.0f);

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
    this->Bricks.clear();

    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) 
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) 
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }

    TileData = tileData;
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
    for (GameObject& tile : this->Bricks)
        if (!tile.Destroyed)
        {
            glm::vec3 color = glm::vec3(1.0f);
            if (tile.Layer == 2)
                color = LAYER1_COLOR;
            else if (tile.Layer == 3)
                color = LAYER2_COLOR;
            else if (tile.Layer == 4)
                color = LAYER3_COLOR;
            else if (tile.Layer == 5)
                color = LAYER4_COLOR;

            tile.Color = color;

            tile.Draw(renderer);
        }
            
}

bool GameLevel::IsCompleted()
{
    for (GameObject& tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::ClearBricks()
{
    for (GameObject& tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            tile.Destroyed = true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();
    float unit_width = levelWidth / static_cast<float>(width);
    float unit_height = levelHeight / height;
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if (tileData[y][x] == 1) 
            {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj.IsSolid = true;
                this->Bricks.push_back(obj);
            }
            else if (tileData[y][x] > 1)
            {
                glm::vec3 color = glm::vec3(1.0f);
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                this->Bricks.push_back(GameObject(pos, size, ResourceManager::GetTexture("block"), color, glm::vec2(0.0f, 0.0f), tileData[y][x]));
            }
        }
    }
}