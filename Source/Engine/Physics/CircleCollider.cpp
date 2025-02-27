///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Physics/CircleCollider.hpp>
#include <Engine/Physics/BoxCollider.hpp>
#include <Engine/Physics/Physics.hpp>
#include <Engine/Core/Transform.hpp>
#include <Engine/Core/Transform.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
bool CircleCollider::intersectsWithBox(const BoxCollider* other) const
{
    Bounds bound = other->getBounds();
    Vec2f center = getCenter();

    Vec2f closest(
        std::max(bound.min().x, std::min(center.x, bound.max().x)),
        std::max(bound.min().y, std::min(center.y, bound.max().y))
    );

    Vec2f scale(1.f);

    if (auto transform = getGameObject()->getComponent<Transform>()) {
        scale = transform->getScale();
    }

    float dist = (closest - center).squaredLength();
    float radius = m_radius * scale.getMax();

    return (dist <= (radius * radius));
}

///////////////////////////////////////////////////////////////////////////////
bool CircleCollider::intersectsWithCircle(const CircleCollider* other) const
{
    Vec2f c1 = getCenter();
    Vec2f c2 = other->getCenter();
    Vec2f s1 = 1.f;
    Vec2f s2 = 2.f;

    if (auto transform = getGameObject()->getComponent<Transform>()) {
        s1 = transform->getScale();
    }

    if (auto transform = other->getGameObject()->getComponent<Transform>()) {
        s2 = transform->getScale();
    }

    float r1 = m_radius * s1.getMax();
    float r2 = other->getRadius() * s2.getMax();
    float dist = (c2 - c1).squaredLength();
    float rsum = r1 + r2;

    return (dist <= (rsum * rsum));
}

///////////////////////////////////////////////////////////////////////////////
Vec2f CircleCollider::getCenter(void) const
{
    Vec2f center = m_offset;

    if (auto transform = getGameObject()->getComponent<Transform>()) {
        center = transform->transformPoint(center);
    }

    return (center);
}

///////////////////////////////////////////////////////////////////////////////
Vec2f CircleCollider::getOffset(void) const
{
    return (m_offset);
}

///////////////////////////////////////////////////////////////////////////////
float CircleCollider::getRadius(void) const
{
    return (m_radius);
}

///////////////////////////////////////////////////////////////////////////////
void CircleCollider::setRadius(float radius)
{
    m_radius = radius;
}

///////////////////////////////////////////////////////////////////////////////
void CircleCollider::setOffset(const Vec2f& offset)
{
    m_offset = offset;
}

///////////////////////////////////////////////////////////////////////////////
Bounds CircleCollider::getBounds(void) const
{
    Vec2f worldPos = m_offset;
    float radius = m_radius;

    if (auto transform = getGameObject()->getComponent<Transform>()) {
        worldPos = transform->transformPoint(worldPos);
        radius *= transform->getScale().getMax();
    }

    return (Bounds(worldPos, Vec2f(radius * 2.f)));
}

} // namespace tkd
