#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    // Create a window with a title "SFML Window"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");
    
    // Set the framerate limit to 60 FPS
    window.setFramerateLimit(60);
    
    // Create a circle shape
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    shape.setPosition(350.f, 250.f);
    
    // Main game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window if requested
            if (event.type == sf::Event::Closed)
                window.close();
                
            // Close window if Escape key is pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }
        
        // Clear the window with black color
        window.clear(sf::Color::Black);
        
        // Draw the shape
        window.draw(shape);
        
        // Display what was drawn
        window.display();
    }
    
    return 0;
}
