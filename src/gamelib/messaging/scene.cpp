#include "gamelib/messaging/scene.hpp"
#include "gamelib/messaging/system.hpp"
#include "gamelib/audio.hpp"
#include "tz/core/profile.hpp"
#include "tz/wsi/monitor.hpp"

#include <filesystem>
#include <iostream>

namespace game::messaging
{
	// right so how does all this shit work?
	// each job worker (equal to number of hardware threads) has its own thread-local message passer.
	// whenever lua code wants to perform some form of write/mutation (e.g adding a new entity, changing an internal variable) it sends a message to its thread-local message passer.
	// at the end of the update cycle, these all sync up - all the thread-local passers redirect their messages to one global receiver. all the messages are then processed via `on_scene_process_message`. Because these are all collaborated and done on a single thread, we avoid false sharing issues and dont have to lock anything.
	// this has an additional advantage:
	// whenever lua code wants to perform some kind of read operation, it does not send a message - it simply instantly does the read without any kind of lock. X threads can concurrently read from a variable so long as no write operations happen. we can guarantee this because those are all deferred to the end of the update.
	// this has one drawback: reads technically read from the previous-frame's state, and writes do not take affect until the end of the frame.
	// however, with clever use of the thread-local message passers, we could in theory check past writes when figuring out what to return for a read (this part is NYI).

	game::scene* sc = nullptr;

	void set_current_scene(game::scene& scene)
	{
		sc = &scene;
	}

	std::vector<entity_uuid> deleted_entities_this_frame = {};	
	std::vector<entity_uuid> deleted_entities_last_frame = {};	
	std::vector<std::size_t> deleted_lights_this_frame = {};
	std::vector<std::size_t> deleted_strings_this_frame = {};

	bool was_deleted_this_frame(auto val)
	{
		return std::find(deleted_entities_this_frame.begin(), deleted_entities_this_frame.end(), val) != deleted_entities_this_frame.end() ||
		std::find(deleted_entities_last_frame.begin(), deleted_entities_last_frame.end(), val) != deleted_entities_last_frame.end();
	}

