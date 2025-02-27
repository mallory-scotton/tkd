///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include <Engine/Utils/Logger.hpp>

///////////////////////////////////////////////////////////////////////////////
// Namespace tkd
///////////////////////////////////////////////////////////////////////////////
namespace tkd
{

///////////////////////////////////////////////////////////////////////////////
Vector<Logger::Entry> Logger::m_entries;

///////////////////////////////////////////////////////////////////////////////
Mutex Logger::m_mutex;

///////////////////////////////////////////////////////////////////////////////
void Logger::log(const String& message, Logger::Level level)
{
    std::lock_guard<Mutex> lock(Logger::m_mutex);

    static const Uint64 MAX_LOG_ENTRIES = 512;

    if (Logger::m_entries.size() >= MAX_LOG_ENTRIES) {
        Logger::m_entries.erase(Logger::m_entries.begin());
    }

    Logger::m_entries.emplace_back(message, level);
}

///////////////////////////////////////////////////////////////////////////////
void Logger::info(const String& message)
{
    Logger::log(message, Level::INFO);
}

///////////////////////////////////////////////////////////////////////////////
void Logger::warning(const String& message)
{
    Logger::log(message, Level::WARNING);
}

///////////////////////////////////////////////////////////////////////////////
void Logger::error(const String& message)
{
    Logger::log(message, Level::ERROR);
}

///////////////////////////////////////////////////////////////////////////////
void Logger::debug(const String& message)
{
    Logger::log(message, Level::DEBUG);
}

///////////////////////////////////////////////////////////////////////////////
void Logger::clear(void)
{
    std::lock_guard<Mutex> lock(Logger::m_mutex);
    Logger::m_entries.clear();
}

///////////////////////////////////////////////////////////////////////////////
const Vector<Logger::Entry>& Logger::getEntries(void)
{
    return (Logger::m_entries);
}

} // namespace tkd
