
#ifndef PKG_ESTD_HASHLIST_HPP
#define PKG_ESTD_HASHLIST_HPP

#include <list>

#include "estd/contain.hpp"

namespace estd
{

template <typename T, typename HASH = std::hash<T>>
struct HashList final
{
	using ListT = std::list<T>;

	using IterT = typename ListT::iterator;

	using CstIterT = typename ListT::const_iterator;

	using RevIterT = typename ListT::reverse_iterator;

	using CstRevIterT = typename ListT::const_reverse_iterator;

	HashList (void) = default;

	template <typename IteratorT>
	HashList (IteratorT begin, IteratorT end)
	{
		for (; begin != end; ++begin)
		{
			push_back(*begin);
		}
	}

	IterT begin (void)
	{
		return lst_.begin();
	}

	IterT end (void)
	{
		return lst_.end();
	}

	CstIterT cbegin (void) const
	{
		return lst_.cbegin();
	}

	CstIterT cend (void) const
	{
		return lst_.cend();
	}

	RevIterT rbegin (void)
	{
		return lst_.rbegin();
	}

	RevIterT rend (void)
	{
		return lst_.rend();
	}

	CstRevIterT crbegin (void) const
	{
		return lst_.crbegin();
	}

	CstRevIterT crend (void) const
	{
		return lst_.crend();
	}

	size_t size (void) const
	{
		return lst_.size();
	}

	bool empty (void) const
	{
		return lst_.empty();
	}

	T& front (void)
	{
		return lst_.front();
	}

	const T& front (void) const
	{
		return lst_.front();
	}

	T& back (void)
	{
		return lst_.back();
	}

	const T& back (void) const
	{
		return lst_.back();
	}

	IterT push_back (const T& val)
	{
		if (estd::has(umap_, val))
		{
			return umap_.at(val);
		}
		lst_.push_back(val);
		auto position = lst_.end();
		--position;
		umap_[val] = position;
		return position;
	}

	void pop_back (void)
	{
		if (lst_.empty())
		{
			return;
		}
		umap_.erase(back());
		lst_.pop_back();
	}

	IterT insert (IterT position, const T& val)
	{
		if (estd::has(umap_, val))
		{
			return umap_.at(val);
		}
		position = lst_.insert(position, val);
		umap_[val] = position;
		return position;
	}

	void erase (const T& val)
	{
		if (false == estd::has(umap_, val))
		{
			return;
		}
		lst_.erase(umap_[val]);
		umap_.erase(val);
	}

	void clear (void)
	{
		umap_.clear();
		lst_.clear();
	}

private:
	ListT lst_;

	std::unordered_map<T,IterT,HASH> umap_;
};

}

#endif // PKG_ESTD_HASHLIST_HPP
