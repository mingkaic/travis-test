#include "gtest/gtest.h"

#include "jobs/managed_job.hpp"
#include "jobs/scope_guard.hpp"
#include "jobs/sequence.hpp"


int main (int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


#ifndef DISABLE_JOB_TEST


TEST(JOBS, ManagedJob)
{
	size_t attempts = 0;
	bool success = false;
	jobs::ManagedJob managed(
	[&attempts](bool& success)
	{
		if (attempts >= 30)
		{
			// allow at most 30 seconds
			// fail condition
			success = false;
			return;
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000));
		success = true;
		++attempts;
	}, std::ref(success));

	std::chrono::time_point<std::chrono::system_clock> deadline =
		std::chrono::system_clock::now() +
		std::chrono::seconds(1);
	std::condition_variable time_done;
	std::thread timed_killer(
	[&]
	{
		std::mutex mtx;
		std::unique_lock<std::mutex> lck(mtx);
		time_done.wait_until(lck, deadline);
		managed.stop();
	});
	managed.join();
	time_done.notify_one();
	if (timed_killer.joinable())
	{
		timed_killer.join();
	}

	// failure takes at most 30 seconds
	EXPECT_TRUE(success) << "managed job failed to stop" << std::endl;
}


TEST(JOBS, ManagedJobTermination)
{
	bool success = false;
	{
		size_t attempts = 0;
		jobs::ManagedJob managed(
		[&success, &attempts]()
		{
			if (attempts >= 30)
			{
				// allow at most 30 seconds
				// fail condition
				success = false;
				return;
			}
			std::this_thread::sleep_for(
				std::chrono::milliseconds(1000));
			success = true;
			++attempts;
		});
		EXPECT_FALSE(success);
		// managed termination by destruction
	}
	// wait at least 5 attempts for job to terminate
	std::this_thread::sleep_for(
		std::chrono::milliseconds(5000));
	EXPECT_TRUE(success);
}


TEST(JOBS, ScopeGuards)
{
	bool expect = false;
	{
		jobs::ScopeGuard defer(
		[&]
		{
			expect = true;
		});
		EXPECT_FALSE(expect);
	}
	EXPECT_TRUE(expect);
}


TEST(JOBS, SequenceOrdering)
{
	jobs::Sequence seq;
	size_t i = 0;

	// job 1
	seq.attach_job(
	[](size_t, size_t& i)
	{
		std::this_thread::sleep_for(
			std::chrono::milliseconds(5000)); // wait longer job 2
		i += 1; // test
		return true;
	}, std::ref(i));

	// job 2
	seq.attach_job(
	[](size_t, size_t& i)
	{
		i *= 2; // test
		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000));
		return true;
	}, std::ref(i));

	seq.join();
	// without sequence, job 2 is more likely to run before job 1
	// if job 2 ran before job 1, i = 0 * 2 + 1 instead of i = (0 + 1) * 2
	EXPECT_EQ(2, i) << "sequence didn't execute properly:" <<
		"{0: 'job 1 did not run', 1: 'job 2 ran before job 1'}";
}


TEST(JOBS, SequenceTermination)
{
	jobs::Sequence seq;
	bool job1_succ = false;
	bool job2_succ = false;

	// job 1
	seq.attach_job(
	[](size_t attempt, bool& success)
	{
		success = true;
		if (attempt >= 20)
		{
			// allow at most 20 seconds
			// fail condition
			success = false;
			return true;
		}
		std::this_thread::sleep_for(
			std::chrono::milliseconds(1000));
		++attempt;
		return false;
	}, std::ref(job1_succ));

	// job 2
	seq.attach_job(
	[](size_t attempt, bool& passed)
	{
		// this job should never have executed
		passed = true;
		return true;
	}, std::ref(job2_succ));

	std::chrono::time_point<std::chrono::system_clock> deadline =
		std::chrono::system_clock::now() +
		std::chrono::seconds(1);
	std::condition_variable time_done;
	std::thread timed_killer(
	[&]
	{
		std::mutex mtx;
		std::unique_lock<std::mutex> lck(mtx);
		time_done.wait_until(lck, deadline);
		seq.stop();
	});
	seq.join();
	time_done.notify_one();
	if (timed_killer.joinable())
	{
		timed_killer.join();
	}

	// failure takes at most 40 seconds
	EXPECT_TRUE(job1_succ) << "job 1 failed to stop before 20 seconds";
	EXPECT_FALSE(job2_succ) << "job 2 failed to stop before 20 seconds";
}


#endif // DISABLE_JOB_TEST
