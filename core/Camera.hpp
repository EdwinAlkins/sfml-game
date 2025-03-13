#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <complex>
#include <spdlog/spdlog.h>
#include <Eigen/Dense>

class Camera
{
private:
  sf::RenderWindow *window;
  sf::Vector2f center;
  sf::Vector2f size;
  float zoom;
  float rotationAngle;
public:
  Camera();
  ~Camera();
  void init();
//   void update();
  void render();
//   void clean();
  void setWindow(sf::RenderWindow *window);
  sf::RenderWindow *getWindow();
  sf::View getView();
  void setCenter(sf::Vector2f center);
  void move(sf::Vector2f offset);
  sf::Vector2f getCenter();
  void setSize(sf::Vector2f size);
  sf::Vector2f getSize();
  void setZoom(float zoom);
  void resetZoom();
  float getZoom();
  void setRotation(float angle);
  void resetRotation();
  float getRotation();
  sf::FloatRect getViewBounds();
};


#endif // CAMERA_HPP
