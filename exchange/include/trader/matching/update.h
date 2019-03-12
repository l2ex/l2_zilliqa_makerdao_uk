/*!
    \file update.h
    \brief Update definition
    \author Ivan Shynkarenka
    \date 09.08.2017
    \copyright MIT License
*/

#ifndef TRADING_PLATFORM_MATCHING_UPDATE_H
#define TRADING_PLATFORM_MATCHING_UPDATE_H

#include "utility/iostream.h"

#include <cstdint>

namespace TradingPlatform {
namespace Matching {

//! Update type
enum class UpdateType : uint8_t
{
    NONE,
    ADD,
    UPDATE,
    DELETE
};
template <class TOutputStream>
TOutputStream& operator<<(TOutputStream& stream, UpdateType type);

} // namespace Matching
} // namespace TradingPlatform

#include "update.inl"

#endif // TRADING_PLATFORM_MATCHING_UPDATE_H
