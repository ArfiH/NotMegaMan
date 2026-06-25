#include <iostream>
#include <fstream>

#include "SFML//Window/Event.hpp"
#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "Assets.h"
#include "Physics.h"
#include "GameEngine.h"
#include "Animation.h"
#include "Components.h"
#include "Action.h"
#include "SFML/Graphics/RectangleShape.hpp"


Scene_Play::Scene_Play(GameEngine *gameEngine, const std::string &levelPath)
        : Scene(gameEngine), m_levelPath(levelPath), m_gridText(gameEngine->assets().getFont("Mario")) {
    init(levelPath);
}

void Scene_Play::init(const std::string &levelPath) {
    registerAction(sf::Keyboard::Key::P, "PAUSE");
    registerAction(sf::Keyboard::Key::Escape, "QUIT");
    registerAction(sf::Keyboard::Key::T, "TOGGLE_TEXTURE");   // Toggle drawing (T)extures
    registerAction(sf::Keyboard::Key::C, "TOGGLE_COLLISION"); // Toggle drawing (C)ollision Boxes
    registerAction(sf::Keyboard::Key::G, "TOGGLE_GRID");      // Toggle drawing (G)rid

    registerAction(sf::Keyboard::Key::W, "JUMP");
    registerAction(sf::Keyboard::Key::D, "RIGHT");
    registerAction(sf::Keyboard::Key::A, "LEFT");
    registerAction(sf::Keyboard::Key::S, "DOWN");
    registerAction(sf::Keyboard::Key::Space, "SHOOT");

    // TODO: Register all MOUSE gameplay Actions
    // registerMouseAction(sf::Mouse::Button::Left, "SHOOT");

    m_gridText.setCharacterSize(12);
    m_gridText.setFont(m_game->assets().getFont("Mario"));
    m_gridText.setFont(m_game->assets().getFont("Tech"));

    loadLevel(levelPath);
}

vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity) {
    // TODO: This function takes in a grid (x,y) position and an Entity
    //       Return a vec2 indicating where the CENTER position of the Entity should be
    //       You must use the Entity's Animation size to position it correctly
    //       The size of the grid width and height is stored in m_gridSize.x and m_gridSize.y
    //       The bottom-left corner of the Animation should aligh with the bottom left of the grid cell
    unsigned int width = 1280;
    unsigned int height = 720;

    float x, y;
    vec2 entitySize = entity->getComponent<CBoundingBox>().halfSize;
    // x = gridX * m_gridSize.x + 64;
    // y = height - ((gridY + 1) * m_gridSize.y) + 64;

    x = gridX * m_gridSize.x + entitySize.x;
    y = height - ((gridY + 1) * m_gridSize.y) + entitySize.y;
    return vec2(x, y);
}

void Scene_Play::loadLevel(const std::string &fileName) {
    // reset the entity manager every time we load a level
    m_entityManager = EntityManager();

    // TODO: read in the level file and add the appropriate entities
    //       use the PlayerConfig struct m_playerConfig to store player properties
    //       this struct is defined at the top of Scene_Play.h
    // Read levelPath.txt
    std::ifstream file("../levels/" + fileName);

    // 2. Check if the file opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return;
    }

    std::string temp;
    while (file >> temp) {
        if (temp == "Player") {
            file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY >> m_playerConfig.SPEED >> m_playerConfig.MAX_SPEED >> m_playerConfig.JUMP >> m_playerConfig.GRAVITY;
            file >> m_playerConfig.WEAPON;
        }
        else if (temp == "Tile") { 
            std::string anim = "";
            file >> anim;
            float gX, gY;
            file >> gX >> gY;
            auto tile = m_entityManager.addEntity("tile");
            // IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
            tile->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            tile->addComponent<CBoundingBox>(m_game->assets().getAnimation(anim).getSize());
            tile->addComponent<CTransform>(gridToMidPixel(gX, gY, tile));
        }
        else if (temp == "Dec") {
            std::string anim = "";
            file >> anim;
            float gX, gY;
            file >> gX >> gY;
            auto dec = m_entityManager.addEntity("dec");
            // IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
            dec->addComponent<CAnimation>(m_game->assets().getAnimation(anim), true);
            dec->addComponent<CTransform>(gridToMidPixel(gX, gY, dec));
        }
    }

    // NOTE: all the code below is sample code which shows you how to
    //       set up and use entities with the new syntax, it should be removed

    spawnPlayer();

    // NOTE: Your final code should position the entity with the grid x,y position read from the file:
    // brick->addComponent<CTransform>(gridToMidPixel(gridX, gridY, brick));

    // if (brick->getComponent<CAnimation>().animation.getName() == "Brick") {
    //     std::cout << "This could be a good way of identifying if a tile is a brick!\n";
    // }

    // NOTE: THIS IS INCREDIBLY IMPORTANT PLEASE READ THIS EXAMPLE
    //       Components are now returned as references rather than pointers
    //       If you do not specify a reference variable type, it will COPY the component
    //       Here is an example:
    //
    //       This will COPY the transform into the variable 'transform1' - it is INCORRECT
    //       Any changes you make to transform1 will not be changed inside the entity
    //       auto transform1 = entity->get<CTransform>()
    //
    //       This will REFERENCE the transform with the variable 'transform2' - it is CORRECT
    //       Now any changes you make to transform2 will be changed inside the entity
    //       auto& transform2 = entity->get<CTransform>()
}