	void on_scene_process_message(const scene_message& msg)
	{
		TZ_PROFZONE("scene message", 0xFF99CC44);
		tz::assert(sc != nullptr);
		// for reasons not immediately clear to me - in the edge-case of TONS of stuff happening in a very slow frame, with a clear at the end - shit goes wrong unless you catch deleted stuff from the past 2 frames instead of 1.
		// i havent yet figured out why its necessary to check the last 2 frames - probably something to do with the latency of add_entity -> instantiate -> (clear goes here and causes trouble) -> messages from instantiate are ran on deleted entity = boom
		auto light_was_deleted_this_frame = [](auto val)->bool
		{
			return std::find(deleted_lights_this_frame.begin(), deleted_lights_this_frame.end(), val) != deleted_lights_this_frame.end();
		};
		auto string_was_deleted_this_frame = [](auto val)->bool
		{
			return std::find(deleted_strings_this_frame.begin(), deleted_strings_this_frame.end(), val) != deleted_strings_this_frame.end();
		};
		if(was_deleted_this_frame(msg.uuid))
		{
			// this uuid has already been deleted within a previous message.
			// we drop this message.
			return;
		}
		switch(msg.operation)
		{
			case scene_operation::add_entity:
			{
				TZ_PROFZONE("add entity", 0xFF99CC44);
				auto val = std::any_cast<tz::lua::lua_generic>(msg.value);
				std::visit([&msg](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					if constexpr(std::is_same_v<T, std::string>) // add_entity(prefab_name) (instantiate from prefab)
					{
						tz::report("init from prefab \"%s\"", arg.c_str());
						sc->add_entity_from_prefab(msg.uuid, arg);
					}
					else if constexpr(std::is_same_v<T, std::int64_t>) // add entity(uuid) (entity copy)
					{
						tz::report("init from uuid %llu", arg);
						tz::assert(!was_deleted_this_frame(arg), "Attempted to create entity from existing uuid %lu, but this entity was deleted by a previous message this frame.", arg);
						sc->add_entity_from_existing(msg.uuid, arg);
					}
					else if constexpr(std::is_same_v<T, tz::lua::nil>) // add entity() (empty entity)
					{
						sc->add_entity(msg.uuid);
					}
					else // some cursed shit.
					{
						tz::error("add_entity(arg) invoked with invalid argument type. Argument must be string (prefab name) or int (uuid)");
					}
				}, val);
			}
			break;
			case scene_operation::remove_entity:
			{
				TZ_PROFZONE("remove entity", 0xFF99CC44);
				sc->remove_entity(msg.uuid);
				deleted_entities_this_frame.push_back(msg.uuid);
			}
			break;
			case scene_operation::clear_entities:
			{
				TZ_PROFZONE("clear entities", 0xFF99CC44);
				// pretend we deleted everything
				for(const auto& ele : *sc)
				{
					tz::assert(!ele.is_null());
					deleted_entities_this_frame.push_back(ele.ent.uuid);
				}
				// actually delete everything.
				sc->clear();
			}
			break;
			case scene_operation::entity_write:
			{
				TZ_PROFZONE("entity write", 0xFF99CC44);
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				sc->get_entity(msg.uuid).internal_variables[varname] = lua_value;
			}
			break;
			case scene_operation::entity_write_add:
			{
				TZ_PROFZONE("entity write add", 0xFF99CC44);
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				std::visit([&msg, varname](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					tz::lua::lua_generic& val = sc->get_entity(msg.uuid).internal_variables[varname];
					if(!std::holds_alternative<T>(val))
					{
						val = T{0};
					}
					if constexpr(std::is_arithmetic_v<T>)
					{
						std::get<T>(val) += arg;
					}
					else
					{
						tz::error("entity_write_add invoked on varname \"%s\" which has non-integral underlying type.", varname.c_str());
					}
				}, lua_value);
			}
			break;
			case scene_operation::entity_write_multiply:
			{
				TZ_PROFZONE("entity write multiply", 0xFF99CC44);
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				std::visit([&msg, varname](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					tz::lua::lua_generic& val = sc->get_entity(msg.uuid).internal_variables[varname];
					if(!std::holds_alternative<T>(val))
					{
						val = T{1};
					}
					if constexpr(std::is_arithmetic_v<T>)
					{
						std::get<T>(val) *= arg;
					}
					else
					{
						tz::error("entity_write_multiply invoked on varname \"%s\" which has non-integral underlying type.", varname.c_str());
					}
				}, lua_value);
			}
			break;
			case scene_operation::entity_write_multiply_percentage:
			{
				TZ_PROFZONE("entity write multiply percentage", 0xFF99CC44);
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				std::visit([&msg, varname](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;
					tz::lua::lua_generic& val = sc->get_entity(msg.uuid).internal_variables[varname];
					if(!std::holds_alternative<T>(val))
					{
						val = T{0};
					}
					if constexpr(std::is_arithmetic_v<T>)
					{
						T& valt = std::get<T>(val);
						valt = ((valt + 1.0f) * arg) - 1.0f;
					}
					else
					{
						tz::error("entity_write_multiply_percentage invoked on varname \"%s\" which has non-integral underlying type.", varname.c_str());
					}
				}, lua_value);
			}
			break;
			case scene_operation::entity_set_name:
			{
				TZ_PROFZONE("entity set name", 0xFF99CC44);
				sc->get_entity(msg.uuid).name = std::any_cast<std::string>(msg.value);
			}
			break;
			case scene_operation::entity_set_local_position:
			{
				TZ_PROFZONE("entity set local position", 0xFF99CC44);
				tz::vec3 val = std::any_cast<tz::vec3>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_local_transform(cmp.obj);
				trans.translate = val;
				sc->get_renderer().get_renderer().animated_object_set_local_transform(cmp.obj, trans);
				sc->notify_entity_change(msg.uuid);
			}
			break;
			case scene_operation::entity_set_local_rotation:
			{
				TZ_PROFZONE("entity set local rotation", 0xFF99CC44);
				tz::quat val = std::any_cast<tz::quat>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_local_transform(cmp.obj);
				trans.rotate = val;
				sc->get_renderer().get_renderer().animated_object_set_local_transform(cmp.obj, trans);
			}
			break;
			case scene_operation::entity_set_local_scale:
			{
				TZ_PROFZONE("entity set local scale", 0xFF99CC44);
				tz::vec3 val = std::any_cast<tz::vec3>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_local_transform(cmp.obj);
				trans.scale = val;
				sc->get_renderer().get_renderer().animated_object_set_local_transform(cmp.obj, trans);
				sc->notify_entity_change(msg.uuid);
			}
			break;
			case scene_operation::entity_set_global_position:
			{
				TZ_PROFZONE("entity set global position", 0xFF99CC44);
				tz::vec3 val = std::any_cast<tz::vec3>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_global_transform(cmp.obj);
				trans.translate = val;
				sc->get_renderer().get_renderer().animated_object_set_global_transform(cmp.obj, trans);
				sc->notify_entity_change(msg.uuid);
			}
			break;
			case scene_operation::entity_set_global_rotation:
			{
				TZ_PROFZONE("entity set global rotation", 0xFF99CC44);
				tz::quat val = std::any_cast<tz::quat>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_global_transform(cmp.obj);
				trans.rotate = val;
				sc->get_renderer().get_renderer().animated_object_set_global_transform(cmp.obj, trans);
			}
			break;
			case scene_operation::entity_set_global_scale:
			{
				TZ_PROFZONE("entity set global scale", 0xFF99CC44);
				tz::vec3 val = std::any_cast<tz::vec3>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				tz::trs trans = sc->get_renderer().get_renderer().animated_object_get_global_transform(cmp.obj);
				trans.scale = val;
				sc->get_renderer().get_renderer().animated_object_set_global_transform(cmp.obj, trans);
				sc->notify_entity_change(msg.uuid);
			}
			break;
			case scene_operation::entity_play_animation:
			{
				TZ_PROFZONE("entity play animation", 0xFF99CC44);
				auto [name, loop, time_warp] = std::any_cast<std::tuple<std::string, bool, float>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				sc->get_renderer().get_element(cmp).play_animation_by_name(name, loop, time_warp);
			}
			break;
			case scene_operation::entity_queue_animation:
			{
				TZ_PROFZONE("entity queue animation", 0xFF99CC44);
				auto [name, loop, time_warp] = std::any_cast<std::tuple<std::string, bool, float>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				sc->get_renderer().get_element(cmp).queue_animation_by_name(name, loop, time_warp);
			}
			break;
			case scene_operation::entity_set_subobject_texture_name:
			{
				TZ_PROFZONE("entity set global subobject texture name", 0xFF99CC44);
				std::tuple<std::size_t, std::string, std::size_t> val = std::any_cast<std::tuple<std::size_t, std::string, std::size_t>>(msg.value);
				std::size_t subobject = std::get<0>(val);
				std::string texture_name = std::get<1>(val);
				std::size_t bound_texture_id = std::get<2>(val);

				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto objh = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj)[subobject];
				auto texloc = elem.object_get_texture(objh, 0);
				//sc->get_renderer().get_renderer().object_set_visible(objh, true);
				auto texhandle = sc->get_renderer().get_texture(texture_name);
				tz::assert(texhandle != tz::nullhand, "Unrecognised texture name \"%s\"", texture_name.c_str());
				texloc.texture = texhandle;
				elem.object_set_texture(objh, bound_texture_id, texloc);
			}
			break;
			case scene_operation::entity_set_subobject_texture_colour:
			{
				TZ_PROFZONE("entity set global subobject texture colour", 0xFF99CC44);
				const auto [subobject_id, bound_texture_id, col] = std::any_cast<std::tuple<std::size_t, std::size_t, tz::vec3>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj);
				auto objh = subobjects[subobject_id];
				//sc->get_renderer().get_renderer().object_set_visible(objh, true);
				auto texloc = elem.object_get_texture(objh, bound_texture_id);
				texloc.colour_tint = col;
				elem.object_set_texture(objh, bound_texture_id, texloc);
			}
			break;
			case scene_operation::entity_set_subobject_visible:
			{
				TZ_PROFZONE("entity set subobject visible", 0xFF99CC44);
				auto [subobject_id, visible] = std::any_cast<std::pair<std::size_t, bool>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj);
				auto objh = subobjects[subobject_id];
				sc->get_renderer().get_renderer().object_set_visible(objh, visible);
			}
			break;
			case scene_operation::entity_set_subobject_pixelated:
			{
				TZ_PROFZONE("entity set subobject pixelated", 0xFF99CC44);
				auto [subobject_id, pixelate] = std::any_cast<std::pair<std::size_t, bool>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj);
				auto objh = subobjects[subobject_id];
				sc->get_renderer().get_renderer().get_object(objh).unused2[2] = pixelate;
			}
			break;
			case scene_operation::entity_set_subobject_parent:
			{
				TZ_PROFZONE("entity set subobject parent", 0xFF99CC44);
				auto [subobject_id, parent_uuid, parent_subobject] = std::any_cast<std::tuple<std::size_t, entity_uuid, std::size_t>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				auto subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj);
				auto objh = subobjects[subobject_id];

				if(parent_uuid == tz::nullhand)
				{
					sc->get_renderer().get_renderer().object_set_parent(objh, tz::nullhand);
				}
				else
				{		
					auto parent_cmp = sc->get_entity_render_component(parent_uuid);
					auto parent_subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(parent_cmp.obj);
					auto parent_objh = parent_subobjects[parent_subobject];

					sc->get_renderer().get_renderer().object_set_parent(objh, parent_objh);
				}
			}
			break;
			case scene_operation::entity_apply_buff:
			{
				TZ_PROFZONE("entity apply buff", 0xFF99CC44);
				auto buff_name = std::any_cast<std::string>(msg.value);

				auto& ent = sc->get_entity(msg.uuid);
				auto iter = std::find(ent.active_buffs.begin(), ent.active_buffs.end(), buff_name);
				if(iter == ent.active_buffs.end())
				{
					ent.active_buffs.push_back(buff_name);
				}
			}
			break;
			case scene_operation::entity_remove_buff:
			{
				TZ_PROFZONE("entity remove buff", 0xFF99CC44);
				auto buff_name = std::any_cast<std::string>(msg.value);
				auto& ent = sc->get_entity(msg.uuid);
				auto iter = std::find(ent.active_buffs.begin(), ent.active_buffs.end(), buff_name);
				if(iter != ent.active_buffs.end())
				{
					ent.active_buffs.erase(iter);
				}
			}
			break;
			case scene_operation::renderer_set_camera_position:
			{
				TZ_PROFZONE("renderer set camera position", 0xFF99CC44);
				tz::vec2 cam_pos = std::any_cast<tz::vec2>(msg.value);
				sc->get_renderer().set_camera_position(cam_pos);
			}
			break;
			case scene_operation::renderer_set_clear_colour:
			{
				TZ_PROFZONE("renderer set clear colour", 0xFF99CC44);
				tz::vec4 rgba = std::any_cast<tz::vec4>(msg.value);
				sc->get_renderer().set_clear_colour(rgba);
			}
			break;
			case scene_operation::renderer_set_global_colour_multiplier:
			{
				TZ_PROFZONE("renderer set global colour multiplier", 0xFF99CC44);
				tz::vec3 rgb = std::any_cast<tz::vec3>(msg.value);
				sc->get_renderer().global_colour_multiplier() = rgb;
			}
			break;
			case scene_operation::renderer_set_ambient_light:
			{
				TZ_PROFZONE("renderer set ambient light", 0xFF99CC44);
				tz::vec3 rgb = std::any_cast<tz::vec3>(msg.value);
				sc->get_renderer().set_ambient_light(rgb);
			}
			break;
			case scene_operation::renderer_add_texture:
			{
				TZ_PROFZONE("renderer add texture", 0xFF99CC44);
				auto [name, relpath] = std::any_cast<std::pair<std::string, std::string>>(msg.value);
				auto modpath = std::filesystem::current_path()/"mods";
				auto texture_path = modpath/relpath;
				sc->get_renderer().add_texture(name, tz::io::image::load_from_file(texture_path.string()));
			}
			break;
			case scene_operation::renderer_add_model:
			{
				TZ_PROFZONE("renderer add model", 0xFF99CC44);
				auto [name, relpath] = std::any_cast<std::pair<std::string, std::string>>(msg.value);
				auto modpath = std::filesystem::current_path()/"mods";
				auto model_path = modpath/relpath;
				sc->get_renderer().add_model(name, tz::io::gltf::from_file(model_path.string().c_str()));
			}
			break;
			case scene_operation::renderer_add_light:
			{
				TZ_PROFZONE("renderer add light", 0xFF99CC44);
				auto [uid, pos, colour, power] = std::any_cast<std::tuple<std::size_t, tz::vec3, tz::vec3, float>>(msg.value);
				sc->get_renderer().add_light(uid, {.position = pos, .colour = colour, .power = power});
				tz::report("add light uid %zu", uid);
			}
			break;
			case scene_operation::renderer_remove_light:
			{
				TZ_PROFZONE("renderer remove light", 0xFF99CC44);
				auto uid = std::any_cast<unsigned int>(msg.value);
				sc->get_renderer().remove_light(uid);
				deleted_lights_this_frame.push_back(uid);
			}
			break;
			case scene_operation::renderer_light_set_position:
			{
				TZ_PROFZONE("renderer light set position", 0xFF99CC44);
				auto [uid, pos] = std::any_cast<std::pair<std::size_t, tz::vec3>>(msg.value);

				if(light_was_deleted_this_frame(uid))
				{
					return;
				}

				auto* light = sc->get_renderer().get_light(uid);
				tz::assert(light != nullptr);
				light->position = pos;
			}
			break;
			case scene_operation::renderer_light_set_colour:
			{
				TZ_PROFZONE("renderer light set colour", 0xFF99CC44);
				auto [uid, col] = std::any_cast<std::pair<std::size_t, tz::vec3>>(msg.value);

				if(light_was_deleted_this_frame(uid))
				{
					return;
				}

				auto* light = sc->get_renderer().get_light(uid);
				tz::assert(light != nullptr);
				light->colour = col;
			}
			break;
			case scene_operation::renderer_light_set_power:
			{
				TZ_PROFZONE("renderer light set power", 0xFF99CC44);
				auto [uid, pow] = std::any_cast<std::pair<std::size_t, float>>(msg.value);

				if(light_was_deleted_this_frame(uid))
				{
					return;
				}

				auto* light = sc->get_renderer().get_light(uid);
				tz::assert(light != nullptr);
				light->power = pow;
			}
			break;
			case scene_operation::renderer_light_set_shape:
			{
				TZ_PROFZONE("renderer light set shape", 0xFF99CC44);
				auto [uid, shape] = std::any_cast<std::pair<std::size_t, std::uint32_t>>(msg.value);

				if(light_was_deleted_this_frame(uid))
				{
					return;
				}

				auto* light = sc->get_renderer().get_light(uid);
				tz::assert(light != nullptr);
				light->shape = shape;
			}
			break;
			case scene_operation::renderer_clear_lights:
			{
				TZ_PROFZONE("renderer clear lights", 0xFF99CC44);
				for(std::size_t uid : sc->get_renderer().get_all_light_uids())
				{
					deleted_lights_this_frame.push_back(uid);
				}
				sc->get_renderer().clear_lights();
			}
			break;
			case scene_operation::renderer_directional_light_set_direction:
			{
				TZ_PROFZONE("renderer directional light set direction", 0xFF99CC44);
				auto dir = std::any_cast<tz::vec3>(msg.value);
				sc->get_renderer().directional_light_set_direction(dir);
			}
			break;
			case scene_operation::renderer_directional_light_set_power:
			{
				TZ_PROFZONE("renderer directional light set power", 0xFF99CC44);
				auto pow = std::any_cast<float>(msg.value);
				sc->get_renderer().directional_light_set_power(pow);
			}
			break;
			case scene_operation::renderer_directional_light_set_colour:
			{
				TZ_PROFZONE("renderer directional light set colour", 0xFF99CC44);
				auto col = std::any_cast<tz::vec3>(msg.value);
				sc->get_renderer().directional_light_set_colour(col);
			}
			break;
			case scene_operation::renderer_add_string:
			{
				TZ_PROFZONE("renderer add string", 0xFF99CC44);
				auto [uid, pos, size, str, colour] = std::any_cast<std::tuple<std::size_t, tz::vec2, float, std::string, tz::vec3>>(msg.value);
				sc->get_renderer().add_string(uid, pos, size, str, colour);
			}
			break;
			case scene_operation::renderer_remove_string:
			{
				TZ_PROFZONE("renderer remove string", 0xFF99CC44);
				auto uid = std::any_cast<unsigned int>(msg.value);
				if(string_was_deleted_this_frame(uid))
				{
					return;
				}
				sc->get_renderer().remove_string(uid);
				deleted_strings_this_frame.push_back(uid);
			}
			break;
			case scene_operation::renderer_clear_strings:
			{
				TZ_PROFZONE("renderer clear strings", 0xFF99CC44);
				for(std::size_t uid : sc->get_renderer().get_all_string_uids())
				{
					deleted_strings_this_frame.push_back(uid);
				}
				sc->get_renderer().clear_strings();
			}
			break;
			case scene_operation::renderer_string_set_position:
			{
				TZ_PROFZONE("renderer string set position", 0xFF99CC44);
				auto [uid, pos] = std::any_cast<std::pair<std::size_t, tz::vec2>>(msg.value);

				if(string_was_deleted_this_frame(uid) || !sc->get_renderer().contains_string(uid))
				{
					return;
				}

				sc->get_renderer().string_set_position(uid, pos);
			}
			break;
			case scene_operation::renderer_set_precipitation:
			{
				TZ_PROFZONE("renderer set precipitation", 0xFF99CC44);
				auto [col, strength, dir] = std::any_cast<std::tuple<tz::vec3, float, tz::vec2>>(msg.value);

				sc->get_renderer().set_precipitation_data(col, strength, dir);
			}
			break;
			case scene_operation::audio_play_sound:
			{
				TZ_PROFZONE("audio play sound", 0xFF99CC44);
				auto [relpath, volume] = std::any_cast<std::pair<std::string, float>>(msg.value);
				auto modpath = std::filesystem::current_path()/"mods";
				auto audio_path = modpath/relpath;
				game::play_sound(audio_path.string().c_str(), volume);
			}
			break;
			case scene_operation::audio_play_music:
			{
				TZ_PROFZONE("audio play music", 0xFF99CC44);
				auto [relpath, track, volume] = std::any_cast<std::tuple<std::string, unsigned int, float>>(msg.value);
				auto modpath = std::filesystem::current_path()/"mods";
				auto audio_path = modpath/relpath;
				game::play_music(audio_path.string().c_str(), track, volume);
			}
			break;
			case scene_operation::audio_stop_music:
			{
				TZ_PROFZONE("audio stop music", 0xFF99CC44);
				auto track_id = std::any_cast<unsigned int>(msg.value);
				game::stop_music(track_id);
			}
			break;
		}
	}

	void on_scene_update()
	{
		deleted_entities_last_frame = std::move(deleted_entities_this_frame);
		deleted_entities_this_frame.clear();
		deleted_lights_this_frame.clear();
		deleted_strings_this_frame.clear();
	}

	REGISTER_MESSAGING_SYSTEM(scene_message, scene, on_scene_process_message, on_scene_update);

	static std::atomic_uint_fast64_t entity_uuid_counter = 0;
	// lua api boilerplate.
	// yes, this is rn.current_scene()

	struct lua_local_scene_message_receiver
	{
		int add_entity(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - add entity", 0xFF99CC44);
			tz::lua::lua_generic arg = tz::lua::nil{};
			auto uuid = static_cast<entity_uuid>(entity_uuid_counter.fetch_add(1));
			if(state.stack_size() > 1)
			{
				arg = state.stack_get_generic(2);
				// note: if arg is a string - it means its a prefab name.
				// imagine the scenario:
				/*
				my_entity = rn.current_scene():add_entity("my_prefab")
				local prefab_name = rn.current_scene():entity_read(my_entity, ".prefab")
				*/
				// for internal variables, this shouldnt be valid (coz the entity message hasnt been processed yet, but you're already asking to read from it.)
				// *however*, .prefab ought to be a special case - that is technically known.
				// lots of the lua codebase reads .prefab, so it's not as simple as saying "well the caller knows the prefab name, they should just use it again"
				// forcing that to be the only way to deal with this would massively complicate the codebase - would need a special code path for everything to deal with this edge-case.
				// what we do here is simple: right now, map the uuid to the prefab name locally to the lua state.
				// note: local dispatch will clear this up for each lua state, so this table doesnt get big and eat memory.
				std::string lua_code = std::format("rn.impl_prefab_map = rn.impl_prefab_map or {{}}; rn.impl_prefab_map[{}] = \"{}\"", uuid, std::get<std::string>(arg));
				state.execute(lua_code.c_str());
			}

			// entity uuid is created *now* and instantly returned.
			// caller now knows the entity id, even though it doesnt exist yet.
			// subsequent messages that use the id *should* be processed after this one, making the whole thing safe.
			local_scene_receiver.send_message
			({
				.operation = scene_operation::add_entity,
				// add new entity. its uuid is `entity_id`
				.uuid = uuid,
				.value = arg
			});
			// return the id as a uint.
			state.stack_push_uint(uuid);
			return 1;
		}

		int remove_entity(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - remove entity", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);


			std::string lua_cmd = std::format("rn.entity.on_remove({})", uuid);
			state.execute(lua_cmd.c_str());

			local_scene_receiver.send_message
			({
				.operation = scene_operation::remove_entity,
				.uuid = static_cast<entity_uuid>(uuid)
			});
			return 0;
		}

		int clear_entities(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - clear entities", 0xFF99CC44);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::clear_entities,
				.uuid = std::numeric_limits<entity_uuid>::max()
			});
			return 0;
		}

		int contains_entity(tz::lua::state& state)
		{
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			state.stack_push_bool(sc->contains_entity(uuid));
			return 1;
		}

		int entity_get_model(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get model", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			std::string mname = sc->get_entity_render_component(uuid).model_name;
			if(mname.empty())
			{
				state.stack_push_nil();
			}
			else
			{
				state.stack_push_string(mname);
			}
			return 1;
		}

		int entity_write(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity write", 0xFF99CC44);
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			tz::lua::lua_generic variable_value = state.stack_get_generic(4);
			std::pair<std::string, tz::lua::lua_generic> message{varname, variable_value};
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_write,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = message
			});
			return 0;
		}

		int entity_write_add(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity write add", 0xFF99CC44);
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			tz::lua::lua_generic variable_value = state.stack_get_generic(4);
			std::pair<std::string, tz::lua::lua_generic> message{varname, variable_value};
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_write_add,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = message
			});
			return 0;
		}

		int entity_write_multiply(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity write multiply", 0xFF99CC44);
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			tz::lua::lua_generic variable_value = state.stack_get_generic(4);
			std::pair<std::string, tz::lua::lua_generic> message{varname, variable_value};
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_write_multiply,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = message
			});
			return 0;
		}

		int entity_write_multiply_percentage(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity write multiply percentage", 0xFF99CC44);
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			tz::lua::lua_generic variable_value = state.stack_get_generic(4);
			std::pair<std::string, tz::lua::lua_generic> message{varname, variable_value};
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_write_multiply_percentage,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = message
			});
			return 0;
		}

		int entity_read(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity read", 0xFF99CC44);
			// reads instantly.
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			// if the scene does not contain this entity, it's for one of two reasons:
			// 1. the uuid is garbage nonsense and this is an error.
			// 2. a message has just been sent (and thus not processing till the next update) that creates a new entity, and the caller has instantly performed a read on it.
			// the code below tries to handle the 2nd case:
			if(!sc->contains_entity(uuid))
			{
				// if the entity doesnt exist (hopefully meaning it was only created this frame, just return nil)
				tz::assert(!was_deleted_this_frame(uuid), "Attempt to entity_read from uuid %zu, which was recently removed from the scene (or is a garbage uuid value). Logic error.", uuid);
				// see above in entity_create for this .prefab special case:
				if(varname == ".prefab")
				{
					// this handles the following case:
					/*
					local sc = rn.current_scene()
					local my_ent = sc:add_entity("my_poggers_prefab")
					local prefab_name = sc:entity_read(my_ent, ".prefab")
					-- prefab_name == "my_poggers_prefab" thanks to this hack.
					*/
					std::string lua_code = std::format("__luatmp = rn.impl_prefab_map[{}];", uuid);
					state.execute(lua_code.c_str());
					auto maybe_prefab_name = state.get_string("__luatmp");
					if(maybe_prefab_name.has_value())
					{
						state.stack_push_string(maybe_prefab_name.value());
						return 1;
					}
				}
				else
				{
					// the caller has probably just sent a "create entity please" message and also just now tried to read from it.
					// this is not an error. but, we can only return nil here.
					// note: this means the following bug could occur:
					/*
					local sc = rn.current_scene()
					local my_ent = sc:add_entity("my_poggers_prefab")
					sc:entity_write(my_ent, "magic", 123)
					local result = sc:entity_read(my_ent, "magic")
					-- result == nil, not `123`
					*/
					state.stack_push_nil();
				}
				return 1;
			}
			const auto& vars = sc->get_entity(uuid).internal_variables;
			// note: this code could be ran concurrently.
			// because all forms of scene/entity mutation are deferred, reading concurrently is safe here. note: the map operator[] accidentally constructing empties would be a data race here, so we are careful and use find() instead.
			auto iter = vars.find(varname);
			tz::lua::lua_generic ret = tz::lua::nil{};
			if(iter != vars.end())
			{
				ret = iter->second;
			}
			state.stack_push_generic(ret);
			return 1;
		}

		int entity_get_animation_length(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get animation length", 0xFF99CC44);
			auto [_, uuid, name] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			auto cmp = sc->get_entity_render_component(uuid);
			const tz::ren::animation_renderer& ren = sc->get_renderer().get_renderer();
			auto gltfh = ren.animated_object_get_gltf(cmp.obj);
			auto maybe_animation_id = ren.gltf_get_animation_id_by_name(gltfh, name);
			tz::assert(maybe_animation_id.has_value(), "No animation \"%s\" exists in gltfh.", name.c_str());
			state.stack_push_float(ren.gltf_get_animation_length(gltfh, maybe_animation_id.value()));
			return 1;
		}

		int entity_get_playing_animation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get playing animation", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);	
			auto cmp = sc->get_entity_render_component(uuid);
			std::string name = sc->get_renderer().get_element(cmp).get_playing_animation_name();
			if(name.empty())
			{
				state.stack_push_nil();
			}
			else
			{
				state.stack_push_string(name);
			}
			return 1;
		}

		int entity_play_animation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity play animation", 0xFF99CC44);
			auto [_, uuid, name] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			bool loop = false;
			float time_warp = 1.0f;
			if(state.stack_size() >= 4)
			{
				loop = state.stack_get_bool(4);
				if(state.stack_size() >= 5)
				{
					time_warp = state.stack_get_float(5);
				}
			}
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_play_animation,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::string, bool, float>{name, loop, time_warp}
			});
			return 0;
		}

		int entity_queue_animation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity queue animation", 0xFF99CC44);
			auto [_, uuid, name] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			bool loop = false;
			float time_warp = 1.0f;
			if(state.stack_size() >= 4)
			{
				loop = state.stack_get_bool(4);
				if(state.stack_size() >= 5)
				{
					time_warp = state.stack_get_float(5);
				}
			}
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_queue_animation,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::string, bool, float>{name, loop, time_warp}
			});
			return 0;
		}

		int entity_get_subobject_texture(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get subobject texture", 0xFF99CC44);
			auto [_, uuid, subobject] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
			std::size_t bound_texture_id = 0;
			if(state.stack_size() >= 4)
			{
				bound_texture_id = state.stack_get_uint(4);
			}
			auto cmp = sc->get_entity_render_component(uuid);	
			auto objh = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj)[subobject];
			auto texloc = sc->get_renderer().get_renderer().object_get_texture(objh, bound_texture_id);
			std::string name = sc->get_renderer().get_texture_name(texloc.texture);
			if(name.empty())
			{
				state.stack_push_nil();
			}
			else
			{
				state.stack_push_string(name);
			}
			return 1;
		}

		int entity_set_subobject_texture(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject texture", 0xFF99CC44);
			auto [_, uuid, subobject, texname] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, std::string>(state);
			std::size_t bound_texture_id = 0;
			if(state.stack_size() >= 5)
			{
				bound_texture_id = state.stack_get_uint(5);
			}
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_texture_name,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::size_t, std::string, std::size_t>{subobject, texname, bound_texture_id}
			});
			return 0;
		}

		int entity_get_subobject_colour(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get subobject colour", 0xFF99CC44);
			auto [_, uuid, subobject] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
			auto cmp = sc->get_entity_render_component(uuid);	
			auto objh = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj)[subobject];
			auto texloc = sc->get_renderer().get_renderer().object_get_texture(objh, 0);
			state.stack_push_float(texloc.colour_tint[0]);
			state.stack_push_float(texloc.colour_tint[1]);
			state.stack_push_float(texloc.colour_tint[2]);
			return 3;
		}

		int entity_set_subobject_colour(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject colour", 0xFF99CC44);
			auto [_, uuid, subobject, r, g, b] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, float, float, float>(state);
			std::size_t bound_texture_id = 0;
			if(state.stack_size() >= 7)
			{
				bound_texture_id = state.stack_get_uint(7);
			}
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_texture_colour,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::size_t, std::size_t, tz::vec3>{subobject, bound_texture_id, tz::vec3{r, g, b}}
			});
			return 0;
		}

		int entity_set_subobject_visible(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject pixelated", 0xFF99CC44);
			auto [_, uuid, subobject, visible] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, bool>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_visible,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::pair<std::size_t, bool>(subobject, visible)
			});
			return 0;
		}

		int entity_set_subobject_pixelated(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject pixelated", 0xFF99CC44);
			auto [_, uuid, subobject, pixelated] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, bool>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_pixelated,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::pair<std::size_t, bool>(subobject, pixelated)
			});
			return 0;
		}

		int entity_set_subobject_parent(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject parent", 0xFF99CC44);
			//size_t, uuid, size_t
			auto [_, uuid, subobject, parent_uuid, parent_subobject] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, unsigned int, unsigned int>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_parent,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::size_t, entity_uuid, std::size_t>(subobject, static_cast<entity_uuid>(parent_uuid), parent_subobject)
			});
			return 0;
		}


		int entity_clear_subobject_parent(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity clear subobject parent", 0xFF99CC44);
			//size_t, uuid, size_t
			auto [_, uuid, subobject] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_parent,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::tuple<std::size_t, entity_uuid, std::size_t>(subobject, static_cast<entity_uuid>(static_cast<tz::hanval>(tz::ren::animation_renderer::object_handle{tz::nullhand})), 0)
			});
			return 0;

		}

		int entity_apply_buff(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity apply buff", 0xFF99CC44);
			auto [_, uuid, buffname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_apply_buff,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = buffname
			});
			return 0;
		}

		int entity_remove_buff(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity remove buff", 0xFF99CC44);
			auto [_, uuid, buffname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_remove_buff,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = buffname
			});
			return 0;
		}

		int entity_get_buff_count(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get buff count", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			state.stack_push_uint(sc->get_entity(uuid).active_buffs.size());	
			return 1;
		}

		int entity_get_buff(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get buff count", 0xFF99CC44);
			auto [_, uuid, buffidx] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
			state.stack_push_string(sc->get_entity(uuid).active_buffs[buffidx]);
			return 1;
		}

		int entity_set_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set name", 0xFF99CC44);
			auto [_, uuid, name] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_name,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = name
			});
			return 0;
		}

		int entity_get_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get name", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			state.stack_push_string(sc->get_entity(uuid).name);	
			return 1;
		}

		tz::trs impl_entity_get_local_transform(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get local transform", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			tz::lua::lua_generic maybe_subobject = tz::lua::nil{};
			if(state.stack_size() >= 3)
			{
				maybe_subobject = state.stack_get_generic(3);
			}

			auto cmp = sc->get_entity_render_component(uuid);
			const auto& scren = sc->get_renderer();
			if(std::holds_alternative<tz::lua::nil>(maybe_subobject))
			{
				// no subobject specified.
				return scren.get_renderer().animated_object_get_local_transform(cmp.obj);
			}
			auto subobjects = scren.get_renderer().animated_object_get_subobjects(cmp.obj);
			unsigned int subobject_val;
			if(std::holds_alternative<double>(maybe_subobject))
			{
				subobject_val = std::get<double>(maybe_subobject);
			}
			else
			{
				subobject_val = std::get<std::int64_t>(maybe_subobject);
			}
			auto objh = subobjects[subobject_val];
			return scren.get_renderer().object_get_local_transform(objh);
		}

		tz::trs impl_entity_get_global_transform(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get global transform", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			tz::lua::lua_generic maybe_subobject = tz::lua::nil{};
			if(state.stack_size() >= 3)
			{
				maybe_subobject = state.stack_get_generic(3);
			}

			auto cmp = sc->get_entity_render_component(uuid);
			const auto& scren = sc->get_renderer();

			if(std::holds_alternative<tz::lua::nil>(maybe_subobject))
			{
				// no subobject specified.
				return scren.get_renderer().animated_object_get_global_transform(cmp.obj);
			}
			auto subobjects = scren.get_renderer().animated_object_get_subobjects(cmp.obj);
			unsigned int subobject_val;
			if(std::holds_alternative<double>(maybe_subobject))
			{
				subobject_val = std::get<double>(maybe_subobject);
			}
			else
			{
				subobject_val = std::get<std::int64_t>(maybe_subobject);
			}
			auto objh = subobjects[subobject_val];
			return scren.get_renderer().object_get_global_transform(objh);
		}

		int entity_get_local_position(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get local position", 0xFF99CC44);
			tz::trs localt = impl_entity_get_local_transform(state);
			state.stack_push_float(localt.translate[0]);
			state.stack_push_float(localt.translate[1]);
			state.stack_push_float(localt.translate[2]);
			return 3;
		}

		int entity_set_local_position(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set local position", 0xFF99CC44);
			auto [_, uuid, x, y, z] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_local_position,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::vec3{x, y, z}
			});
			return 0;
		}

		int entity_get_local_rotation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get local rotation", 0xFF99CC44);
			tz::trs localt = impl_entity_get_local_transform(state);
			state.stack_push_float(localt.rotate[0]);
			state.stack_push_float(localt.rotate[1]);
			state.stack_push_float(localt.rotate[2]);
			state.stack_push_float(localt.rotate[3]);
			return 4;
		}

		int entity_set_local_rotation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set local rotation", 0xFF99CC44);
			auto [_, uuid, x, y, z, w] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_local_rotation,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::quat{x, y, z, w}
			});
			return 0;
		}

		int entity_get_local_scale(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get local scale", 0xFF99CC44);
			tz::trs localt = impl_entity_get_local_transform(state);
			state.stack_push_float(localt.scale[0]);
			state.stack_push_float(localt.scale[1]);
			state.stack_push_float(localt.scale[2]);
			return 3;
		}

		int entity_set_local_scale(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set local scale", 0xFF99CC44);
			auto [_, uuid, x, y, z] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_local_scale,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::vec3{x, y, z}
			});
			return 0;
		}

		int entity_get_global_position(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get global position", 0xFF99CC44);
			tz::trs localt = impl_entity_get_global_transform(state);
			state.stack_push_float(localt.translate[0]);
			state.stack_push_float(localt.translate[1]);
			state.stack_push_float(localt.translate[2]);
			return 3;
		}

		int entity_set_global_position(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set global position", 0xFF99CC44);
			auto [_, uuid, x, y, z] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_global_position,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::vec3{x, y, z}
			});
			return 0;
		}


		int entity_get_global_rotation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get global rotation", 0xFF99CC44);
			tz::trs localt = impl_entity_get_global_transform(state);
			state.stack_push_float(localt.rotate[0]);
			state.stack_push_float(localt.rotate[1]);
			state.stack_push_float(localt.rotate[2]);
			state.stack_push_float(localt.rotate[3]);
			return 4;
		}

		int entity_set_global_rotation(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set global rotation", 0xFF99CC44);
			auto [_, uuid, x, y, z, w] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_global_rotation,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::quat{x, y, z, w}
			});
			return 0;
		}

		int entity_get_global_scale(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get global scale", 0xFF99CC44);
			tz::trs localt = impl_entity_get_global_transform(state);
			state.stack_push_float(localt.scale[0]);
			state.stack_push_float(localt.scale[1]);
			state.stack_push_float(localt.scale[2]);
			return 3;
		}

		int entity_set_global_scale(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set global scale", 0xFF99CC44);
			auto [_, uuid, x, y, z] = tz::lua::parse_args<tz::lua::nil, unsigned int, float, float, float>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_global_scale,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = tz::vec3{x, y, z}
			});
			return 0;
		}

		int get_mouse_position(tz::lua::state& state)
		{
			tz::vec2 pos = sc->get_mouse_position_world_space();
			state.stack_push_float(pos[0]);
			state.stack_push_float(pos[1]);
			return 2;
		}
	};

	LUA_CLASS_BEGIN(lua_local_scene_message_receiver)
		LUA_CLASS_METHODS_BEGIN
			LUA_METHOD(lua_local_scene_message_receiver, add_entity)
			LUA_METHOD(lua_local_scene_message_receiver, remove_entity)
			LUA_METHOD(lua_local_scene_message_receiver, clear_entities)
			LUA_METHOD(lua_local_scene_message_receiver, contains_entity)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_model)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write_add)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write_multiply)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write_multiply_percentage)
			LUA_METHOD(lua_local_scene_message_receiver, entity_read)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_animation_length)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_playing_animation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_play_animation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_queue_animation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_subobject_texture)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_texture)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_subobject_colour)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_colour)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_visible)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_pixelated)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_parent)
			LUA_METHOD(lua_local_scene_message_receiver, entity_clear_subobject_parent)
			LUA_METHOD(lua_local_scene_message_receiver, entity_apply_buff)
			LUA_METHOD(lua_local_scene_message_receiver, entity_remove_buff)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_buff_count)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_buff)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_name)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_name)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_local_position)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_local_position)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_local_rotation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_local_rotation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_local_scale)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_local_scale)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_global_position)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_global_position)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_global_rotation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_global_rotation)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_global_scale)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_global_scale)
			LUA_METHOD(lua_local_scene_message_receiver, get_mouse_position)
		LUA_CLASS_METHODS_END
	LUA_CLASS_END

	LUA_BEGIN(rn_renderer)
		using namespace game::render;
		impl_rn_scene_renderer ren{.renderer = &sc->get_renderer()};
		LUA_CLASS_PUSH(state, impl_rn_scene_renderer, ren);
		return 1;
	LUA_END

	LUA_BEGIN(rn_current_scene)
		LUA_CLASS_PUSH(state, lua_local_scene_message_receiver, lua_local_scene_message_receiver{});
		return 1;
	LUA_END

	// implementation.

	void scene_messaging_lua_initialise(tz::lua::state& state)
	{
		// set the local message passer to target the global receiver. otherwise all its messages will be dropped.
		local_scene_receiver.set_target(global_scene_receiver);
		// expose the class api
		state.execute("rn = rn or {}");
		state.new_type("lua_local_scene_message_receiver", LUA_CLASS_NAME(lua_local_scene_message_receiver)::registers);
		state.assign_func("rn.renderer", LUA_FN_NAME(rn_renderer));
		state.assign_func("rn.current_scene", LUA_FN_NAME(rn_current_scene));
		// rn.current_scene:add_entity(...) is now a thing.
	}

	void scene_messaging_update()
	{
		TZ_PROFZONE("scene - update messages", 0xFFAAAACC);
		// main thread might also have messages to pass on.
		local_scene_receiver.update();

		// global receiver will now have all the messages ready.
		// we can finally process them all.
		TZ_PROFZONE("scene - process all messages", 0xFF99CC44);
		global_scene_receiver.update();
	}

	void scene_messaging_local_dispatch()
	{
		TZ_PROFZONE("scene - pass local messages", 0xFF99CC44);
		local_scene_receiver.update();
		// clear our hacky global table up
		tz::lua::get_state().execute("rn.impl_prefab_map = {}");
	}

	void scene_messaging_force_dispatches()
	{
		TZ_PROFZONE("scene - force local dispatches", 0xFF99CC44);
		tz::lua::for_all_states([](tz::lua::state& state)
		{
			// for each state, invoke update() on its local scene receiver.
			// this passes all its messages to the global scene receiver.
			scene_messaging_local_dispatch();
		});
		// do this thread too. main thread could also have messages.
		scene_messaging_local_dispatch();
	}

	entity_uuid scene_quick_add(std::string prefab_name)
	{
		auto ret = static_cast<entity_uuid>(entity_uuid_counter.fetch_add(1));
		sc->add_entity_from_prefab(ret, prefab_name);
		return ret;
	}

	void scene_insert_message(scene_message msg)
	{
		local_scene_receiver.send_message(msg);
	}
}
