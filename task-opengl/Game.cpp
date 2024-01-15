#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"

SpriteRenderer* Renderer;
irrklang::ISoundEngine* se = irrklang::createIrrKlangDevice();
irrklang::ISoundSource* SFX1;
irrklang::ISoundSource* SFX2;
irrklang::ISoundSource* SFX3;

typedef std::tuple<bool, Direction, glm::vec2> Collision;
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);
GameObject* Player;
glm::vec2 playerPos;

const glm::vec2 BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;
BallObject* Ball;
glm::vec2 ballPos;

typedef std::tuple<bool, Direction, glm::vec2> Collision;
Direction VectorDirection(glm::vec2 target);
Collision CheckCollision(BallObject& one, GameObject& two);
void StopAllSFX();
void PlaySoundFX(irrklang::ISoundSource*);

Game::Game(GLuint width, GLuint height)
    : State(GAME_MENU), Keys(), Width(width), Height(height)
{

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{
    ResourceManager::LoadShader("VertexShader.glsl", "FragmentShader.frag", "sprite");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

    ResourceManager::LoadTexture("background.png", true, "background");
    ResourceManager::LoadTexture("Ball.png", true, "ball");
    ResourceManager::LoadTexture("block.png", true, "block");
    ResourceManager::LoadTexture("block_solid.png", true, "block_solid");
    ResourceManager::LoadTexture("paddle.png", true, "paddle");
    ResourceManager::LoadTexture("MainScreen.png", true, "mainscreen");
    ResourceManager::LoadTexture("WinScreen.png", true, "winscreen");
    ResourceManager::LoadTexture("LoseScreen.png", true, "losescreen");
    
    GameLevel one; one.Load("level_1.txt", this->Width, this->Height / 2);
    GameLevel two; two.Load("level_2.txt", this->Width, this->Height / 2);
    GameLevel three; three.Load("level_3.txt", this->Width, this->Height / 2);
    GameLevel four; four.Load("level_4.txt", this->Width, this->Height / 2);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);

    playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

    ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
    Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("ball"));

    SFX1 = se->addSoundSourceFromFile("block_sfx1.mp3");
    SFX2 = se->addSoundSourceFromFile("block_sfx2.mp3");
    SFX3 = se->addSoundSourceFromFile("block_sfx3.mp3");

    se->play2D("bgm.mp3", true);
}

void Game::Update(GLfloat dt)
{
    //レベルをクリアーした
    if (this->Levels[Level].IsCompleted())
    {
        if (Level == 3)
            this->State = GAME_WIN;
        else
        {
            Level += 1;
            ResetLevel();
            ResetPlayer();
        }
            
    }
    //プレヤーが死んだ
    if (Ball->Position.y >= this->Height)
    {
        this->State = GAME_LOSE;
        ResetLevel();
        ResetPlayer();
    }
    
    Ball->Move(dt, this->Width, PlaySoundFX, SFX3);
    this->DoCollisions();
}

void Game::ProcessInput(GLfloat dt)
{
    if (this->State == GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_SPACE])
        {
            this->State = GAME_ACTIVE;
            this->Level = 0;
        }         
    }
    else if (this->State == GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;

        if (this->Keys[GLFW_KEY_A])
        {
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= velocity;
                if (Ball->Stuck)
                    Ball->Position.x -= velocity;
            }
        }

        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += velocity;
                if (Ball->Stuck)
                    Ball->Position.x += velocity;
            }
        }

        if (this->Keys[GLFW_KEY_S])
            Ball->Stuck = false;
    }

    else if (this->State == GAME_LOSE)
    {
        if (this->Keys[GLFW_KEY_SPACE])
            this->State = GAME_ACTIVE;
    }
}

void Game::Render()
{
    if (this->State == GAME_MENU)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("mainscreen"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    }
    else if (this->State == GAME_ACTIVE)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
        
        this->Levels[this->Level].Draw(*Renderer);

        Player->Draw(*Renderer);
        Ball->Draw(*Renderer);
    }
    else if (this->State == GAME_LOSE)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("losescreen"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    }
    else if (this->State == GAME_WIN)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("winscreen"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
    }

}

void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) 
            {
                if (!box.IsSolid)
                {
                    if (box.Layer > 2)
                        box.Layer = box.Layer - 1;
                    else if (box.Layer == 2)
                        box.Destroyed = true;

                    StopAllSFX();
                    se->play2D(SFX2, false);
                }
                else if (box.IsSolid)
                {
                    StopAllSFX();
                    se->play2D("block_sfx3.mp3", false);
                }
                    
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT)
                {
                    Ball->Velocity.x = -Ball->Velocity.x; 
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration;
                    else
                        Ball->Position.x -= penetration;
                }
                else
                {
                    Ball->Velocity.y = -Ball->Velocity.y;
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; 
                    else
                        Ball->Position.y += penetration; 
                }
            }
        }
    }

    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = BALL_VELOCITY.x * percentage * strength;
        Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        StopAllSFX();
        se->play2D("block_sfx1.mp3", false);
    }
}

void Game::ResetLevel()
{
    if (this->Level == 0)
        this->Levels[0].Load("level_1.txt", this->Width, this->Height / 2);
    else if (this->Level == 1)
        this->Levels[1].Load("level_2.txt", this->Width, this->Height / 2);
    else if (this->Level == 2)
        this->Levels[2].Load("level_3.txt", this->Width, this->Height / 2);
    else if (this->Level == 3)
        this->Levels[3].Load("level_4.txt", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
    Player->Size = PLAYER_SIZE;
    Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
    Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), BALL_VELOCITY);
}

Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	
        glm::vec2(1.0f, 0.0f),	
        glm::vec2(0.0f, -1.0f),
        glm::vec2(-1.0f, 0.0f)
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

Collision CheckCollision(BallObject& one, GameObject& two)
{
    glm::vec2 center(one.Position + one.Radius);
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x,
        two.Position.y + aabb_half_extents.y
    );

    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    glm::vec2 closest = aabb_center + clamped;
    difference = closest - center;

    if (glm::length(difference) <= one.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

void StopAllSFX()
{
    se->stopAllSoundsOfSoundSource(SFX1);
    se->stopAllSoundsOfSoundSource(SFX2);
    se->stopAllSoundsOfSoundSource(SFX3);
}

void PlaySoundFX(irrklang::ISoundSource* source)
{
    StopAllSFX();
    se->play2D(source, false);
}