void Scene_Play::spawnPlayer() {
    // here is a sample player entity which you can use to construct other entities
    const float GRAVITY = 0.5f;

    m_player = m_entityManager.addEntity("player");
    m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
    // m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
    m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
    m_player->addComponent<CBoundingBox>(vec2(m_playerConfig.CX, m_playerConfig.CY));
    m_player->addComponent<CInput>();
    m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
    m_player->addComponent<CState>("Standing");

    // TODO: be sure to add the remaining components to the player
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> e) {
    // TODO: this should spawn a bullet at the given entity, going in the direction the entity is facing
    std::cerr << "Spawned Bullet\n";
    auto bullet = m_entityManager.addEntity("bullet");
    // bullet->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
    bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Buster"), true);
    vec2 playerPos = e->getComponent<CTransform>().pos;
    bullet->addComponent<CBoundingBox>(vec2(32.f, 32.f));
    bullet->addComponent<CTransform>(playerPos);
    vec2 scale = e->getComponent<CTransform>().scale;
    bullet->getComponent<CTransform>().velocity = vec2(20.f * scale.x, 0.f);
}

void Scene_Play::update() {
    m_entityManager.update();

    // TODO: implement pause functionality

    sMovement();
    sLifespan();
    sCollision();
    sAnimation();
    sRender();
    m_currentFrame++;
}

void Scene_Play::sMovement() {
    // TODO: Implement player movement/jumping based on its CInput component
    // TODO: Implement gravity's effect on the player
    // TODO: Implement the maximum player speed in both X and Y directions
    // NOTE: Setting an entity's scale.x to -1/1 will make it face to the left/right

    // Update prevPos of player
    m_player->getComponent<CTransform>().prevPos = m_player->getComponent<CTransform>().pos;

    // for all entities having CGravity component, apply gravity velocity
    m_player->getComponent<CTransform>().velocity.y += m_player->getComponent<CGravity>().gravity;

    // set limit to player speed
    m_player->getComponent<CTransform>().velocity.x = std::min(m_playerConfig.MAX_SPEED, m_player->getComponent<CTransform>().velocity.x);
    m_player->getComponent<CTransform>().velocity.y = std::min(m_playerConfig.MAX_SPEED, m_player->getComponent<CTransform>().velocity.y);

    m_player->getComponent<CTransform>().pos += m_player->getComponent<CTransform>().velocity;

    for (const auto& e: m_entityManager.getEntities("bullet")) {
        e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity;
    }
}

void Scene_Play::sLifespan() {
    // TODO: Check lifespan of entities the have them, and destroy them if the go over
    for (const auto& e: m_entityManager.getEntities()) {
        if (!e->hasComponent<CLifespan>()) {
            continue;
        }
        auto lifespanComponent = e->getComponent<CLifespan>();
        if (lifespanComponent.frameCreated + lifespanComponent.lifespan < m_currentFrame) {
            e->destroy();
        }
    }
}

