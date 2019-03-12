/*!
    \file errors.h
    \brief Errors definition
    \author Ivan Shynkarenka
    \date 16.08.2017
    \copyright MIT License
*/

#ifndef TRADING_PLATFORM_MATCHING_ERRORS_H
#define TRADING_PLATFORM_MATCHING_ERRORS_H

#include "utility/iostream.h"

#include <cstdint>

namespace TradingPlatform {
namespace Matching {

//! Error code
enum class ErrorCode : uint8_t
{
    OK,
    SYMBOL_DUPLICATE,
    SYMBOL_NOT_FOUND,
    ORDER_BOOK_DUPLICATE,
    ORDER_BOOK_NOT_FOUND,
    ORDER_DUPLICATE,
    ORDER_NOT_FOUND,
    ORDER_ID_INVALID,
    ORDER_TYPE_INVALID,
    ORDER_PARAMETER_INVALID,
    ORDER_QUANTITY_INVALID
};
template <class TOutputStream>
TOutputStream& operator<<(TOutputStream& stream, ErrorCode error);

} // namespace Matching
} // namespace TradingPlatform

#include "errors.inl"

#endif // TRADING_PLATFORM_MATCHING_ERRORS_H
