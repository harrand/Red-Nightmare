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

	void on_scene_process_message(const scene_message& msg)
	{
		TZ_PROFZONE("scene message", 0xFF99CC44);
		tz::assert(sc != nullptr);
		auto was_deleted_this_frame = [](auto val)->bool
		{
			return std::find(deleted_entities_this_frame.begin(), deleted_entities_this_frame.end(), val) != deleted_entities_this_frame.end();
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
				std::visit([&msg, &was_deleted_this_frame](auto&& arg)
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
			case scene_operation::set_level_name:
			{
				TZ_PROFZONE("set level name", 0xFF99CC44);
				std::string name = std::any_cast<std::string>(msg.value);
				sc->set_current_level_name(name);
			}
			break;
			case scene_operation::entity_write:
			{
				TZ_PROFZONE("entity write", 0xFF99CC44);
				const auto& [varname, lua_value] = std::any_cast<std::pair<std::string, tz::lua::lua_generic>>(msg.value);
				sc->get_entity(msg.uuid).internal_variables[varname] = lua_value;
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
			case scene_operation::entity_set_subobject_texture_name:
			{
				TZ_PROFZONE("entity set global subobject texture name", 0xFF99CC44);
				std::pair<std::size_t, std::string> val = std::any_cast<std::pair<std::size_t, std::string>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto objh = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj)[val.first];
				auto texloc = elem.object_get_texture(objh, 0);
				//sc->get_renderer().get_renderer().object_set_visible(objh, true);
				auto texhandle = sc->get_renderer().get_texture(val.second);
				tz::assert(texhandle != tz::nullhand, "Unrecognised texture name \"%s\"", val.second.c_str());
				texloc.texture = texhandle;
				elem.object_set_texture(objh, 0, texloc);
			}
			break;
			case scene_operation::entity_set_subobject_texture_colour:
			{
				TZ_PROFZONE("entity set global subobject texture colour", 0xFF99CC44);
				std::pair<std::size_t, tz::vec3> val = std::any_cast<std::pair<std::size_t, tz::vec3>>(msg.value);
				auto cmp = sc->get_entity_render_component(msg.uuid);
				render::scene_element elem = sc->get_renderer().get_element(cmp);
				auto subobjects = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj);
				auto objh = subobjects[val.first];
				//sc->get_renderer().get_renderer().object_set_visible(objh, true);
				auto texloc = elem.object_get_texture(objh, 0);
				texloc.colour_tint = val.second;
				elem.object_set_texture(objh, 0, texloc);
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
		deleted_entities_this_frame.clear();
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
			if(state.stack_size() > 1)
			{
				arg = state.stack_get_generic(2);
			}
			// entity uuid is created *now* and instantly returned.
			// caller now knows the entity id, even though it doesnt exist yet.
			// subsequent messages that use the id *should* be processed after this one, making the whole thing safe.
			auto uuid = static_cast<entity_uuid>(entity_uuid_counter.fetch_add(1));
			local_scene_receiver.send_message
			({
				.operation = scene_operation::add_entity,
				// add new entity. its uuid is `entity_id`
				.uuid = uuid,
				.value = arg
			});
			// return the id as a uint.
			// TODO: wrap userdata around this?
			state.stack_push_uint(uuid);
			return 1;
		}

		int remove_entity(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - remove entity", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
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

		int set_level_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - set level name", 0xFF99CC44);
			auto [_, name] = tz::lua::parse_args<tz::lua::nil, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::set_level_name,
				.uuid = std::numeric_limits<entity_uuid>::max(),
				.value = name
			});
			return 0;
		}

		int get_level_name(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - get level name", 0xFF99CC44);
			std::string name = sc->get_current_level_name();
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

		int entity_read(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity read", 0xFF99CC44);
			// reads instantly.
			auto [_, uuid, varname] = tz::lua::parse_args<tz::lua::nil, unsigned int, std::string>(state);
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

		int entity_get_subobject_texture(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get subobject texture", 0xFF99CC44);
			auto [_, uuid, subobject] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int>(state);
			auto cmp = sc->get_entity_render_component(uuid);	
			auto objh = sc->get_renderer().get_renderer().animated_object_get_subobjects(cmp.obj)[subobject];
			auto texloc = sc->get_renderer().get_renderer().object_get_texture(objh, 0);
			state.stack_push_string(sc->get_renderer().get_texture_name(texloc.texture));
			return 1;
		}

		int entity_set_subobject_texture(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity set subobject texture", 0xFF99CC44);
			auto [_, uuid, subobject, texname] = tz::lua::parse_args<tz::lua::nil, unsigned int, unsigned int, std::string>(state);
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_texture_name,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::pair<std::size_t, std::string>{subobject, texname}
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
			local_scene_receiver.send_message
			({
				.operation = scene_operation::entity_set_subobject_texture_colour,
				.uuid = static_cast<entity_uuid>(uuid),
				.value = std::pair<std::size_t, tz::vec3>{subobject, tz::vec3{r, g, b}}
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
			auto objh = subobjects[std::get<std::int64_t>(maybe_subobject)];
			return scren.get_renderer().object_get_local_transform(objh);
		}

		tz::trs impl_entity_get_global_transform(tz::lua::state& state)
		{
			TZ_PROFZONE("scene - entity get global transform", 0xFF99CC44);
			auto [_, uuid] = tz::lua::parse_args<tz::lua::nil, unsigned int>(state);
			tz::lua::lua_generic maybe_subobject = state.stack_get_generic(3);

			auto cmp = sc->get_entity_render_component(uuid);
			const auto& scren = sc->get_renderer();

			if(std::holds_alternative<tz::lua::nil>(maybe_subobject))
			{
				// no subobject specified.
				return scren.get_renderer().animated_object_get_global_transform(cmp.obj);
			}
			auto subobjects = scren.get_renderer().animated_object_get_subobjects(cmp.obj);
			auto objh = subobjects[std::get<double>(maybe_subobject)];
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
			LUA_METHOD(lua_local_scene_message_receiver, set_level_name)
			LUA_METHOD(lua_local_scene_message_receiver, get_level_name)
			LUA_METHOD(lua_local_scene_message_receiver, contains_entity)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_model)
			LUA_METHOD(lua_local_scene_message_receiver, entity_write)
			LUA_METHOD(lua_local_scene_message_receiver, entity_read)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_subobject_texture)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_texture)
			LUA_METHOD(lua_local_scene_message_receiver, entity_get_subobject_colour)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_colour)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_visible)
			LUA_METHOD(lua_local_scene_message_receiver, entity_set_subobject_pixelated)
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
