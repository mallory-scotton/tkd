///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Utils/Types.hpp>
#include <Engine/Utils/Singleton.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Logger
{
public:
    enum class Level
    {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    struct Entry
    {
    public:
        String message;
        Level level;
        String timestamp;

    public:
        Entry(const String& message, Level level)
            : message(message)
            , level(level)
        {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);

            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%H:%M:%S");
            timestamp = ss.str();
        }
    };

private:
    static Vector<Entry> m_entries;
    static Mutex m_mutex;

public:
    static void log(const String& message, Level level = Level::INFO);

    static void info(const String& message);

    static void warning(const String& message);

    static void error(const String& message);

    static void debug(const String& message);

    static void clear(void);

    static const Vector<Entry>& getEntries(void);
};

} // namespace tkd
