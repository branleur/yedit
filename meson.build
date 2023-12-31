project('yedit', 'cpp',
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++14'])

# Dependencies
sdl2_deps = [dependency('sdl2'), dependency('sdl2_mixer'), dependency('sdl2_image'), dependency('sdl2_ttf'), dependency('SDL2_gfx'), dependency('glm')]
intl_dep = dependency('intl', required: true)
font_deps = [dependency('fontconfig'), dependency('freetype2')]

i18n = import('i18n')
i18n.gettext(meson.project_name())

incdir = include_directories('include')

# Our output
executable('yedit',
           'live_screen.cpp',
           'gfx_fonts.cpp',
           'FakeIOGfxDisplay.cpp',
           'log.cpp',
           'live_sprite.cpp',
           'imgui_widgets.cpp',
           'ImageLoader.cpp',
           'input.cpp',
           'IOGfxSurfaceSW.cpp',
           'AppFreeDinkedit.cpp',
           'debug_imgui.cpp',
           'IOGfxSurfaceGL2.cpp',
           'io_util.cpp',
           'hardness_tiles.cpp',
           'editor_screen.cpp',
           'gfx_tiles.cpp',
           'freedink_xpm.cpp',
           'imgui_impl_sdlrenderer2.cpp',
           'fastfile.cpp',
           'dinkini.cpp',
           'sfx.cpp',
           'imgui_impl_sdl2.cpp',
           'DMod.cpp',
           'resources.cpp',
           'live_sprites_manager.cpp',
           'gfx_sprites.cpp',
           'IOGfxDisplay.cpp',
           'IOGfxDisplaySW.cpp',
           'freedinkedit.cpp',
           'IOGfxGLFuncs.cpp',
           'str_util.cpp',
           'imgui.cpp',
           'rect.cpp',
           'bgm.cpp',
           'gfx_palette.cpp',
           'vgasys_fon.cpp',
           'EditorMap.cpp',
           'IOGfxSurface.cpp',
           'paths.cpp',
           'IOGfxPrimitivesSW.cpp',
           'imgui_stdlib.cpp',
           'BgTilesetsManager.cpp',
           'imgui_tables.cpp',
           'IOGfxDisplayGL2.cpp',
           'gfx.cpp',
           'imgui_demo.cpp',
           'imgui_draw.cpp',
           'app.cpp',
           'backward.cpp',
           'imgui_impl_opengl3.cpp',
           install : true, dependencies: [sdl2_deps, intl_dep, font_deps], include_directories: incdir)
