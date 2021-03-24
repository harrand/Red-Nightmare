// Game includes
#include "sprite_manager.hpp"
// Game DUI includes
#include "dui/sprite_manager_window.hpp"

// Basic Window/Frame includes
#include "core/tz.hpp"
#include "gl/frame.hpp"

// IO Includes
#include "gl/tz_stb_image/image_reader.hpp"
#include "GLFW/glfw3.h"

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
#include "render/device.hpp"

constexpr std::size_t scene_max_size = 512;

tz::gl::IndexedMesh square_mesh();
tz::gl::ShaderPreprocessor pre(tz::gl::Object& o);
tz::gl::ShaderProgram compile_base_shaders(tz::gl::ShaderPreprocessor& p);
tz::gl::SSBO* get_ssbo(tz::gl::ShaderPreprocessor& pre, tz::gl::Object& o, const char* name);
tz::gl::UBO* get_ubo(tz::gl::ShaderPreprocessor& pre, tz::gl::Object& o, const char* name);
void register_listeners(tz::Vec3& player_pos, rn::SpriteState& player_state);

std::size_t ubo_module_id;
std::size_t ssbo_module_id;

rn::SpriteTextureStorage collate_sprites()
{
    rn::SpriteTextureStorage sts;
    using namespace tz::ext::stb;
    auto reg = [&sts](const char* name, rn::SpriteState state, const char* uri)
    {
        sts.add_texture(name, state, read_image<tz::gl::PixelRGBA8>(uri));
    };
    reg("player", rn::SpriteState::Up, "res/textures/player/idle.png");
    reg("player", rn::SpriteState::Down, "res/textures/player/idle.png");
    reg("player", rn::SpriteState::Left, "res/textures/player/left_1.png");
    reg("player", rn::SpriteState::Right, "res/textures/player/right_1.png");
    reg("player", rn::SpriteState::Idle, "res/textures/player/idle.png");
    reg("player", rn::SpriteState::Dead, "res/textures/player/dead.png");
    reg("player", rn::SpriteState::Casting, "res/textures/player/special.png");
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
        
        tz::gl::UBO* entity_textures_ubo = get_ubo(prep, *m, "entity_textures");
        // Sampler for each scene element.
        entity_textures_ubo->terminal_resize(sizeof(tz::gl::BindlessTextureHandle) * scene_max_size);

        // Scene setup.
        tz::render::Scene scene{transform_ssbo->map()};
        tz::render::AssetBuffer::Index square_mesh_idx = scene.add_mesh({m, square_handle});

        for(std::size_t i = 0; i < scene_max_size; i++)
        {
            tz::render::SceneElement ele{square_mesh_idx};
            ele.transform.position = tz::Vec3{0.0f, 0.0f, 0.0f};
            ele.transform.rotation = tz::Vec3{0.0f, tz::pi, tz::pi};
			ele.transform.scale = tz::Vec3{10.0f, 10.0f, 10.0f};
			ele.camera.position = tz::Vec3{0.0f, 0.0f, 15.0f};
            ele.camera.fov = 1.57f;
			ele.camera.aspect_ratio = 1920.0f/1080.0f;
			ele.camera.near = 0.1f;
			ele.camera.far = 20000.0f;
            scene.add(ele);
        }

        constexpr std::size_t player_id = 0;
        tz::render::SceneElement& player_element = scene.get(player_id);
        player_element.transform.position[0] = -10.0f;

        // Render setup
        tz::IWindow& wnd = tz::get().window();
        wnd.get_frame()->set_clear_color(0.4f, 0.4f, 1.0f);

        rn::SpriteTextureStorage sprites = collate_sprites();

        tz::render::Device device{wnd.get_frame(), &base_prg, &*m};
        device.add_resource_buffers({transform_ssbo, entity_textures_ubo});
        device.set_handle(m.get_indices());

        // DUI setup
        tz::dui::emplace_window<rn::dui::SpriteManagerWindow>(sprites);

        // Other Game Initialisation
        tz::mem::UniformPool<tz::gl::BindlessTextureHandle> handle_pool = entity_textures_ubo->map_uniform<tz::gl::BindlessTextureHandle>();
        for(std::size_t i = 0; i < scene_max_size; i++)
        {
            // Initial Sprite Textures
            rn::Sprite player_idle = sprites.get("player");
            player_idle.set_state(rn::SpriteState::Casting);
            handle_pool.set(i, player_idle.get_texture());
        }

        rn::SpriteState player_state = rn::SpriteState::Casting;

        register_listeners(player_element.transform.position, player_state);

        while(!wnd.is_close_requested())
        {
            device.clear();
            (*m).bind();
            // Scene elements manipulation
            // Update player sprite.
            {
                rn::Sprite player_sprite = sprites.get("player");
                player_sprite.set_state(player_state);
                handle_pool.set(player_id, player_sprite.get_texture());
            }

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

void register_listeners(tz::Vec3& player_pos, rn::SpriteState& player_state)
{
    tz::IWindow& wnd = tz::get().window();
    wnd.register_this();
    wnd.emplace_custom_key_listener([&player_pos, &player_state](tz::input::KeyPressEvent e)
    {
        constexpr float multiplier = 1.0f;
        switch(e.key)
        {
        case GLFW_KEY_W:
            if(e.action == GLFW_RELEASE)
            {
                player_state = rn::SpriteState::Idle;
            }
            else
            {
                player_pos += tz::Vec3{0.0f, 1.0f, 0.0f} * multiplier;
                player_state = rn::SpriteState::Up;
            }
        break;
        case GLFW_KEY_S:
            if(e.action == GLFW_RELEASE)
            {
                player_state = rn::SpriteState::Idle;
            }
            else
            {
                player_pos += tz::Vec3{0.0f, -1.0f, 0.0f} * multiplier;
                player_state = rn::SpriteState::Down;
            }
        break;
        case GLFW_KEY_A:
            if(e.action == GLFW_RELEASE)
            {
                player_state = rn::SpriteState::Idle;
            }
            else
            {
                player_pos += tz::Vec3{-1.0f, 0.0f, 0.0f} * multiplier;
                player_state = rn::SpriteState::Left;
            }
        break;
        case GLFW_KEY_D:
            if(e.action == GLFW_RELEASE)
            {
                player_state = rn::SpriteState::Idle;
            }
            else
            {
                player_pos += tz::Vec3{1.0f, 0.0f, 0.0f} * multiplier;
                player_state = rn::SpriteState::Right;
            }
        break;
        }
    });
}