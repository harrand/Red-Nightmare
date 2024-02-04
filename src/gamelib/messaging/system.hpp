#include "tz/core/messaging.hpp"

#define rnxstr(a) rnstr(a)
#define rnstr(a) #a

namespace game::messaging
{
	#define REGISTER_MESSAGING_SYSTEM(MSG_T, THING_NAME, ON_PROCESS_MESSAGE, ON_UPDATE) \
	class thread_local_##THING_NAME##_message_receiver : public tz::message_passer<MSG_T, false> \
	{ public: void update(){this->process_messages();}}; \
	class global_##THING_NAME##_message_receiver : public tz::message_receiver<MSG_T, true> \
	{public: void update(){ON_UPDATE(); this->process_messages();} \
	virtual void process_message(const MSG_T& msg) override final \
	{ TZ_PROFZONE(rnxstr(THING_NAME)" - process message", 0XFF99CC44); ON_PROCESS_MESSAGE(msg);}}; \
	global_##THING_NAME##_message_receiver global_##THING_NAME##_receiver; \
	thread_local thread_local_##THING_NAME##_message_receiver local_##THING_NAME##_receiver;
}
