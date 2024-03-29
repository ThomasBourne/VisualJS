#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include <vector>

#include <iostream>

#include "zone/zone.hpp"

#define CONTROL_PRESSED sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)

bool needUpdate = true;

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "SFML window", sf::Style::Close | sf::Style::Resize);

    //define zones
    std::vector<Zone*> zones = {
        new FolderZone(sf::Vector2u(300, 500), sf::Vector2u(0, 0), sf::Color(50,50,50), "C:\\"), //default side menu zone
        new HintZone(sf::Vector2u(300, 300), sf::Vector2u(0, 525), sf::Color(50,50,50)), //default hints zone
        new TextZone(sf::Vector2u(700, 1080), sf::Vector2u(300, 0)), //default text zone 1
        new TextZone(sf::Vector2u(700, 1080), sf::Vector2u(1050, 0)), //default text zone 1
    };
    unsigned int currentZone = 2; //define the current selected zone
    zones[currentZone]->selected = true; //initalise first selected zone

    while (window.isOpen())
    {
        // Handle events
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
                window.close();

            else if (evnt.type == sf::Event::Resized) {
                window.setView(sf::View(sf::FloatRect(0, 0, evnt.size.width, evnt.size.height)));
                needUpdate = true;
            }

            else if (evnt.type == sf::Event::MouseButtonReleased) {
                if (evnt.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = sf::Vector2f(sf::Mouse::getPosition(window));
                    for (int i = 0; i < zones.size(); i++) {
                        if (zones[i]->contains(mousePos) && currentZone != i) {
                            zones[i]->selected = true;
                            zones[i]->ReadTransfer(needUpdate);
                            zones[currentZone]->selected = false;
                            currentZone = i;
                            needUpdate = true;
                            break;
                        }
                    }
                }
            }

            else if (CONTROL_PRESSED) {
                if (evnt.type == sf::Event::MouseWheelMoved) {
                    sf::View view = window.getView();
                    if (evnt.mouseWheel.delta > 0)
                        view.zoom(0.95f);
                    else
                        view.zoom(1.05f);
                    window.setView(view);
                    needUpdate = true;
                }

                else if (evnt.type == sf::Event::KeyReleased && evnt.key.code == sf::Keyboard::Q) {
                    sf::View view = window.getView();
                    view.setCenter(sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2));
                    view.setSize(sf::Vector2f(window.getSize()));
                    window.setView(view);
                    needUpdate = true;
                }
                else if (evnt.type == sf::Event::KeyReleased && evnt.key.code == sf::Keyboard::W)
                    window.close();
            }

            else if (evnt.type == sf::Event::TextEntered) {
                zones[currentZone]->HandleInput(evnt);
                needUpdate = true;
            }

            else if (evnt.type == sf::Event::KeyReleased) {
                zones[currentZone]->MoveCursor(evnt);
                needUpdate = true;
            }

            else if(evnt.type == sf::Event::MouseWheelMoved) {
                sf::View view = window.getView();
                if (evnt.mouseWheel.delta > 0)
                    view.move(sf::Vector2f(0, -1.f));
                else
                     view.move(sf::Vector2f(0, 1.f));
                window.setView(view);
                needUpdate = true;
            }
        }

        if (needUpdate) {
            // Clear the window
            window.clear();

            // Draw the zones
            for (Zone* zone : zones)
                zone->Draw(window);

            // Display the window
            window.display();
            needUpdate = false;
        }
    }

    return 0;
}