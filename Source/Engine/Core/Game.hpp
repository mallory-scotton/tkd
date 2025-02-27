///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Utils.hpp>
#include <Engine/Core/World.hpp>
#include <Engine/Graphics/SpriteRenderer.hpp>
#include <Engine/Core/Transform.hpp>
#include <Engine/Core/Input.hpp>
#include <Engine/Gui/Editor.hpp>
#include <Engine/Physics.hpp>
#include <Engine/Gui/Configurator.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Game
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    sf::RenderWindow m_window;          //<!
    sf::Clock m_clock;                  //<!
    World m_world;                      //<!
    float m_fixedTimeStep = 1.f / 60.f; //<!
    float m_accumulatedTime = 0.f;      //<!
    bool m_editorMode = false;          //<!
    UPtr<Gui::Editor> m_editor;         //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param title
    /// \param width
    /// \param height
    ///
    ///////////////////////////////////////////////////////////////////////////
    Game(const String& title, Uint32 width, Uint32 height)
        : m_window(sf::VideoMode({width, height}), title)
    {
        m_window.setFramerateLimit(60);

        if (!ImGui::SFML::Init(m_window))
            std::cerr << "Cannot initialied Imgui" << std::endl;

        Gui::Configurator::setup();

        m_editor = std::make_unique<Gui::Editor>(m_world);
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    ~Game()
    {
        ImGui::SFML::Shutdown();
    }

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void handleEvents(void)
    {
        while (const Optional<sf::Event> event = m_window.pollEvent()) {
            ImGui::SFML::ProcessEvent(m_window, *event);

            if (event->is<sf::Event::Closed>()) {
                m_window.close();
            }

            if (const auto& key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::F12) {
                    m_editorMode = !m_editorMode;
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void renderAll(sf::RenderTarget& target)
    {
        for (auto& object : m_world.getGameObjects()) {
            if (object->isActive()) {
                if (auto renderer = object->getComponent<SpriteRenderer>()) {
                    renderer->render(target);
                }
            }
        }
    }

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void initialize(void)
    {
        auto player = m_world.createGameObject("Player");

        player->addComponent<Transform>();
        player->addComponent<SpriteRenderer>("player.png");
        player->addComponent<InputComponent>();
        player->addComponent<BoxCollider>();

        auto floor = m_world.createGameObject("Floor");

        floor->addComponent<Transform>(Vec2f(m_window.getSize().x / 2.f, 500))->setScale(Vec2f(100.f, 1.f));
        floor->addComponent<SpriteRenderer>("player.png")->setColor(sf::Color::Blue);
        floor->addComponent<BoxCollider>();

        for (auto& object : m_world.getGameObjects()) {
            object->start();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void run(void)
    {
        while (m_window.isOpen()) {
            handleEvents();

            sf::Time restart = m_clock.restart();
            float deltaTime = restart.asSeconds();
            m_accumulatedTime += deltaTime;

            ImGui::SFML::Update(m_window, restart);

            if (m_editorMode) {
                auto& gameViewTexture = m_editor->getGameViewTexture();

                gameViewTexture.clear(sf::Color(50, 50, 50));
                renderAll(gameViewTexture);
                gameViewTexture.display();

                m_editor->render();
            }

            m_world.update(deltaTime);

            while (m_accumulatedTime >= m_fixedTimeStep) {
                m_world.FixedUpdate(m_fixedTimeStep);
                m_accumulatedTime -= m_fixedTimeStep;
            }

            m_window.clear(sf::Color(50, 50, 50));

            if (!m_editorMode) {
                renderAll(m_window);
            }

            ImGui::SFML::Render(m_window);

            m_window.display();
        }
    }
};

} // namespace tkd
