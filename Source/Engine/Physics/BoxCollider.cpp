///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Physics/BoxCollider.hpp>
#include <Engine/Physics/CircleCollider.hpp>
#include <Engine/Physics/Physics.hpp>
#include <Engine/Core/Transform.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
bool BoxCollider::intersectsWithBox(const BoxCollider* other) const
{
    Bounds b1 = getBounds();
    Bounds b2 = other->getBounds();

    return (
        b1.min().x <= b2.max().x &&
        b1.max().x >= b2.min().x &&
        b1.min().y <= b2.min().y &&
        b1.max().y >= b2.min().y
    );
}

///////////////////////////////////////////////////////////////////////////////
bool BoxCollider::intersectsWithCircle(const CircleCollider* other) const
{
    Bounds bound = getBounds();
    Vec2f mn = bound.min();
    Vec2f mx = bound.max();

    Vec2f center = other->getCenter();
    float radius = other->getRadius();

    Vec2f closest(
        std::max(mn.x, std::min(center.x, mx.x)),
        std::max(mn.y, std::min(center.y, mx.y))
    );

    float dist = (closest - center).squaredLength();

    return (dist <= (radius * radius));
}

///////////////////////////////////////////////////////////////////////////////
Vec2f BoxCollider::getSize(void) const
{
    return (m_size);
}

///////////////////////////////////////////////////////////////////////////////
Vec2f BoxCollider::getOffset(void) const
{
    return (m_offset);
}

///////////////////////////////////////////////////////////////////////////////
void BoxCollider::setSize(const Vec2f& size)
{
    m_size = size;
}

///////////////////////////////////////////////////////////////////////////////
void BoxCollider::setOffset(const Vec2f& offset)
{
    m_offset = offset;
}

///////////////////////////////////////////////////////////////////////////////
Bounds BoxCollider::getBounds(void) const
{
    Vec2f worldPos = m_offset;
    Vec2f scale(1.f);

    if (auto transform = getGameObject()->getComponent<Transform>()) {
        scale = transform->getScale();
        worldPos = transform->transformPoint(worldPos);
    }

    return (Bounds(worldPos, m_size * scale));
}

} // namespace tkd
