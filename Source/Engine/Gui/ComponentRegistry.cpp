///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Gui/ComponentRegistry.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd::Gui
///////////////////////////////////////////////////////////////////////////////
namespace tkd::Gui
{

///////////////////////////////////////////////////////////////////////////////
const Vector<String> ComponentRegistry::getComponentNames(void) const
{
    Vector<String> names;

    for (const auto& [index, info] : m_components) {
        names.push_back(info.name);
    }
    return (names);
}

///////////////////////////////////////////////////////////////////////////////
Component* ComponentRegistry::createComponent(
    const String& name,
    GameObject* object
)
{
    for (const auto& [index, info] : m_components) {
        if (info.name == name) {
            return (info.creator(object));
        }
    }
    return (nullptr);
}

///////////////////////////////////////////////////////////////////////////////
void ComponentRegistry::inspectComponent(Component* component)
{
    auto it = m_components.find(std::type_index(typeid(*component)));
    if (it != m_components.end()) {
        it->second.inspector(component);
    }
}

///////////////////////////////////////////////////////////////////////////////
bool ComponentRegistry::hasInspector(Component* component)
{
    auto it = m_components.find(std::type_index(typeid(*component)));
    return it != m_components.end();
}

///////////////////////////////////////////////////////////////////////////////
ComponentRegistry& ComponentRegistry::instance(void)
{
    static ComponentRegistry instance;
    return (instance);
}

} // namespace tkd::Gui
