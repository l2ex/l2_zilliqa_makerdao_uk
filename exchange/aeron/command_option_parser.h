/*
 * Copyright 2014-2019 Real Logic Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TRADING_PLATFORM_AERON_COMMAND_OPTION_PARSER_H
#define TRADING_PLATFORM_AERON_COMMAND_OPTION_PARSER_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include "command_option.h"

namespace TradingPlatform {
namespace Aeron {

class CommandOptionParser {

public:

    CommandOptionParser()
    {
        addOption(CommandOption("", 0, 0, "Unnamed Options"));
    }

    void parse(int argc, char **argv)
    {
        std::string currentOption;
        getOption(currentOption).setPresent();
        for (int n = 1; n < argc; n++)
        {
            std::string argStr(argv[n]);
            if ((argStr.size() >= 3) && (argStr[0] == '-' && argStr[1] == '-'))
            {
                currentOption = &argStr[2];
                auto option = m_options.find(currentOption);
                if (option != m_options.end())
                    option->second.setPresent();
                else
                    currentOption = "";
            }
            else if (!currentOption.empty())
            {
                CommandOption &option = getOption(currentOption);
                option.addParam(argStr);
            }
        }

        for (auto option = m_options.begin(); option != m_options.end(); ++option)
        {
            option->second.validate();
        }
    }

    void addOption(const CommandOption &option)
    {
        m_options[option.getOptionKey()] = option;
    }

    CommandOption &getOption(const std::string &optionKey)
    {
        auto option = m_options.find(optionKey);
        if (option == m_options.end())
            throw aeron::util::SourcedException(std::string("CommandOptionParser::getOption invalid option lookup: ") + optionKey, SOURCEINFO);
        return option->second;
    }

    void displayOptionsHelp(std::ostream &out) const
    {
        for (auto option = m_options.begin(); option != m_options.end(); ++option)
        {
            if (!option->first.empty())
            {
                out << "    --" << option->first << " " << option->second.getHelpText() << std::endl;
            }
        }
    }

private:

    std::unordered_map<std::string, CommandOption> m_options;
};

} // namespace Aeron
} // namespace TradingPlatform

#endif // TRADING_PLATFORM_AERON_COMMAND_OPTION_PARSER_H