bool Scene_Play::isColliding(std::shared_ptr<Entity> e, const std::string& tag) {
    bool isCollided = false;
    bool isCollidedFromAbove = false;

    // player collision check with ground
    for (const auto& b: m_entityManager.getEntities(tag)) {
        if (!b->hasComponent<CBoundingBox>() || e->getComponent<CAnimation>().animation.getName() == b->getComponent<CAnimation>().animation.getName()) {
            continue;
        }

        vec2 pos = e->getComponent<CTransform>().pos;
        vec2 prevPos = e->getComponent<CTransform>().prevPos;
        vec2 overlap = m_worldPhysics.GetOverlap(e, b);
        vec2 prevOverlap = m_worldPhysics.GetPreviousOverlap(e, b);

        if (overlap.x > 0 && overlap.y > 0) {
            isCollided = true;
            e->getComponent<CTransform>().velocity.y = 0;

            // check player direction and resolve collision
            // player came from either left or right
            if (prevOverlap.y > 0) {
                // player came from right
                if (prevPos.x > pos.x) {
                    e->getComponent<CTransform>().pos.x += overlap.x;
                }
                else {
                    e->getComponent<CTransform>().pos.x -= overlap.x;
                }
            }
            else {
                // player came from down
                if (prevPos.y > pos.y) {
                    // destroy tile which has brick animation
                    if (b->getComponent<CAnimation>().animation.getName() == "Brick") {
                        spawnBrickDebris(b);
                        b->destroy();
                    }

                    if (b->getComponent<CAnimation>().animation.getName() == "ActiveQues") {
                        spawnCoinSpin(b);
                        b->addComponent<CAnimation>(m_game->assets().getAnimation("InactiveQues"), true);
                    }

                    e->getComponent<CTransform>().pos.y += overlap.y;
                }
                else {
                    isCollidedFromAbove = true;
                    e->getComponent<CTransform>().pos.y -= overlap.y;
                }
            }
        }
    }
    // return isCollided;
    return isCollidedFromAbove;
}

void Scene_Play::spawnCoinSpin(std::shared_ptr<Entity> tile) {
    std::cerr << "Coin spawned\n";
    const float coinGap = 60.f;
    const vec2 coinSize = vec2(32.f, 32.f);
    auto coin = m_entityManager.addEntity("coin");
    coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), true);
    vec2 tilePos = tile->getComponent<CTransform>().pos;
    coin->addComponent<CTransform>(vec2(tilePos.x, tilePos.y - coinGap));
    coin->addComponent<CBoundingBox>(coinSize);
}

void Scene_Play::spawnBrickDebris(std::shared_ptr<Entity> tile) {
    auto debris = m_entityManager.addEntity("debris");
    debris->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), true);
    debris->addComponent<CTransform>(tile->getComponent<CTransform>().pos);
    debris->addComponent<CBoundingBox>(tile->getComponent<CBoundingBox>().size);
    debris->addComponent<CLifespan>(25, m_currentFrame);
}


void Scene_Play::sCollision() {
    // REMEMBER: SFML's (0,0) position is in the TOP-LEFT corner
    //           This means jumping will have a negative y-component
    //           and gravity will have a positive y-component
    //           Also, something BELOW something else will hava a y value GREATER than it
    //           Also, something ABOVE something else will hava a y value LESS than it

    // is player fell down a hole, go back to main menu
    vec2 pos = m_player->getComponent<CTransform>().pos;
    if (pos.y > m_game->window().getSize().y) {
        onEnd();
    }

    // player/tile collision
    m_isCollidingWithGround = isColliding(m_player, "tile");

    // player/coin collision
    for (const auto& b: m_entityManager.getEntities("coin")) {
        if (!b->hasComponent<CBoundingBox>()) {
            continue;
        }

        vec2 overlap = m_worldPhysics.GetOverlap(m_player, b);
        if (overlap.x > 0 && overlap.y > 0) {
            std::cerr << "Coin collected\n";
            m_score += 1;
            b->destroy();
        }
    }

    // bullet/tile collision
    for (const auto& b : m_entityManager.getEntities("bullet")) {
        for (const auto& e: m_entityManager.getEntities("tile")) {
            if (!e->hasComponent<CBoundingBox>()) {
                continue;
            }

            vec2 overlap = m_worldPhysics.GetOverlap(b, e);            
            if (overlap.x > 0 && overlap.y > 0) {
                if (e->getComponent<CAnimation>().animation.getName() == "Brick") {
                    spawnBrickDebris(e);
                    e->destroy();
                }
                b->destroy();
            }
        }
    }

    // TODO: Implement player/tile collisions and resolutions
    //       Update the CState component of the player to store whether
    //       it is currently on the ground or in the air. This will be
    //       used by the Animation system
    // TODO: Check to see if the player has fallen down a hole (y > height())
    // TODO: Don't let the player walk off the left side of the map
}

