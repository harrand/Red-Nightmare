#include "action.hpp"
#include "scene.hpp"
#include "hdk/profile.hpp"

namespace game
{
	#define ACTION_IMPL_BEGIN(T) template<> void action_invoke<T>(SceneData& scene, Action<T>& action){
	#define ACTION_IMPL_END(T) } template void action_invoke<T>(SceneData& scene, Action<T>&);

	ACTION_IMPL_BEGIN(ActionID::GotoMouse)
		scene.actor().entity.add<ActionID::GotoTarget>
		({
			.target_position = scene.mouse_position
		});
		action.set_is_complete(true);
	ACTION_IMPL_END(ActionID::GotoMouse)

	ACTION_IMPL_BEGIN(ActionID::LaunchToMouse)
		const hdk::vec2 to_mouse = scene.mouse_position - scene.quad().position;
		scene.actor().entity.add<ActionID::Launch>
		({
			.direction = scene.mouse_position - scene.quad().position,
			.speed_multiplier = action.data().speed_multiplier
		});
	ACTION_IMPL_END(ActionID::LaunchToMouse)

	void ActionEntity::update()
	{
		HDK_PROFZONE("ActionEntity - Update", 0xFF00AA00);
		// Remove all completed actions.
		this->components.erase(std::remove_if(this->components.begin(), this->components.end(), [](const auto& comp_ptr)
		{
			return comp_ptr->get_is_complete();
		}), this->components.end());
	}
}
