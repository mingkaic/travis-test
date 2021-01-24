///
/// myers_diff.hpp
/// diff
///
/// Purpose:
/// Define and implement myers diff algorithm
///
/// all implementations are based on the tutorial outlined in
/// https://blog.jcoglan.com/2017/02/12/the-myers-diff-algorithm-part-1/
///

#ifndef PKG_DIFF_MYERS_HPP
#define PKG_DIFF_MYERS_HPP

#include <cassert>
#include <utility>
#include <iterator>
#include <vector>

namespace diff
{

using IndexT = int16_t;

using PointT = std::pair<IndexT,IndexT>;

template <typename ARR>
using ArrValT = typename std::iterator_traits<
	typename ARR::iterator>::value_type;

/// Return the minimum number of edits between orig and updated arrays
template <typename ARR, typename COMPARATOR=std::equal_to<ArrValT<ARR>>>
size_t myers_diff_min_edit (const ARR& orig, const ARR& updated)
{
	COMPARATOR comparator;
	auto orig_begin = std::begin(orig);
	auto updated_begin = std::begin(updated);
	IndexT n = std::distance(orig_begin, std::end(orig));
	IndexT m = std::distance(updated_begin, std::end(updated));
	IndexT max_edit = n + m;

	IndexT x, y;
	size_t ncost = 2 * max_edit + 1;
	std::vector<IndexT> costs(ncost, 0);
	bool cont = true;
	IndexT min_edits = max_edit;
	for (IndexT iedit = 0; iedit < max_edit && cont; ++iedit)
	{
		for (IndexT k = -iedit; k <= iedit && cont; k += 2)
		{
			IndexT prevk = costs[(ncost + k - 1) % ncost];
			IndexT nextk = costs[(ncost + k + 1) % ncost];
			if (k == -iedit || (k != iedit && prevk < nextk))
			{
				x = nextk;
			}
			else
			{
				x = prevk + 1;
			}

			y = x - k;

			while (x < n && y < m && comparator(
				*std::next(orig_begin, x), *std::next(updated_begin, y)))
			{
				++x;
				++y;
			}

			costs[(ncost + k) % ncost] = x;

			if (x >= n && y >= m)
			{
				min_edits = iedit;
				cont = false;
			}
		}
	}
	return min_edits;
}

/// Return the diff trace, the flattened representation of 2-D array
/// <2 * (orig.size + updated.size) + 1, min_edits>
/// each row in the trace matrix are the minimum costs at every step
/// each row's origin starts at index orig.size + updated.size
/// zeros in each row represent unreachable states at that step
/// This function is a helper function for myers_diff, but can use in diagnosis
template <typename ARR, typename COMPARATOR=std::equal_to<ArrValT<ARR>>>
std::vector<IndexT> myers_diff_trace (ARR orig, ARR updated)
{
	COMPARATOR comparator;
	auto orig_begin = std::begin(orig);
	auto updated_begin = std::begin(updated);
	IndexT n = std::distance(orig_begin, std::end(orig));
	IndexT m = std::distance(updated_begin, std::end(updated));
	IndexT max_edit = n + m;

	IndexT x, y;
	size_t ncost = 2 * max_edit + 1;
	std::vector<IndexT> costs(ncost, 0);
	std::vector<IndexT> trace;
	bool cont = true;
	for (IndexT iedit = 0; iedit <= max_edit && cont; ++iedit)
	{
		trace.insert(trace.end(), costs.begin(), costs.end());
		for (IndexT k = -iedit; k <= iedit && cont; k += 2)
		{
			IndexT prevk = costs[(ncost + k - 1) % ncost];
			IndexT nextk = costs[(ncost + k + 1) % ncost];
			if (k == -iedit || (k != iedit && prevk < nextk))
			{
				x = nextk;
			}
			else
			{
				x = prevk + 1;
			}

			y = x - k;

			while (x < n && y < m && comparator(
				*std::next(orig_begin, x), *std::next(updated_begin, y)))
			{
				++x;
				++y;
			}

			costs[(ncost + k) % ncost] = x;
			cont = x < n || y < m;
		}
	}
	return trace;
}

/// Returns a vector of points (X being orig index and Y being updated index)
/// These points represent the minimum cost diffs
/// This function is a helper function for myers_diff, but can use in diagnosis
template <typename ARR, typename COMPARATOR=std::equal_to<ArrValT<ARR>>>
std::vector<PointT> myers_diff_backtrace (ARR orig, ARR updated)
{
	IndexT x = std::distance(std::begin(orig), std::end(orig));
	IndexT y = std::distance(std::begin(updated), std::end(updated));
	IndexT ncost = 2 * (x + y) + 1;
	IndexT prev_x, prev_y, prev_k;

	auto traces = myers_diff_trace<ARR,COMPARATOR>(orig, updated);

	std::vector<PointT> points;
	IndexT ntraces = traces.size() / ncost;
	for (IndexT iedit = ntraces - 1; iedit >= 0; --iedit)
	{
		IndexT k = x - y;
		if (k == -iedit || (k != iedit &&
			traces[iedit * ncost + (ncost + k - 1) % ncost] <
			traces[iedit * ncost + (ncost + k + 1) % ncost]))
		{
			prev_k = k + 1;
		}
		else
		{
			prev_k = k - 1;
		}

		prev_x = traces[iedit * ncost + (ncost + prev_k) % ncost];
		prev_y = prev_x - prev_k;

		while (x > prev_x && y > prev_y)
		{
			points.push_back({--x, --y});
		}

		if (iedit > 0)
		{
			points.push_back({prev_x, prev_y});
		}

		x = prev_x;
		y = prev_y;
	}
	return points;
}

/// Encode of edit action
enum Action {
	EQ = 0,
	ADD,
	DEL
};

/// Diff representation
template <typename T>
struct Diff
{
	/// Character being edited
	T val_;

	/// Character position in the original array (-1 if not found in original)
	IndexT orig_;

	/// Character position in the updated array (-1 if not found in original)
	IndexT updated_;

	/// Edit action
	Action action_;
};

template <typename ARR>
using DiffArrT = Diff<ArrValT<ARR>>;

/// Return minimum-cost list of differences between orig and updated arrays
template <typename ARR, typename COMPARATOR=std::equal_to<ArrValT<ARR>>>
std::vector<DiffArrT<ARR>> myers_diff (ARR orig, ARR updated)
{
	std::vector<DiffArrT<ARR>> diffs;
	PointT prev{orig.size(), updated.size()};
	auto points = myers_diff_backtrace<ARR,COMPARATOR>(orig, updated);
	for (PointT& next : points)
	{
		if (next.first == prev.first)
		{
			diffs.push_back(DiffArrT<ARR>{
				updated[next.second],
				-1,
				next.second,
				ADD
			});
		}
		else if (next.second == prev.second)
		{
			diffs.push_back(DiffArrT<ARR>{
				orig[next.first],
				next.first,
				-1,
				DEL
			});
		}
		else
		{
			diffs.push_back(DiffArrT<ARR>{
				orig[next.first],
				next.first,
				next.second,
				EQ,
			});
		}
		prev = next;
	}
	return std::vector<DiffArrT<ARR>>(
		diffs.rbegin(), diffs.rend());
}

}

#endif // PKG_DIFF_MYERS_HPP
