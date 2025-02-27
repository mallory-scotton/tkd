///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Gui/Editor.hpp>
#include <Engine/Gui/ComponentRegistry.hpp>
#include <Engine/Core/Component.hpp>
#include <Engine/Graphics/SpriteRenderer.hpp>
#include <Engine/Core/Transform.hpp>
#include <Engine/Core/Input.hpp>
#include <Engine/Utils.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <IconsFontAwesome6.hpp>
#include <imgui-SFML.h>
#include <cxxabi.h>
#include <typeinfo>
#include <typeindex>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Gui
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Gui
{

#define INSPECTOR ICON_FA_MAGNIFYING_GLASS " Inspector"
#define HIERARCHY ICON_FA_SITEMAP " Hierarchy"
#define GAME_VIEW ICON_FA_EXPAND " Game View"
#define CONSOLE   ICON_FA_TERMINAL " Console"

///////////////////////////////////////////////////////////////////////////////
Editor::Editor(World& world)
    : m_world(world)
    , m_selected(nullptr)
{
    registerComponentInspectors();
}

///////////////////////////////////////////////////////////////////////////////
String Editor::getCleanClassName(const char* name) const
{
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    String cleaned(demangled);
    std::free(demangled);
    return (cleaned);
}

///////////////////////////////////////////////////////////////////////////////
void Editor::drawHierarchy(void)
{
    ImGui::Begin(HIERARCHY);

    if (ImGui::Button("Create GameObject")) {
        m_world.createGameObject("New GameObject");
    }

    ImGui::Separator();

    for (const auto& object : m_world.getGameObjects()) {
        bool isSelected = (object.get() == m_selected);
        if (ImGui::Selectable(object->getName().c_str(), isSelected)) {
            m_selected = object.get();
        }
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Editor::drawInspector(void)
{
    ImGui::Begin(INSPECTOR);

    if (!m_selected) {
        ImGui::Text("No GameObject selected");
        return (ImGui::End());
    }

    char name[256];
    ::strcpy(name, m_selected->getName().c_str());
    if (ImGui::InputText("Name", name, sizeof(name))) {
        m_selected->setName(name);
    }

    bool active = m_selected->isActive();
    if (ImGui::Checkbox("Active", &active)) {
        m_selected->setActive(active);
    }

    ImGui::Separator();

    int index = 0;
    for (auto& component : m_selected->getComponents()) {
        String type = getCleanClassName(typeid(*component).name());
        bool isEnabled = component->isEnabled();
        String sidx = "##" + std::to_string(index);

        if (ImGui::CollapsingHeader((type + sidx).c_str(), 32)) {
            if (ImGui::Checkbox(("Enabled" + sidx).c_str(), &isEnabled)) {
                component->setEnabled(isEnabled);
            }

            if (ComponentRegistry::instance().hasInspector(component.get())) {
                ComponentRegistry::instance().inspectComponent(component.get());
            }

            if (ImGui::Button(
                (ICON_FA_TRASH " Remove Component" + sidx).c_str())
            ) {
                m_selected->removeComponent(component.get());
                break;
            }
        }

        index++;
    }

    if (ImGui::Button(ICON_FA_PLUS " Add Component")) {
        ImGui::OpenPopup("ComponentList");
    }

    if (ImGui::BeginPopup("ComponentList")) {
        ImGui::Text("Add Component");
        ImGui::Separator();

        for (
            const auto& name :
            ComponentRegistry::instance().getComponentNames()
        ) {
            if (ImGui::Selectable(name.c_str())) {
                ComponentRegistry::instance().createComponent(
                    name, m_selected
                );
            }
        }

        ImGui::EndPopup();
    }

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
ImTextureID convertGLTextureHandleToImTextureID(Uint32 glTextureHandle)
{
    ImTextureID textureID{};
    std::memcpy(&textureID, &glTextureHandle, sizeof(Uint32));
    return textureID;
}

///////////////////////////////////////////////////////////////////////////////
void Editor::drawGameView(void)
{
    ImGui::Begin(GAME_VIEW);

    ImVec2 size = ImGui::GetContentRegionAvail();

    size.x = std::max(size.x, 1.f);
    size.y = std::max(size.y, 1.f);

    if (
        !m_gameViewInitialized ||
        m_texture.getSize() != sf::Vector2u(size.x, size.y)
    ) {
        (void)m_texture.resize({(Uint32)size.x, (Uint32)size.y});
        m_gameViewInitialized = true;
    }

    ImTextureID textureID = convertGLTextureHandleToImTextureID(
        m_texture.getTexture().getNativeHandle()
    );

    ImGui::Image(
        textureID,
        ImVec2(size.x, size.y),
        ImVec2(0, 1),
        ImVec2(1, 0)
    );

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Editor::drawConsole(void)
{
    ImGui::Begin(CONSOLE);

    if (ImGui::Button("Clear")) {
        tkd::Logger::clear();
    }

    ImGui::SameLine();

    static bool showInfo = true;
    static bool showWarning = true;
    static bool showError = true;
    static bool showDebug = true;

    ImGui::Checkbox("Info", &showInfo);
    ImGui::SameLine();
    ImGui::Checkbox("Warning", &showWarning);
    ImGui::SameLine();
    ImGui::Checkbox("Error", &showError);
    ImGui::SameLine();
    ImGui::Checkbox("Debug", &showDebug);

    ImGui::SameLine();
    if (ImGui::Button("Copy All")) {
        std::string clipboardText;
        for (const auto& entry : tkd::Logger::getEntries()) {
            clipboardText += entry.timestamp + " " +
                             (entry.level == tkd::Logger::Level::INFO ? "[INFO] " :
                              entry.level == tkd::Logger::Level::WARNING ? "[WARNING] " :
                              entry.level == tkd::Logger::Level::ERROR ? "[ERROR] " : "[DEBUG] ") +
                             entry.message + "\n";
        }
        ImGui::SetClipboardText(clipboardText.c_str());
    }

    ImGui::Separator();

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    const auto& entries = tkd::Logger::getEntries();
    for (const auto& entry : entries) {
        if ((entry.level == tkd::Logger::Level::INFO && !showInfo) ||
            (entry.level == tkd::Logger::Level::WARNING && !showWarning) ||
            (entry.level == tkd::Logger::Level::ERROR && !showError) ||
            (entry.level == tkd::Logger::Level::DEBUG && !showDebug))
            continue;

        ImVec4 color;
        switch (entry.level) {
            case tkd::Logger::Level::INFO:
                color = ImVec4(1.0f, 1.0f, 1.0f,.9f); // White
                break;
            case tkd::Logger::Level::WARNING:
                color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); // Yellow
                break;
            case tkd::Logger::Level::ERROR:
                color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
                break;
            case tkd::Logger::Level::DEBUG:
                color = ImVec4(0.5f, 0.5f, 1.0f, 1.0f); // Light blue
                break;
        }

        ImGui::TextColored(color, "[%s] ", entry.timestamp.c_str());
        ImGui::SameLine();

        switch (entry.level) {
            case tkd::Logger::Level::INFO:
                ImGui::TextColored(color, "[INFO] ");
                break;
            case tkd::Logger::Level::WARNING:
                ImGui::TextColored(color, "[WARNING] ");
                break;
            case tkd::Logger::Level::ERROR:
                ImGui::TextColored(color, "[ERROR] ");
                break;
            case tkd::Logger::Level::DEBUG:
                ImGui::TextColored(color, "[DEBUG] ");
                break;
        }

        ImGui::SameLine();
        ImGui::TextWrapped("%s", entry.message.c_str());
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
void Editor::registerComponentInspectors(void)
{
    ComponentRegistry::instance().registerComponent<Transform>(
        "Transform",
        &TransformInspector
    );

    ComponentRegistry::instance().registerComponent<SpriteRenderer>(
        "Sprite Renderer",
        &SpriteRendererInspector,
        "player.png"
    );

    ComponentRegistry::instance().registerComponent<InputComponent>(
        "Input Component",
        &InputComponentInspector
    );
}

///////////////////////////////////////////////////////////////////////////////
void Editor::TransformInspector(Component* component)
{
    Transform* transform = static_cast<Transform*>(component);

    Vec2f position = transform->getPosition();
    float pos[2] = { position.x, position.y };
    if (ImGui::DragFloat2(ICON_FA_UP_DOWN_LEFT_RIGHT " Position", pos, 1.f)) {
        transform->setPosition({pos[0], pos[1]});
    }

    float rotation = transform->getRotation().asDegrees();
    if (ImGui::DragFloat(ICON_FA_ROTATE " Rotation",
            &rotation, 1.0f, 0.0f, 360.0f)
        ) {
        transform->setRotation(sf::degrees(rotation));
    }

    Vec2f scale = transform->getScale();
    float scl[2] = { scale.x, scale.y };
    if (ImGui::DragFloat2(
        ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER " Scale",
        scl, 0.1f, 0.1f, 10.0f)
    ) {
        transform->setScale({scl[0], scl[1]});
    }
}

///////////////////////////////////////////////////////////////////////////////
void Editor::SpriteRendererInspector(Component* component)
{
    SpriteRenderer* renderer = static_cast<SpriteRenderer*>(component);

    char texturePath[256] = "";
    if (ImGui::InputText(
            "Texture Path",
            texturePath,
            sizeof(texturePath),
            ImGuiInputTextFlags_EnterReturnsTrue)
    ) {
        renderer->loadTexture(texturePath);
    }

    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        renderer->loadTexture(texturePath);
    }

    sf::Color color = renderer->getColor();
    float col[4] = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f
    };

    if (ImGui::ColorEdit4("Color", col)) {
        renderer->setColor(sf::Color(
            static_cast<Uint8>(col[0] * 255),
            static_cast<Uint8>(col[1] * 255),
            static_cast<Uint8>(col[2] * 255),
            static_cast<Uint8>(col[3] * 255)
        ));
    }
}

///////////////////////////////////////////////////////////////////////////////
void Editor::InputComponentInspector(Component* component)
{
    InputComponent* inputComponent = static_cast<InputComponent*>(component);

    float speed = inputComponent->getMoveSpeed();
    if (ImGui::DragFloat("Move Speed", &speed, 1.0f, 1.0f, 1000.0f)) {
        inputComponent->setMoveSpeed(speed);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Editor::render()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar |
                                    ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(
        viewport->WorkPos.x, viewport->WorkPos.y - 20));
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    window_flags |= ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus |
                    ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static bool first_time = true;
        if (first_time)
        {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id,
                dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            ImGuiID dmi = dockspace_id;
            ImGuiID dri = ImGui::DockBuilderSplitNode(
                dmi, ImGuiDir_Right, 0.25f, nullptr, &dmi);
            ImGuiID drti = dri;
            ImGuiID drbi = ImGui::DockBuilderSplitNode(
                dri, ImGuiDir_Down, 0.5f, nullptr, &drti);
            ImGuiID dbi = ImGui::DockBuilderSplitNode(
                dmi, ImGuiDir_Down, 0.25f, nullptr, &dmi);

            ImGui::DockBuilderDockWindow(GAME_VIEW, dmi);
            ImGui::DockBuilderDockWindow(HIERARCHY, drti);
            ImGui::DockBuilderDockWindow(INSPECTOR, drbi);
            ImGui::DockBuilderDockWindow(CONSOLE, dbi);

            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    drawGameView();
    drawHierarchy();
    drawInspector();
    drawConsole();

    ImGui::End();
}

///////////////////////////////////////////////////////////////////////////////
sf::RenderTexture& Editor::getGameViewTexture(void)
{
    return (m_texture);
}

} // namespace tkd::Gui
