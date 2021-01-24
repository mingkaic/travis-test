#include "diff/msg.hpp"
#include <iostream>

#ifdef PKG_DIFF_MSG_HPP

namespace diff
{

using MsgDiffT = std::vector<DiffArrT<types::StringsT>>;

static std::string to_string (const MsgDiffT& diffs)
{
	IndexT ndiffs = diffs.size();
	bool show[ndiffs];
	std::memset(show, false, sizeof(bool) * ndiffs);
	// mark exact line to show
	for (IndexT i = 0; i < ndiffs; ++i)
	{
		if (diffs[i].action_ != EQ)
		{
			IndexT begin = std::max(0, i - lines_before);
			IndexT end = std::min(ndiffs - 1, i + lines_after);
			std::memset(show + begin, true, sizeof(bool) * (end - begin + 1));
		}
	}
	size_t oidx = 0;
	size_t uidx = 0;
	std::stringstream out;
	for (IndexT i = 0; i < ndiffs; ++i)
	{
		if (show[i])
		{
			diff_line_format(out, diffs[i].val_,
				diffs[i].action_, oidx, uidx);
		}
		switch (diffs[i].action_)
		{
			case EQ:
				++oidx;
				++uidx;
				break;
			case ADD:
				++uidx;
				break;
			case DEL:
				++oidx;
				break;
		}
	}
	return out.str();
}

static void process_lines (types::StringsT& lines, std::istream& str,
	bool ignore_empty_lines, bool trim_spaces)
{
	std::string line;
	while (std::getline(str, line))
	{
		if (trim_spaces)
		{
			fmts::trim(line);
		}
		if (false == ignore_empty_lines || line.size() > 0)
		{
			// consider line
			lines.push_back(line);
		}
	}
}

static void safe_diff_msg_helper (MsgDiffT& diffs,
	const types::StringsT& expect,
	const types::StringsT& got,
	size_t batch_limit)
{
	size_t nexpect = expect.size();
	size_t ngot = got.size();

	if (nexpect <= batch_limit && ngot <= batch_limit)
	{
		MsgDiffT mdiffs = myers_diff(expect, got);
		std::stringstream out;
		for (IndexT i = 0, n = mdiffs.size(); i < n; ++i)
		{
			diffs.push_back(mdiffs[i]);
		}
		return;
	}

	auto exit = expect.begin();
	auto goit = got.begin();
	types::StringsT exbatch(exit, exit + std::min(batch_limit, nexpect));
	types::StringsT gobatch(goit, goit + std::min(batch_limit, ngot));

	MsgDiffT mdiffs = myers_diff(exbatch, gobatch);

	IndexT diff_end, eoffset, goffset;
	diff_end = mdiffs.size();
	eoffset = std::min(batch_limit, nexpect);
	goffset = std::min(batch_limit, ngot);
	for (IndexT i = 0, n = mdiffs.size(); i < n; ++i)
	{
		if (mdiffs[i].action_ == EQ)
		{
			diff_end = i + 1;
			eoffset = mdiffs[i].orig_ + 1;
			goffset = mdiffs[i].updated_ + 1;
		}
	}

	for (IndexT i = 0; i < diff_end; ++i)
	{
		diffs.push_back(mdiffs[i]);
	}

	// revalidate contiguous non-EQ that continues pass the end of line
	// and affix to existing messages
	safe_diff_msg_helper(diffs,
		types::StringsT(exit + eoffset, expect.end()),
		types::StringsT(goit + goffset, got.end()), batch_limit);
}

std::string diff_msg (
	const types::StringsT& expect,
	const types::StringsT& got)
{
	MsgDiffT diffs = myers_diff(expect, got);
	return to_string(diffs);
}

std::string safe_diff_msg (
	const types::StringsT& expect,
	const types::StringsT& got,
	size_t batch_limit)
{
	MsgDiffT diff;
	safe_diff_msg_helper(diff, expect, got, batch_limit);
	return to_string(diff);
}

std::string diff_lines (
	std::istream& expect, std::istream& got,
	bool ignore_empty_lines, bool trim_spaces)
{
	types::StringsT exlines;
	types::StringsT golines;
	process_lines(exlines, expect, ignore_empty_lines, trim_spaces);
	process_lines(golines, got, ignore_empty_lines, trim_spaces);

	return safe_diff_msg(exlines, golines);
}

}

#endif
