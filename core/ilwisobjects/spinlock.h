#pragma once

#include <atomic>

namespace Ilwis {
	class SpinLock
	{
	private:
		std::atomic_flag _lockFlag;

	public:
		SpinLock()
			: _lockFlag{ ATOMIC_FLAG_INIT }
		{ }

		void lock()
		{
			while (_lockFlag.test_and_set(std::memory_order_acquire))
			{
			}
		}

		bool try_lock()
		{
			return !_lockFlag.test_and_set(std::memory_order_acquire);
		}

		void unlock()
		{
			_lockFlag.clear();
		}
	};
}
