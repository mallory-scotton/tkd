///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Utils/Types.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Instantiable
{
protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Instantiable() {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual ~Instantiable() = default;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Instantiable(const Instantiable&) = delete;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Instantiable& operator=(const Instantiable&) = delete;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class Singleton : public Instantiable
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    static UPtr<T> m_instance;      //<!
    static Mutex m_mutex;           //<!

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    Singleton(void) {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static T* getInstance(void)
    {
        std::lock_guard<Mutex> lock(m_mutex);
        if (!m_instance)
        {
            m_instance.reset(new T());
        }
        return (m_instance.get());
    }
};

///////////////////////////////////////////////////////////////////////////////
template <typename T>
UPtr<T> Singleton<T>::m_instance = nullptr;

///////////////////////////////////////////////////////////////////////////////
template <typename T>
Mutex Singleton<T>::m_mutex;

} // namespace tkd
