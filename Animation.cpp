#include <iostream>
#include "Animation.h"
#include <cmath>
#include <utility>


sf::Texture Animation::s_defaultTexture; // SFML3: backing texture so the default-constructed m_sprite has something to bind to

Animation::Animation() = default;

Animation::Animation(const std::string &name, const sf::Texture &t)
        : Animation(name, t, 1, 0) {}

Animation::Animation(std::string name, const sf::Texture &t, size_t frameCount, size_t speed)
        : m_name(std::move(name)), m_sprite(t), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed) {
    m_size = vec2((float) t.getSize().x / float(frameCount), (float) t.getSize().y);
    m_sprite.setOrigin(sf::Vector2f(m_size.x / 2.0f, m_size.y / 2.0f));
    m_sprite.setTextureRect(sf::IntRect(
            sf::Vector2i(std::floor(float(m_currentFrame) * m_size.x), 0),
            sf::Vector2i(m_size.x, m_size.y)
    ));
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update() {
    // add the speed variable to the current frame
    m_currentFrame++;

    // TODO: 1) calculate the correct frame of animation to play based on currentFrame and speed
    //       2) set the texture rectangle properly (see constructor for sample)
    if (m_speed == 0) {
        std::cerr << "m_speed is 0\n";
        return;
    }
    int animFrame = (m_currentFrame / m_speed) % m_frameCount;
    // std::cerr << "anim frame is " << animFrame << '\n';
    const int FW = m_size.x;
    const int FH = m_size.y;
    m_sprite.setTextureRect({{animFrame * FW, 0}, {FW, FH}});
}

bool Animation::hasEnded() const {
    // TODO: detect when animation has ended (last frame waw played) and return true
    return false;
}

const vec2 &Animation::getSize() const {
    return m_size;
}

const std::string &Animation::getName() const {
    return m_name;
}

sf::Sprite &Animation::getSprite() {
    return m_sprite;
}
