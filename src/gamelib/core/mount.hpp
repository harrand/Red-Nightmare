#ifndef RNLIB_CORE_MOUNT_HPP
#define RNLIB_CORE_MOUNT_HPP
#include <cstddef>

namespace rnlib
{
	enum class mount_error
	{
		// no error took place.
		no_error,
		// ran out of quads.
		ooq,
	};

	struct mount_result
	{
		mount_error error = mount_error::no_error;
		std::size_t count = 0;

		inline mount_result operator<<(const mount_result& rhs)
		{
			if(this->error != mount_error::no_error)
			{
				// dont bother if the mount is already failed.
				return *this;
			}
			this->error = rhs.error;
			this->count += rhs.count;
			return *this;
		}
	};
}

#endif // RNLIB_CORE_MOUNT_HPP
