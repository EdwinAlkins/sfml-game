#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <spdlog/spdlog.h>
#include "Constants.hpp"

class Camera
{
private:
  sf::View view;
  sf::Vector2f center;
  sf::Vector2f baseSize;  // Taille de base (sans zoom)
  float zoomFactor;       // Facteur de zoom (1.0 = pas de zoom)
  float rotationAngle;

  void updateView();

public:
  Camera();
  explicit Camera(sf::Vector2f center, sf::Vector2f size = {SCREEN_WIDTH, SCREEN_HEIGHT});
  ~Camera();

  void init();
  void apply(sf::RenderWindow& window);

  void setCenter(sf::Vector2f center);
  void move(sf::Vector2f offset);
  sf::Vector2f getCenter() const;

  void setSize(sf::Vector2f size);
  sf::Vector2f getSize() const;

  void setZoom(float zoom);
  void resetZoom();
  float getZoom() const;

  void setRotation(float angle);
  void resetRotation();
  float getRotation() const;

  sf::FloatRect getViewBounds() const;
  const sf::View& getView() const;
};

#endif // CAMERA_HPP
