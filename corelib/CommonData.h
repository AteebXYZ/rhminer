/*

This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file CommonData.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 * @author Polyminer1 <https://github.com/polyminer1>
 * @date 2018
 *
 * Shared algorithms and data types.
 */

#pragma once

#include <unordered_set>
#include <type_traits>
#include "Common.h"

extern bool g_ExitApplication;
#define RHMINER_RETURN_ON_EXIT_FLAG() if (g_ExitApplication) \
    { \
        RH_SetThreadPriority(RH_ThreadPrio_Normal); \
        return; \
    }

#define RHMINER_RETURN_ON_EXIT_FLAG_EX(RET_VAL) if (g_ExitApplication) \
    { \
        RH_SetThreadPriority(RH_ThreadPrio_Normal); \
        return RET_VAL; \
    }

// String conversion functions, mainly to/from hex/nibble/byte representations.

enum class WhenError
{
	DontThrow = 0,
	Throw = 1,
};

enum class HexPrefix
{
	DontAdd = 0,
	Add = 1,
};
/// Convert a series of bytes to the corresponding string of hex duplets.
/// @param _w specifies the width of the first of the elements. Defaults to two - enough to represent a byte.
/// @example toHex("A\x69") == "4169"
template <class T>
std::string toHex(vector_ref<T> const& _data, int _w = 2, HexPrefix _prefix = HexPrefix::DontAdd)
{
	std::ostringstream ret;
	unsigned ii = 0;
	for (auto i: _data)
		ret << std::hex << std::setfill('0') << std::setw(ii++ ? 2 : _w) << (int)(typename std::make_unsigned<decltype(i)>::type)i;
	return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

template <class T>
std::string toHex(std::vector<T> const& _data, int _w = 2, HexPrefix _prefix = HexPrefix::DontAdd)
{
    std::ostringstream ret;
    unsigned ii = 0;
    for (auto i : _data)
        ret << std::hex << std::setfill('0') << std::setw(ii++ ? 2 : _w) << (int)(typename std::make_unsigned<decltype(i)>::type)i;
    return (_prefix == HexPrefix::Add) ? "0x" + ret.str() : ret.str();
}

/// Converts a (printable) ASCII hex character into the correspnding integer value.
/// @example fromHex('A') == 10 && fromHex('f') == 15 && fromHex('5') == 5
int fromHex(char _i, WhenError _throw);

/// Converts a (printable) ASCII hex string into the corresponding byte stream.
/// @example fromHex("41626261") == asBytes("Abba")
/// If _throw = ThrowType::DontThrow, it replaces bad hex characters with 0's, otherwise it will throw an exception.
bytes fromHex(std::string const& _s, WhenError _throw = WhenError::DontThrow);

/// Converts byte array to a string containing the same (binary) data. Unless
/// the byte array happens to contain ASCII data, this won't be printable.
inline std::string asString(bytes const& _b)
{
	return std::string((char const*)_b.data(), (char const*)(_b.data() + _b.size()));
}

/// Converts a string to a byte array containing the string's (byte) data.
inline bytes asBytes(std::string const& _b)
{
	return bytes((byte const*)_b.data(), (byte const*)(_b.data() + _b.size()));
}


// Big-endian to/from host endian conversion functions.

/// Converts a templated integer value to the big-endian byte-stream represented on a templated collection.
/// The size of the collection object will be unchanged. If it is too small, it will not represent the
/// value properly, if too big then the additional elements will be zeroed out.
/// @a Out will typically be either std::string or bytes.
/// @a T will typically by unsigned, u160, u256 or bigint.
template <class T, class Out>
inline void toBigEndian(T _val, Out& o_out)
{
	static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed, "only unsigned types or bigint supported"); //bigint does not carry sign bit on shift
	for (auto i = o_out.size(); i != 0; _val >>= 8, i--)
	{
		T v = _val & (T)0xff;
		o_out[i - 1] = (typename Out::value_type)(uint8_t)v;
	}
}

/// Converts a big-endian byte-stream represented on a templated collection to a templated integer value.
/// @a _In will typically be either std::string or bytes.
/// @a T will typically by unsigned, u160, u256 or bigint.
template <class T, class _In>
inline T fromBigEndian(_In const& _bytes)
{
	T ret = (T)0;
	for (auto i: _bytes)
		ret = (T)((ret << 8) | (byte)(typename std::make_unsigned<typename _In::value_type>::type)i);
	return ret;
}

/// Convenience functions for toBigEndian
inline bytes toBigEndian(u256 _val) { bytes ret(32); toBigEndian(_val, ret); return ret; }
inline bytes toBigEndian(u160 _val) { bytes ret(20); toBigEndian(_val, ret); return ret; }

/// Convenience function for toBigEndian.
/// @returns a byte array just big enough to represent @a _val.
template <class T>
inline bytes toCompactBigEndian(T _val, unsigned _min = 0)
{
	static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed, "only unsigned types or bigint supported"); //bigint does not carry sign bit on shift
	int i = 0;
	for (T v = _val; v; ++i, v >>= 8) {}
	bytes ret(std::max<unsigned>(_min, i), 0);
	toBigEndian(_val, ret);
	return ret;
}

/// Convenience function for conversion of a u256 to hex
inline std::string toHexBE(u256 val, HexPrefix prefix = HexPrefix::DontAdd)
{
	std::string str = toHex(toBigEndian(val));
	return (prefix == HexPrefix::Add) ? "0x" + str : str;
}

inline std::string toHex(uint64_t _n)
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(sizeof(_n) * 2) << _n;
    return ss.str();
}

inline std::string toHex(uint32_t _n)
{
    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(sizeof(_n) * 2) << _n;
    return ss.str();
}

extern std::string toHex(void* p, size_t len, bool withEndl = true);
// Algorithms for string and string-like collections.

/// Escapes a string into the C-string representation.
/// @p _all if true will escape all characters, not just the unprintable ones.
std::string escaped(std::string const& _s, bool _all = true);

// General datatype convenience functions.

/// Determine bytes required to encode the given integer value. @returns 0 if @a _i is zero.
template <class T>
inline unsigned bytesRequired(T _i)
{
	static_assert(std::is_same<bigint, T>::value || !std::numeric_limits<T>::is_signed, "only unsigned types or bigint supported"); //bigint does not carry sign bit on shift
	unsigned i = 0;
	for (; _i != 0; ++i, _i >>= 8) {}
	return i;
}

/// Sets enviromental variable.
///
/// Portable wrapper for setenv / _putenv C library functions.
bool setenv(const char name[], const char value[], bool override = false);


inline string stringToJS(uint32_t _n)
{
    string h = toHex(toCompactBigEndian(_n, 1));
    // remove first 0, if it is necessary;
    std::string res = h[0] != '0' ? h : h.substr(1);
    return "0x" + res;
}


inline string stringToJS(uint64_t _n)
{
    string h = toHex(toCompactBigEndian(_n, 1));
    // remove first 0, if it is necessary;
    std::string res = h[0] != '0' ? h : h.substr(1);
    return "0x" + res;
}

