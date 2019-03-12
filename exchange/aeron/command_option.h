#ifndef TRADING_PLATFORM_AERON_COMMAND_OPTION_H
#define TRADING_PLATFORM_AERON_COMMAND_OPTION_H

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "util/Exceptions.h"
#include "util/StringUtil.h"

namespace TradingPlatform {
namespace Aeron {

class CommandOption {

public:

    CommandOption()
        : m_optionKey("")
        , m_minParams(0)
        , m_maxParams(0)
        , m_helpText("")
        , m_isPresent(false)
    {
    }

    CommandOption(std::string optionKey, size_t minParams, size_t maxParams, std::string helpText)
        : m_optionKey(std::move(optionKey))
        , m_minParams(minParams)
        , m_maxParams(maxParams)
        , m_helpText(std::move(helpText))
        , m_isPresent(false)
    {
    }

    std::string getOptionKey() const { return m_optionKey; }

    std::string getHelpText() const { return m_helpText; }

    void addParam(std::string p) { m_params.push_back(std::move(p)); }

    void validate() const
    {
        if (!isPresent()) {
            return;
        }

        if (m_params.size() > m_maxParams) {
            throw aeron::util::SourcedException(std::string("option --") + m_optionKey +
                                            " has too many parameters specified.", SOURCEINFO);
        }

        if (m_params.size() < m_minParams) {
            throw aeron::util::SourcedException(std::string("option --") + m_optionKey +
                                            " has too few parameters specified.", SOURCEINFO);
        }
    }

    bool isPresent() const { return m_isPresent; }
    void setPresent() { m_isPresent = true; }

    size_t getNumParams() const { return m_params.size(); }

    std::string getParam(size_t index) const
    {
        checkIndex(index);
        return m_params[index];
    }

    std::string getParam(size_t index, std::string defaultValue) const
    {
        if (!isPresent())
            return defaultValue;
        return getParam(index);
    }

    int getParamAsInt(size_t index) const
    {
        checkIndex(index);
        std::string param = m_params[index];
        return aeron::util::parse<int>(param);
    }

    long getParamAsLong(size_t index) const
    {
        checkIndex(index);
        std::string param = m_params[index];
        return aeron::util::parse<long>(param);
    }

    int getParamAsInt(size_t index, int minValue, int maxValue, int defaultValue) const
    {
        if (!isPresent()) {
            return defaultValue;
        }

        int value = getParamAsInt(index);
        if (value < minValue || value > maxValue)
        {
            throw aeron::util::SourcedException(std::string("value \"") + aeron::util::toString(value) +
                                            "\" out of range: [" + aeron::util::toString(minValue) +
                                            ".." + aeron::util::toString(maxValue) +
                                            "] on option --" + m_optionKey, SOURCEINFO);
        }

        return value;
    }

    long getParamAsLong(size_t index, long minValue, long maxValue, long defaultValue) const
    {
        if (!isPresent())
            return defaultValue;

        long value = getParamAsLong(index);
        if (value < minValue || value > maxValue)
        {
            throw aeron::util::SourcedException(std::string("value \"") + aeron::util::toString(value) +
                                            "\" out of range: [" + aeron::util::toString(minValue) +
                                            ".." + aeron::util::toString(maxValue) +
                                            "] on option --" + m_optionKey, SOURCEINFO);
        }

        return value;
    }

private:

    void checkIndex(size_t index) const
    {
        if (index > m_params.size())
            throw aeron::util::SourcedException(std::string("Internal Error: index out of range for option: ") + m_optionKey, SOURCEINFO);
    }

private:

    std::string m_optionKey;
    size_t m_minParams;
    size_t m_maxParams;
    std::string m_helpText;

    bool m_isPresent;

    std::vector<std::string> m_params;
};

} // namespace Aeron
} // namespace TradingPlatform

#endif // TRADING_PLATFORM_AERON_COMMAND_OPTION_H