///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Core/Component.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
void Component::awake(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Component::start(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Component::update(float deltaTime)
{
    UNUSED(deltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void Component::fixedUpdate(float fixedDeltaTime)
{
    UNUSED(fixedDeltaTime);
}

///////////////////////////////////////////////////////////////////////////////
void Component::onDestroy(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Component::setEnabled(bool value)
{
    m_enabled = value;
}

///////////////////////////////////////////////////////////////////////////////
bool Component::isEnabled(void) const
{
    return (m_enabled);
}

///////////////////////////////////////////////////////////////////////////////
void Component::setGameObject(GameObject* object)
{
    m_gameObject = object;
}

///////////////////////////////////////////////////////////////////////////////
GameObject* Component::getGameObject(void) const
{
    return (m_gameObject);
}

///////////////////////////////////////////////////////////////////////////////
void Component::onCollisionEnter(GameObject* other)
{
    UNUSED(other);
}

///////////////////////////////////////////////////////////////////////////////
void Component::onCollisionExit(GameObject* other)
{
    UNUSED(other);
}

///////////////////////////////////////////////////////////////////////////////
void Component::onEnable(void)
{}

///////////////////////////////////////////////////////////////////////////////
void Component::onDisable(void)
{}

} // namespace tkd
