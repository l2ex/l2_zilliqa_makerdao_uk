#ifndef TRADING_PLATFORM_THREAD_H
#define TRADING_PLATFORM_THREAD_H

#include <thread>

namespace TradingPlatform {

class Thread : public std::thread
{
public:

    enum class Priority : std::uint8_t
    {
        Idle,
        Lowest,
        BelowNormal,
        Normal,
        AboveNormal,
        Highest,
        TimeCritical,	// be care with using this!
    };

    template<class Function, class... Args>
    Thread(Function&& func, Args&&... args)
        : thread(func, args...)
    {
    }

    const std::string& getName() const { return m_name; }

    void setName(const std::string& name)
    {
#if defined(_WIN32)
#pragma pack(push, 8)
        using THREADNAME_INFO = struct tagTHREADNAME_INFO
        {
            DWORD dwType;     // Must be 0x1000.
            LPCSTR szName;    // Pointer to name (in user addr space).
            DWORD dwThreadID; // Thread ID (-1=caller thread).
            DWORD dwFlags;    // Reserved for future use, must be zero.
        };
#pragma pack(pop)

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.getUTF8().c_str();
#pragma warning(push)
        info.dwThreadID = ::GetThreadId(static_cast<HANDLE>(native_handle()));
#pragma warning(pop)
        info.dwFlags = 0;

        __try
        {
            static constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), reinterpret_cast<ULONG_PTR*>(&info));
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
        }
#elif defined(__APPLE__)
        if (pthread_self() == native_handle())
            pthread_setname_np(name.c_str());
#endif

        m_name = name;
    }

    size_t getIndex() const { return m_index; }

    void setIndex(size_t index) { m_index = index; }

    Priority getPriority() const { return m_priority; }

    void setPriority(Priority priority)
    {
        setThreadPriority(native_handle(), priority);
        m_priority = priority;
    }

    /** Returns true in case when this method is called on it's own thread. */
    bool isCurrent() const { return get_id() == std::this_thread::get_id(); }

public:

    static id getCurrentThreadId()
    {
        return std::this_thread::get_id();
    }

    static native_handle_type getCurrentThreadNativeId()
    {
#ifdef _WIN32
        return ::GetCurrentThread();
#else
        return native_handle_type();
#endif
    }

    static void setThreadPriority(native_handle_type threadHandle, Priority priority)
    {
        if (!threadHandle)
            return;

#ifdef _WIN32
        int nativePriority = THREAD_PRIORITY_NORMAL;
        switch (priority)
        {
        case Priority::Idle:
            nativePriority = THREAD_PRIORITY_IDLE;
            break;
        case Priority::Lowest:
            nativePriority = THREAD_PRIORITY_LOWEST;
            break;
        case Priority::BelowNormal:
            nativePriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case Priority::Normal:
            nativePriority = THREAD_PRIORITY_NORMAL;
            break;
        case Priority::AboveNormal:
            nativePriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case Priority::Highest:
            nativePriority = THREAD_PRIORITY_HIGHEST;
            break;
        case Priority::TimeCritical:
            nativePriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
        default:
            nativePriority = THREAD_PRIORITY_NORMAL;
        }
        SetThreadPriority(threadHandle, nativePriority);
#endif
    }

private:

    // Thread name helps a lot during debugging
    std::string m_name;

    // Thread index in a thread pool (used by ThreadPool)
    size_t m_index = 0;

    // Thread priority
    Priority m_priority = Priority::Normal;
};

}

#endif // TRADING_PLATFORM_THREAD_H
