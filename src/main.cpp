// Game includes
#include "sprite_manager.hpp"
// Game DUI includes
#include "dui/sprite_manager_window.hpp"
#include "dui/player_manager_window.hpp"

// Basic Window/Frame includes
#include "core/tz.hpp"
#include "gl/frame.hpp"

// IO Includes
#include "gl/tz_stb_image/image_reader.hpp"
#include "core/scheduler.hpp"
#include "GLFW/glfw3.h"
#include <chrono>

// Buffer includes
#include "core/matrix.hpp"
#include "gl/manager.hpp"
#include "dui/buffer_tracker.hpp"

// Shader Related Includes
#include "gl/shader.hpp"
#include "gl/shader_preprocessor.hpp"
#include "gl/shader_compiler.hpp"
#include "gl/modules/bindless_sampler.hpp"
#include "gl/modules/cplvals.hpp"
#include "gl/modules/ssbo.hpp"
#include "gl/modules/ubo.hpp"

// Rendering includes
#include "render/scene.hpp"
#include "gl/screen.hpp"
#include "render/device.hpp"

constexpr std::size_t scene_max_size = 512;
constexpr float universal_scale = 100.0f;
constexpr tz::Vec3 cam_pos{0.0f, 0.0f, 15.0f};

const char* player_sprite_name = player_skin_default_name;

unsigned long long get_current_time()
{
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    return ms.count();  
}

void set_player_skin(PlayerSkin skin)
{
    switch(skin)
    {
        case PlayerSkin::Default:
            player_sprite_name = player_skin_default_name;
        break;
        case PlayerSkin::Hokenian:
            player_sprite_name = player_skin_hokenian_name;
        break;
        case PlayerSkin::Anthir:
            player_sprite_name = player_skin_anthir_name;
        break;
    }
}

tz::gl::IndexedMesh square_mesh();
tz::gl::ShaderPreprocessor pre(tz::gl::Object& o);
tz::gl::ShaderProgram compile_base_shaders(tz::gl::ShaderPreprocessor& p);
tz::gl::SSBO* get_ssbo(tz::gl::ShaderPreprocessor& pre, tz::gl::Object& o, const char* name);
tz::gl::UBO* get_ubo(tz::gl::ShaderPreprocessor& pre, tz::gl::Object& o, const char* name);
tz::input::KeyListener& register_listeners(rn::SpriteState& player_state, tz::Vec3& rune_pos, rn::SpriteState& rune_state);
tz::Vec3 screen_space_to_world_space(tz::Vec2 screen_space);

std::size_t ubo_module_id;
std::size_t ssbo_module_id;

