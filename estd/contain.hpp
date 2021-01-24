///
/// estd.hpp
/// estd
///
/// Purpose:
/// Define map and set utility functions
///

#ifndef PKG_ESTD_HASH_HPP
#define PKG_ESTD_HASH_HPP

#include "logs/logs.hpp"

namespace estd
{

/// Hasher useful for satisfying old GCC constraint where enum is not hashable
struct EnumHash
{
	template <typename T>
	size_t operator() (T e) const
	{
		return static_cast<size_t>(e);
	}
};

template <typename MAPPABLE>
using KeyT = typename MAPPABLE::key_type;

template <typename MAPPABLE>
using ValT = typename MAPPABLE::mapped_type;

template <typename MAPPABLE>
std::vector<KeyT<MAPPABLE>> get_keys (const MAPPABLE& m)
{
	std::vector<KeyT<MAPPABLE>> keys;
	keys.reserve(m.size());
	for (const auto& e : m)
	{
		keys.push_back(e.first);
	}
	return keys;
}

/// Return true if key is found in map/set s
template <typename SEARCHABLE, typename KEYTYPE>
bool has (const SEARCHABLE& s, const KEYTYPE& key)
{
	return s.end() != s.find(key);
}

/// Return true if key is found in map s, and assign mapped value to val
template <typename MAPPABLE, typename KEYTYPE>
bool get (ValT<MAPPABLE>& val, const MAPPABLE& s, const KEYTYPE& key)
{
	auto it = s.find(key);
	bool found = s.end() != it;
	if (found)
	{
		val = it->second;
	}
	return found;
}

/// Return value mapped by key in s if key exists in s
/// otherwise return default_val
template <typename MAPPABLE, typename KEYTYPE>
ValT<MAPPABLE> try_get (const MAPPABLE& s, const KEYTYPE& key,
	ValT<MAPPABLE> default_val)
{
	auto it = s.find(key);
	if (s.end() != it)
	{
		return it->second;
	}
	return default_val;
}

/// Return value mapped by key in s if key exists in s
/// otherwise fatal log (which throws runtime error by default)
template <typename MAPPABLE, typename KEYTYPE, typename... ARGS>
const ValT<MAPPABLE>& must_getf (
	const MAPPABLE& s, const KEYTYPE& key,
	std::string msg, ARGS... args)
{
	auto it = s.find(key);
	if (s.end() == it)
	{
		logs::fatalf(msg, args...);
	}
	return it->second;
}

/// Return true if key is found in list/vector/array s
template <typename ARR, typename CONTENT>
bool arr_has (const ARR& s, const CONTENT& key)
{
	auto et = s.end();
	return et != std::find(s.begin(), et, key);
}

/// Return unordered set of map
template <typename KEYTYPE, typename VALTYPE>
std::unordered_set<KEYTYPE> map_keyset (
	const std::unordered_map<KEYTYPE,VALTYPE>& kv)
{
	std::unordered_set<KEYTYPE> k;
	for (auto& p : kv)
	{
		k.emplace(p.first);
	}
	return k;
}

}

#endif // PKG_ESTD_HASH_HPP