void Scene_Play::sDoAction(const Action &action) {
    m_player->getComponent<CState>().state = "Standing";
    
    if (action.type() == "START") {
        if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
        if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
        if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
        if (action.name() == "PAUSE") { setPaused(!m_paused); }
        if (action.name() == "QUIT") { onEnd(); }
        if (action.name() == "JUMP") {
            if (m_player->getComponent<CState>().state != "Jumping") {
                if (m_isCollidingWithGround) {
                    std::cerr << "Perform Jump\n";
                    m_player->getComponent<CTransform>().velocity.y = -m_playerConfig.JUMP;
                }
                m_player->getComponent<CState>().state = "Jumping";
            }
        }
        if (action.name() == "RIGHT") {
            std::cerr << "Perform RIGHT\n";
            m_player->getComponent<CTransform>().scale.x = 1;
            m_player->getComponent<CTransform>().pos.x += m_playerConfig.SPEED;
            m_player->getComponent<CState>().state = "Running";
        }
        if (action.name() == "LEFT") {
            std::cerr << "Perform LEFT\n";
            m_player->getComponent<CTransform>().scale.x = -1;
            m_player->getComponent<CTransform>().pos.x += -m_playerConfig.SPEED;
            m_player->getComponent<CState>().state = "Running";
        }
        if (action.name() == "DOWN") {
            std::cerr << "Perform DOWN\n";
            m_player->getComponent<CTransform>().velocity.y = m_playerConfig.JUMP;
        }
        if (action.name() == "SHOOT") {
            std::cerr << "Perform SHOOT\n";
            spawnBullet(m_player);
            m_player->getComponent<CState>().state = "Shooting";
        }
    }   
    else if (action.type() == "END") {

    }
}

void Scene_Play::sAnimation() {
    // TODO: Complete the Animation class code first

    if (m_player->getComponent<CState>().state == "Jumping") {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Jump"), true);
    }
    else if (m_player->getComponent<CState>().state == "Standing") {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);        
    }
    else if (m_player->getComponent<CState>().state == "Running") {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);        
    }
    else if (m_player->getComponent<CState>().state == "Shooting") {
        m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Shot"), true);        
    }
    
    for (const auto &e: m_entityManager.getEntities()) {
        if (e->hasComponent<CAnimation>()) {
            e->getComponent<CAnimation>().animation.update();
        }
    }
}

void Scene_Play::onEnd() {
    // TODO: when the scene ends, change back to the MENU scene
    // use m_game->changeScene(correct params);
    m_game->changeScene( "MENU", std::make_shared<Scene_Menu>(m_game));
    // m_game->quit();
}

void Scene_Play::sRender() {
    // color the background darker, so you know that the game is paused
    if (!m_paused) {
        m_game->window().clear(sf::Color(100, 100, 255));
    } else {
        m_game->window().clear(sf::Color(50, 50, 150));
    }

    // set the viewport of the window to be centered on the player if it's far enough right
    auto &pPos = m_player->getComponent<CTransform>().pos;
    float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
    sf::View view = m_game->window().getView();
    view.setCenter(sf::Vector2f(windowCenterX, m_game->window().getSize().y - view.getCenter().y));
    m_game->window().setView(view);

    // draw all Entity textures / animations
    if (m_drawTextures) {
        for (const auto &e: m_entityManager.getEntities()) {
            auto &transform = e->getComponent<CTransform>();
            if (e->hasComponent<CAnimation>()) {
                auto &animation = e->getComponent<CAnimation>().animation;
                animation.getSprite().setRotation(sf::degrees(transform.angle));
                animation.getSprite().setPosition(
                        sf::Vector2f(transform.pos.x, transform.pos.y)
                );
                animation.getSprite().setScale(
                        sf::Vector2f(transform.scale.x, transform.scale.y)
                );
                m_game->window().draw(animation.getSprite());
            }
        }
    }

    // draw all Entity collision bounding boxes with a rectangle shape
    if (m_drawCollision) {
        for (const auto &e: m_entityManager.getEntities()) {
            if (e->hasComponent<CBoundingBox>()) {
                auto &box = e->getComponent<CBoundingBox>();
                auto &transform = e->getComponent<CTransform>();
                sf::RectangleShape rect;
                rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
                rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
                rect.setPosition(sf::Vector2f(transform.pos.x, transform.pos.y));
                rect.setFillColor(sf::Color(0, 0, 0, 0));
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }

    // draw the grid so that can easily debug
    if (m_drawGrid) {
        float leftX = m_game->window().getView().getCenter().x - width() / 2.0;
        float rightX = leftX + width() + m_gridSize.x;
        float nextGridX = leftX - ((int) leftX % (int) m_gridSize.x);

        for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
            drawLine(vec2(x, 0), vec2(x, height()));
        }

        for (float y = 0; y < height(); y += m_gridSize.y) {
            drawLine(vec2(leftX, height() - y), vec2(rightX, height() - y));

            for (float x = nextGridX; x < rightX; x += m_gridSize.x) {
                std::string xCell = std::to_string((int) x / (int) m_gridSize.x);
                std::string yCell = std::to_string((int) y / (int) m_gridSize.y);
                m_gridText.setString("(" + xCell + "," + yCell + ")");
                m_gridText.setPosition(sf::Vector2f(x + 3, height() - y - m_gridSize.y + 2));
                m_game->window().draw(m_gridText);
            }
        }
    }
}