rn::SpriteTextureStorage collate_sprites()
{
    rn::SpriteTextureStorage sts;
    using namespace tz::ext::stb;
    auto reg = [&sts](const char* name, rn::SpriteState state, std::initializer_list<const char*> uri)
    {
        rn::AnimatedTexture<tz::gl::Image<tz::gl::PixelRGBA8>> img{8};
        for(const char* cur_uri : uri)
        {
            img.add_frame(1, read_image<tz::gl::PixelRGBA8>(cur_uri));
        }
        sts.add_texture(name, state, img);
    };
    // Player
    reg(player_skin_default_name, rn::SpriteState::Up, {"res/textures/player/idle.png"});
    reg(player_skin_default_name, rn::SpriteState::Down, {"res/textures/player/idle.png"});
    reg(player_skin_default_name, rn::SpriteState::Left, {"res/textures/player/left_1.png", "res/textures/player/left_2.png", "res/textures/player/left_3.png"});
    reg(player_skin_default_name, rn::SpriteState::Right, {"res/textures/player/right_1.png", "res/textures/player/right_2.png", "res/textures/player/right_3.png"});
    reg(player_skin_default_name, rn::SpriteState::Idle, {"res/textures/player/idle.png"});
    reg(player_skin_default_name, rn::SpriteState::Dead, {"res/textures/player/dead.png"});
    reg(player_skin_default_name, rn::SpriteState::Casting, {"res/textures/player/special.png"});

    // Player (Hokenian Skin)
    reg(player_skin_hokenian_name, rn::SpriteState::Up, {"res/textures/player/skins/hokenian/idle.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Down, {"res/textures/player/skins/hokenian/idle.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Left, {"res/textures/player/skins/hokenian/left_1.png", "res/textures/player/skins/hokenian/left_2.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Right, {"res/textures/player/skins/hokenian/right_1.png", "res/textures/player/skins/hokenian/right_2.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Idle, {"res/textures/player/skins/hokenian/idle.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Dead, {"res/textures/player/skins/hokenian/dead.png"});
    reg(player_skin_hokenian_name, rn::SpriteState::Casting, {"res/textures/player/skins/hokenian/special.png"});

    // Player (Anthir Skin)
    reg(player_skin_anthir_name, rn::SpriteState::Up, {"res/textures/player/skins/anthir/idle.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Down, {"res/textures/player/skins/anthir/idle.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Left, {"res/textures/player/skins/anthir/left_1.png", "res/textures/player/skins/anthir/left_2.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Right, {"res/textures/player/skins/anthir/right_1.png", "res/textures/player/skins/anthir/right_2.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Idle, {"res/textures/player/skins/anthir/idle.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Dead, {"res/textures/player/skins/anthir/dead.png"});
    reg(player_skin_anthir_name, rn::SpriteState::Casting, {"res/textures/player/skins/anthir/special.png"});

    // Nightmare
    reg("boss", rn::SpriteState::Up, {"res/textures/boss/idle.png"});
    reg("boss", rn::SpriteState::Down, {"res/textures/boss/idle.png"});
    reg("boss", rn::SpriteState::Left, {"res/textures/boss/left_1.png"});
    reg("boss", rn::SpriteState::Right, {"res/textures/boss/right_1.png"});
    reg("boss", rn::SpriteState::Idle, {"res/textures/boss/idle.png"});
    reg("boss", rn::SpriteState::Dead, {"res/textures/boss/dead.png"});
    reg("boss", rn::SpriteState::Casting, {"res/textures/boss/special.png"});

    // Ghost
    reg("ghost", rn::SpriteState::Up, {"res/textures/ghost/idle.png"});
    reg("ghost", rn::SpriteState::Down, {"res/textures/ghost/idle.png"});
    reg("ghost", rn::SpriteState::Left, {"res/textures/ghost/left_1.png"});
    reg("ghost", rn::SpriteState::Right, {"res/textures/ghost/right_1.png"});
    reg("ghost", rn::SpriteState::Idle, {"res/textures/ghost/idle.png"});
    reg("ghost", rn::SpriteState::Dead, {"res/textures/ghost/dead.png"});
    reg("ghost", rn::SpriteState::Casting, {"res/textures/ghost/special.png"});

    // Rune
    reg("rune", rn::SpriteState::Up, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Down, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Left, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Right, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Idle, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Dead, {"res/textures/rune_off.png"});
    reg("rune", rn::SpriteState::Casting, {"res/textures/rune_on.png"});
    return sts;
}

int main()
{
    tz::initialise("Red Nightmare");
    {
        tz::get().render_settings().set_culling(tz::RenderSettings::CullTarget::FrontFaces);
        // 2D game so no depth-testing (transparency in images gets depth-tested!)
        glDisable(GL_DEPTH_TEST);
        // Blend transparent pixels with background colour
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        tz::gl::Manager m;
        tz::dui::track_object(&*m);
        tz::gl::Manager::Handle square_handle = m.add_mesh(square_mesh());

        tz::gl::ShaderPreprocessor prep = pre(*m);
        tz::gl::ShaderProgram base_prg = compile_base_shaders(prep);

        tz::gl::SSBO* transform_ssbo = get_ssbo(prep, *m, "transforms");
        // MVP matrix for every scene element. Each MVP stored as a single matrix.
        transform_ssbo->terminal_resize(sizeof(tz::Mat4) * scene_max_size);
        
        tz::gl::SSBO* entity_textures_ubo = get_ssbo(prep, *m, "entity_textures");
        // Sampler for each scene element.
        entity_textures_ubo->terminal_resize(sizeof(tz::gl::BindlessTextureHandle) * scene_max_size);

        // Scene setup.
        tz::render::StandardScene2D scene{transform_ssbo->map()};
        tz::render::AssetBuffer::Index square_mesh_idx = scene.add_mesh({m, square_handle});

        const float aspect_ratio = tz::gl::Screen::primary().get_aspect_ratio();
        for(std::size_t i = 0; i < scene_max_size; i++)
        {
            tz::render::SceneElement2D ele{square_mesh_idx};
            ele.transform.position = tz::Vec3{0.0f, 0.0f, 0.0f};
            ele.transform.rotation = tz::Vec3{0.0f, tz::pi, tz::pi};
			ele.transform.scale = tz::Vec3{0.10f, 0.10f, 0.10f} * universal_scale;
			ele.camera.position = cam_pos;
            ele.camera.top = universal_scale / aspect_ratio;
            ele.camera.bottom = -universal_scale / aspect_ratio;
            ele.camera.left = -universal_scale;
            ele.camera.right = universal_scale;
            ele.camera.near = -universal_scale;
            ele.camera.far = universal_scale;
            scene.add(ele);
        }

        constexpr std::size_t player_id = scene_max_size - 1;
        constexpr std::size_t nightmare_id = 1;
        constexpr std::size_t rune_id = 2;
        constexpr std::size_t first_ghost_id = 3;
        tz::render::SceneElement2D& player_element = scene.get(player_id);
        tz::render::SceneElement2D& nightmare_element = scene.get(nightmare_id);
        tz::render::SceneElement2D& rune_element = scene.get(rune_id);
        player_element.transform.position[0] = universal_scale * -0.15f;
        nightmare_element.transform.position[0] = universal_scale * 0.15f;
        rune_element.transform.position[0] = universal_scale * -0.2f;
        rune_element.transform.scale = tz::Vec3{0.05f, 0.05f, 0.05f} * universal_scale;

        // Render setup
        tz::IWindow& wnd = tz::get().window();
        wnd.set_cursor_showing(false);
        wnd.get_frame()->set_clear_color(0.05f, 0.05f, 0.05f);

        rn::SpriteTextureStorage sprites = collate_sprites();

        tz::render::Device device{wnd.get_frame(), &base_prg, &*m};
        device.add_resource_buffers({transform_ssbo, entity_textures_ubo});
        device.set_handle(m.get_indices());

        // DUI setup
        PlayerSkin player_skin = PlayerSkin::Default;
        tz::dui::emplace_window<rn::dui::SpriteManagerWindow>(sprites);
        tz::dui::emplace_window<rn::dui::PlayerManagerWindow>(player_skin).visible = true;

        // Other Game Initialisation
        tz::mem::UniformPool<tz::gl::BindlessTextureHandle> handle_pool = entity_textures_ubo->map_uniform<tz::gl::BindlessTextureHandle>();
        tz::FixedUpdateScheduler fixed_update{1000.0f / 60.0f}; // 60 TPS
        for(std::size_t i = first_ghost_id; i < scene_max_size; i++)
        {
            // Initial Sprite Textures
            rn::Sprite player_idle = sprites.get("ghost");
            player_idle.set_state(rn::SpriteState::Idle);
            handle_pool.set(i, player_idle.get_texture());
        }
        // Nightmare isn't a ghost.
        {
            rn::Sprite nightmare_idle = sprites.get("boss");
            nightmare_idle.set_state(rn::SpriteState::Casting);
            handle_pool.set(nightmare_id, nightmare_idle.get_texture());
        }
        // Neither is the rune.
        {
            rn::Sprite rune_idle = sprites.get("rune");
            rune_idle.set_state(rn::SpriteState::Idle);
            handle_pool.set(rune_id, rune_idle.get_texture());
        }

        float rune_rotation_z = 0.0f;

        rn::SpriteState player_state = rn::SpriteState::Idle;
        rn::SpriteState rune_state = rn::SpriteState::Idle;

        tz::input::KeyListener& kl = register_listeners(player_state, rune_element.transform.position, rune_state);

        while(!wnd.is_close_requested())
        {
            static unsigned long long old_time = get_current_time();
            unsigned long long new_time = get_current_time();
            auto delta_millis = static_cast<float>(new_time - old_time);
            old_time = new_time;

            fixed_update.update(delta_millis);
            {
                tz::FixedUpdateTick tick = fixed_update.tick();
                if(tick)
                {
                    constexpr float multiplier = 0.0025f * universal_scale;
                    bool moving = false;
                    tz::Vec3 pos_delta{0.0f, 0.0f, 0.0f};
                    if(kl.is_key_down(GLFW_KEY_W))
                    {
                        pos_delta += tz::Vec3{0.0f, 1.0f, 0.0f};
                        player_state = rn::SpriteState::Up;
                        rune_state = rn::SpriteState::Idle;
                        moving = true;
                    }
                    if(kl.is_key_down(GLFW_KEY_S))
                    {
                        pos_delta += tz::Vec3{0.0f, -1.0f, 0.0f};
                        player_state = rn::SpriteState::Down;
                        rune_state = rn::SpriteState::Idle;
                        moving = true;
                    }
                    if(kl.is_key_down(GLFW_KEY_A))
                    {
                        pos_delta += tz::Vec3{-1.0f, 0.0f, 0.0f};
                        player_state = rn::SpriteState::Left;
                        rune_state = rn::SpriteState::Idle;
                        moving = true;
                    }
                    if(kl.is_key_down(GLFW_KEY_D))
                    {
                        pos_delta += tz::Vec3{1.0f, 0.0f, 0.0f};
                        player_state = rn::SpriteState::Right;
                        rune_state = rn::SpriteState::Idle;
                        moving = true;
                    }
                    if(moving)
                    {
                        player_element.transform.position += pos_delta.normalised() * multiplier;
                    }
                    else if(player_state != rn::SpriteState::Casting)
                    {
                        player_state = rn::SpriteState::Idle;
                    }
                }
            }

            set_player_skin(player_skin);

            device.clear();
            (*m).bind();
            // Scene elements manipulation
            // Update player sprite.
            {
                rn::Sprite player_sprite = sprites.get(player_sprite_name);
                player_sprite.set_state(player_state);
                handle_pool.set(player_id, player_sprite.get_texture());
            }
            // Update rune sprite
            {
                rn::Sprite rune_sprite = sprites.get("rune");
                rune_sprite.set_state(rune_state);
                handle_pool.set(rune_id, rune_sprite.get_texture());
            }
            rune_element.transform.rotation[2] = rune_rotation_z -= 0.03f;

            sprites.update(delta_millis);

            scene.configure(device);
            device.render();
            tz::update();
            wnd.update();
        }
    }
    tz::terminate();
}

tz::gl::IndexedMesh square_mesh()
{
    tz::gl::IndexedMesh square;
    square.vertices.push_back(tz::gl::Vertex{{{-0.5f, -0.5f, 0.0f}}, {{0.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    square.vertices.push_back(tz::gl::Vertex{{{0.5f, -0.5f, 0.0f}}, {{1.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    square.vertices.push_back(tz::gl::Vertex{{{0.5f, 0.5f, 0.0f}}, {{1.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    
    square.vertices.push_back(tz::gl::Vertex{{{-0.5f, -0.5f, 0.0f}}, {{0.0f, 0.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    square.vertices.push_back(tz::gl::Vertex{{{0.5f, 0.5f, 0.0f}}, {{1.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    square.vertices.push_back(tz::gl::Vertex{{{-0.5f, 0.5f, 0.0f}}, {{0.0f, 1.0f}}, {{0.0f, 0.0f, -1.0f}}, {{}}, {{}}});
    square.indices = {0, 1, 2, 3, 4, 5};
    return square;
}

tz::gl::ShaderPreprocessor pre(tz::gl::Object& o)
{
    tz::gl::ShaderPreprocessor p;
    ubo_module_id = p.emplace_module<tz::gl::p::UBOModule>(o);
    ssbo_module_id = p.emplace_module<tz::gl::p::SSBOModule>(o);
    auto ctv_module_id = p.emplace_module<tz::gl::p::CompileTimeValueModule>();
    static_cast<tz::gl::p::CompileTimeValueModule*>(p[ctv_module_id])->set("RNSCENEMAXSIZE", std::to_string(scene_max_size));
    p.emplace_module<tz::gl::p::BindlessSamplerModule>();
    return p;
}

tz::gl::ShaderProgram compile_base_shaders(tz::gl::ShaderPreprocessor& p)
{
    tz::gl::ShaderProgram prg;
    auto vtx_src_opt = tz::res().load_text("res/shaders/base.vertex.glsl");
    auto frg_src_opt = tz::res().load_text("res/shaders/base.fragment.glsl");
    topaz_assert(vtx_src_opt.has_value() && frg_src_opt.has_value(), "Base shaders could not be read.");
    std::string vtx_src = vtx_src_opt.value();
    std::string frg_src = frg_src_opt.value();

    // Preprocess all sources
    p.set_source(vtx_src);
    p.preprocess();
    vtx_src = p.result();
    p.set_source(frg_src);
    p.preprocess();
    frg_src = p.result();

    // Upload preprocessed source
    tz::gl::Shader* vs = prg.emplace(tz::gl::ShaderType::Vertex, vtx_src);
    tz::gl::Shader* fs = prg.emplace(tz::gl::ShaderType::Fragment, frg_src);
    
    // Compile
    tz::gl::ShaderCompiler cpl;
    auto cpl_diag = cpl.compile(*vs);
    topaz_assert(cpl_diag.successful(), "Shader Vtx Compilation Fail: ", cpl_diag.get_info_log());
    cpl_diag = cpl.compile(*fs);
	topaz_assert(cpl_diag.successful(), "Shader Frg Compilation Fail: ", cpl_diag.get_info_log());

    // Link
    auto lnk_diag = cpl.link(prg);
    topaz_assert(lnk_diag.successful(), "Shader Linkage Fail: ", lnk_diag.get_info_log());

    topaz_assert(prg.usable(), "Base Shader Program not ready.");
    // We're done. Return the ready program.
    return prg;
}

tz::gl::SSBO* get_ssbo(tz::gl::ShaderPreprocessor& p, tz::gl::Object& o, const char* name)
{
    std::size_t buf_id = static_cast<tz::gl::p::SSBOModule*>(p[ssbo_module_id])->get_buffer_id(name);
    return o.get<tz::gl::BufferType::ShaderStorage>(buf_id);
}

tz::gl::UBO* get_ubo(tz::gl::ShaderPreprocessor& p, tz::gl::Object& o, const char* name)
{
    std::size_t buf_id = static_cast<tz::gl::p::UBOModule*>(p[ubo_module_id])->get_buffer_id(name);
    return o.get<tz::gl::BufferType::UniformStorage>(buf_id);
}

tz::input::KeyListener& register_listeners(rn::SpriteState& player_state, tz::Vec3& rune_pos, rn::SpriteState& rune_state)
{
    tz::IWindow& wnd = tz::get().window();
    wnd.register_this();
    tz::input::KeyListener& kl = wnd.emplace_custom_key_listener();

    wnd.emplace_custom_mouse_listener(
			[&rune_pos](tz::input::MouseUpdateEvent mue)
			{
                tz::Vec2 mouse_screen_space{static_cast<float>(mue.xpos), static_cast<float>(mue.ypos)};
                rune_pos = screen_space_to_world_space(mouse_screen_space);
			}
			,
			[&rune_state, &player_state](tz::input::MouseClickEvent mce)
			{
				if(mce.button == GLFW_MOUSE_BUTTON_LEFT)
				{
					if(mce.action == GLFW_PRESS && player_state == rn::SpriteState::Idle)
					{
                        rune_state = rn::SpriteState::Casting;
						player_state = rn::SpriteState::Casting;
					}
					else if(mce.action == GLFW_RELEASE)
					{
						rune_state = rn::SpriteState::Idle;
                        player_state = rn::SpriteState::Idle;
					}
				}
			}
		);
    return kl;
}

tz::Vec3 screen_space_to_world_space(tz::Vec2 screen_space)
{
    tz::gl::Screen monitor = tz::gl::Screen::primary();
    screen_space[0] /= monitor.get_width();
    screen_space[1] /= monitor.get_height();
    // screen space y goes down->up, but we want up->down
    screen_space[1] = (1.0f - screen_space[1]);
    screen_space -= tz::Vec2{0.5f, 0.5f};
    screen_space[0] *= universal_scale * 2;
    screen_space[1] *= universal_scale * 2 / monitor.get_aspect_ratio();
    return {screen_space[0], screen_space[1], 0.0f};
}