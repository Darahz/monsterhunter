#include <SFML/Graphics.hpp>
#include <iostream>


class Window {
public:
    Window();
    ~Window();

    bool initialize();
    void update();
    void render();
    void loadFont(const std::string& fontPath);
    bool isOpen() const;
    std::string getVersion() const;

private:
    sf::RenderWindow window;
    sf::Font font;
    int majorVersion = 8;
    int minorVersion = 18;
    int patchVersion = 0;
};
