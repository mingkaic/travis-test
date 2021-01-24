///
/// sequence.hpp
/// jobs
///
/// Purpose:
/// Define structure that manages sequentially dependency for threads
///

#ifndef PKG_JOBS_SEQUENCE_HPP
#define PKG_JOBS_SEQUENCE_HPP

#include <functional>
#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <list>

#include "logs/logs.hpp"

namespace jobs
{

/// Manages sequential dependency of detached threads
struct Sequence final
{
	Sequence (void)
	{
		stop_future_ = stop_signal_.get_future();
		master_ = ManagedJob(
			[](Sequence* seq)
			{
				std::packaged_task<void()> tsk;
				{
					std::unique_lock<std::mutex> lock(seq->queue_mutex_);
					seq->condition_.wait(lock,
						[seq]
						{
							if (seq->master_.exit_future_.wait_for(
								std::chrono::milliseconds(1)) ==
								std::future_status::timeout)
							{
								return seq->tasks_.size() > 0;
							}
							// immediately trigger condition
							return true;
						});
					if (seq->tasks_.empty())
					{
						return;
					}
					tsk = std::move(seq->tasks_.front());
					seq->tasks_.pop_front();
				}
				std::thread thd(std::move(tsk));
				thd.join();
			}, this);
	}

	~Sequence (void)
	{
		stopped_ = true;
		stop();
		master_.stop();
		master_.join();
	}

	/// Add a new job that depends on all jobs previously attached
	template <typename FN, typename ...ARGS>
	void attach_job (FN&& call, ARGS&&... args)
	{
		std::function<bool(size_t)> job = std::bind(
			std::forward<FN>(call), std::placeholders::_1,
				std::forward<ARGS>(args)...);
		std::packaged_task<void()> tsk([this, job]()
		{
			size_t i = 0;
			do
			{
				if (job(i))
				{
					break;
				}
				++i;
			}
			while (this->stop_future_.wait_for(
				std::chrono::milliseconds(1)) ==
				std::future_status::timeout);
		});
		last_future_ = tsk.get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex_);
			// don't allow enqueueing after stopping the pool
			if(stopped_)
			{
				logs::fatal("cannot attach new job on deleted sequence");
			}
			tasks_.push_back(std::move(tsk));
		}
		condition_.notify_one();
	}

	/// Return true if any job is running
	/// otherwise false
	bool is_running (void) const
	{
		return last_future_.valid();
	}

	/// Join all jobs to complete
	void join (void)
	{
		if (last_future_.valid())
		{
			last_future_.wait();
		}
	}

	/// Stop all jobs
	void stop (void)
	{
		std::unique_lock<std::mutex> lock(queue_mutex_);
		tasks_.clear();
		try
		{
			stop_signal_.set_value();
		}
		catch (...) {}
		condition_.notify_one();
	}

private:
	std::promise<void> stop_signal_;

	std::future<void> stop_future_;

	std::condition_variable condition_;

	bool stopped_ = false;

	std::list<std::packaged_task<void()>> tasks_;

	mutable std::mutex queue_mutex_;

	std::future<void> last_future_;

	ManagedJob master_;
};

}

#endif // PKG_JOBS_SEQUENCE_HPP
