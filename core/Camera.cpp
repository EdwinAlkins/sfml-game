#include "Camera.hpp"

Camera::Camera()
{
    this->window = nullptr;
    this->center = sf::Vector2f(0, 0);
    this->size = sf::Vector2f(800, 600); // Default size
    this->zoom = 0.0f;
    this->rotationAngle = 0.0f;
}

Camera::~Camera()
{
}

void Camera::init()
{
    if (this->window == nullptr) {
        spdlog::error("Camera::init: window is null");
        return;
    }
    this->center = sf::Vector2f(this->window->getSize().x / 2, this->window->getSize().y / 2);
    this->size = sf::Vector2f(this->window->getSize().x, this->window->getSize().y);
    this->zoom = 0.0f;
    this->rotationAngle = 0.0f;
}

void Camera::setWindow(sf::RenderWindow *window)
{
    this->window = window;
}

sf::RenderWindow *Camera::getWindow()
{
    return this->window;
}

sf::View Camera::getView()
{
    if (this->window == nullptr) {
        spdlog::error("Camera::getView: window is null");
        return sf::View(); // Return default view
    }
    return this->window->getView();
}

void Camera::setCenter(sf::Vector2f center)
{
    this->center = center;
    if (this->window == nullptr) {
        spdlog::error("Camera::setCenter: window is null");
        return;
    }
    sf::View view = this->window->getView();
    view.setCenter(center);
    this->window->setView(view);
}

void Camera::move(sf::Vector2f offset)
{
    this->center += offset;
    if (this->window == nullptr) {
        spdlog::error("Camera::move: window is null");
        return;
    }
    sf::View view = window->getView();
    view.setCenter(this->center);
    window->setView(view);
}

sf::Vector2f Camera::getCenter()
{
    return this->center;
}

void Camera::setSize(sf::Vector2f size)
{
    this->size = size;
    if (this->window == nullptr) {
        spdlog::error("Camera::setSize: window is null");
        return;
    }
    sf::View view = window->getView();
    view.setSize(size);
    window->setView(view);
}

sf::Vector2f Camera::getSize()
{
    return this->size;
}

void Camera::setZoom(float zoom)
{
    this->zoom += zoom;
    if (this->window == nullptr) {
        spdlog::error("Camera::setZoom: window is null");
        return;
    }
    sf::View view = window->getView();
    view.zoom(1.0f + zoom);
    window->setView(view);
}

void Camera::resetZoom()
{
    this->setSize(this->size);
    this->zoom = 0.0f;
}

float Camera::getZoom()
{
    return this->zoom;
}

void Camera::setRotation(float angle)
{
    this->rotationAngle += angle;
    if (this->window == nullptr) {
        spdlog::error("Camera::setRotation: window is null");
        return;
    }
    sf::View view = window->getView();
    view.rotate(angle);
    window->setView(view);
}

void Camera::resetRotation()
{
    if (this->window == nullptr) {
        spdlog::error("Camera::resetRotation: window is null");
        return;
    }
    sf::View view = window->getView();
    view.rotate(this->rotationAngle * -1.0f);
    window->setView(view);
    this->rotationAngle = 0.0f;
}

float Camera::getRotation()
{
    return this->rotationAngle;
}

sf::FloatRect Camera::getViewBounds()
{
    // spdlog::debug("Camera getViewBounds");
    if (this->window == nullptr) {
        spdlog::error("Camera::getViewBounds: window is null");
        return sf::FloatRect(0, 0, 0, 0); // Return empty rectangle
    }
    

    
    sf::View view = this->window->getView();   
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    float viewRotation = view.getRotation() * (3.14159265f / 180.0f); // Convert to radians
    
    /*
    // Create rotation matrix using Eigen
    Eigen::Matrix2f rotationMatrix;
    rotationMatrix << std::cos(viewRotation), -std::sin(viewRotation),
                      std::sin(viewRotation),  std::cos(viewRotation);
    
    // Calculate the four corners of the view
    sf::Vector2f halfSize = viewSize / 2.0f;
    
    // Create corner vectors
    Eigen::Vector2f corners[4] = {
        Eigen::Vector2f(-halfSize.x, -halfSize.y), // topLeft
        Eigen::Vector2f(halfSize.x, -halfSize.y),  // topRight
        Eigen::Vector2f(-halfSize.x, halfSize.y),  // bottomLeft
        Eigen::Vector2f(halfSize.x, halfSize.y)    // bottomRight
    };
    
    // Apply rotation to all corners at once using Eigen
    Eigen::Vector2f rotatedCorners[4];
    for (int i = 0; i < 4; i++) {
        rotatedCorners[i] = rotationMatrix * corners[i];
    }
    
    // Translate corners to world space and find min/max
    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();
    
    for (int i = 0; i < 4; i++) {
        float worldX = rotatedCorners[i].x() + viewCenter.x;
        float worldY = rotatedCorners[i].y() + viewCenter.y;
        
        minX = std::min(minX, worldX);
        maxX = std::max(maxX, worldX);
        minY = std::min(minY, worldY);
        maxY = std::max(maxY, worldY);
    }
    
   */

    float cosRotation = std::cos(viewRotation);
    float sinRotation = std::sin(viewRotation);
    // Calculate the four corners of the view
    sf::Vector2f halfSize = viewSize / 2.0f;
    // Unrotated corners relative to center
    sf::Vector2f topLeft(-halfSize.x, -halfSize.y);
    sf::Vector2f topRight(halfSize.x, -halfSize.y);
    sf::Vector2f bottomLeft(-halfSize.x, halfSize.y);
    sf::Vector2f bottomRight(halfSize.x, halfSize.y);
    // Apply rotation to corners
    sf::Vector2f rotatedTopLeft(
        topLeft.x * cosRotation - topLeft.y * sinRotation,
        topLeft.x * sinRotation + topLeft.y * cosRotation
    );
    sf::Vector2f rotatedTopRight(
        topRight.x * cosRotation - topRight.y * sinRotation,
        topRight.x * sinRotation + topRight.y * cosRotation
    );
    sf::Vector2f rotatedBottomLeft(
        bottomLeft.x * cosRotation - bottomLeft.y * sinRotation,
        bottomLeft.x * sinRotation + bottomLeft.y * cosRotation
    );
    sf::Vector2f rotatedBottomRight(
        bottomRight.x * cosRotation - bottomRight.y * sinRotation,
        bottomRight.x * sinRotation + bottomRight.y * cosRotation
    );
    // Translate corners to world space
    rotatedTopLeft += viewCenter;
    rotatedTopRight += viewCenter;
    rotatedBottomLeft += viewCenter;
    rotatedBottomRight += viewCenter;
    
    // Find the min and max coordinates to create an AABB that contains the rotated view
    float minX = std::min({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float maxX = std::max({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float minY = std::min({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});
    float maxY = std::max({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});
    
    return sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
}