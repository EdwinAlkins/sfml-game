#include "Camera.hpp"

Camera::Camera()
    : center(0, 0)
    , baseSize(SCREEN_WIDTH, SCREEN_HEIGHT)
    , zoomFactor(1.0f)
    , rotationAngle(0.0f)
{
    view = sf::View(center, baseSize);
}

Camera::Camera(sf::Vector2f center, sf::Vector2f size)
    : center(center)
    , baseSize(size)
    , zoomFactor(1.0f)
    , rotationAngle(0.0f)
{
    view = sf::View(center, baseSize);
}

Camera::~Camera()
{
}

void Camera::init()
{
    spdlog::info("Camera initialized with center ({}, {}) and size ({}, {}), zoom={}", 
                 center.x, center.y, baseSize.x, baseSize.y, zoomFactor);
}

void Camera::apply(sf::RenderWindow& window)
{
    window.setView(view);
}

void Camera::updateView()
{
    // Calculer la taille effective avec le zoom
    sf::Vector2f effectiveSize = baseSize * zoomFactor;
    
    // Appliquer tous les paramètres au view
    view.setCenter(center);
    view.setSize(effectiveSize);
    view.setRotation(rotationAngle);
}

void Camera::setCenter(sf::Vector2f center)
{
    this->center = center;
    updateView();
}

void Camera::move(sf::Vector2f offset)
{
    this->center += offset;
    updateView();
}

sf::Vector2f Camera::getCenter() const
{
    return this->center;
}

void Camera::setSize(sf::Vector2f size)
{
    this->baseSize = size;
    updateView();
}

sf::Vector2f Camera::getSize() const
{
    return this->baseSize;
}

void Camera::setZoom(float zoom)
{
    // zoom > 0 => zoom in (réduit la taille effective)
    // zoom < 0 => zoom out (augmente la taille effective)
    zoomFactor += zoom;
    
    // Clamp pour éviter les valeurs invalides
    if (zoomFactor < 0.1f) zoomFactor = 0.1f;
    if (zoomFactor > 10.0f) zoomFactor = 10.0f;
    
    updateView();
}

void Camera::resetZoom()
{
    this->zoomFactor = 1.0f;
    updateView();
}

float Camera::getZoom() const
{
    return this->zoomFactor;
}

void Camera::setRotation(float angle)
{
    this->rotationAngle += angle;
    
    // Normaliser l'angle entre 0 et 360
    while (this->rotationAngle >= 360.0f) this->rotationAngle -= 360.0f;
    while (this->rotationAngle < 0.0f) this->rotationAngle += 360.0f;
    
    updateView();
}

void Camera::resetRotation()
{
    this->rotationAngle = 0.0f;
    updateView();
}

float Camera::getRotation() const
{
    return this->rotationAngle;
}

sf::FloatRect Camera::getViewBounds() const
{
    sf::Vector2f viewCenter = view.getCenter();
    sf::Vector2f viewSize = view.getSize();
    float viewRotation = view.getRotation() * (3.14159265f / 180.0f);

    float cosRotation = std::cos(viewRotation);
    float sinRotation = std::sin(viewRotation);

    sf::Vector2f halfSize = viewSize / 2.0f;

    sf::Vector2f topLeft(-halfSize.x, -halfSize.y);
    sf::Vector2f topRight(halfSize.x, -halfSize.y);
    sf::Vector2f bottomLeft(-halfSize.x, halfSize.y);
    sf::Vector2f bottomRight(halfSize.x, halfSize.y);

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

    rotatedTopLeft += viewCenter;
    rotatedTopRight += viewCenter;
    rotatedBottomLeft += viewCenter;
    rotatedBottomRight += viewCenter;

    float minX = std::min({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float maxX = std::max({rotatedTopLeft.x, rotatedTopRight.x, rotatedBottomLeft.x, rotatedBottomRight.x});
    float minY = std::min({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});
    float maxY = std::max({rotatedTopLeft.y, rotatedTopRight.y, rotatedBottomLeft.y, rotatedBottomRight.y});

    return sf::FloatRect(minX, minY, maxX - minX, maxY - minY);
}

const sf::View& Camera::getView() const
{
    return view;
}
