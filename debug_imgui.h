#include "imgui.h"
#include "editor_screen.h"

//Called from IOGfxDisplaySW
extern void dbg_imgui();

//For the debug log
extern ImGuiTextBuffer Buf;
//For the tile toggle
extern bool render_tiles;
//For the pink squares, sprites, and mem info, and so on and so on
extern bool debug_meminfo, debug_pinksquares, debug_drawsprites, debug_drawgame, debug_gamewindow, debug_paused, debug_conpause, debug_scripboot, dswitch, debug_texturefilter;
//For the editor
extern char msg[500];
extern int scr;
extern int ts;
extern int edit_edscale;
extern bool edit_hard, edit_nohit, debug_input, edit_viewsprites, edit_fillhard, edit_minimapupdate, edit_minimapbig, edit_minimapbigrend, edit_viewinvis, edit_spboundaries, edit_drawtiles, edit_cornertile, edit_drawvis;
extern void render_sprite_preview();
//For the minimap 
extern SDL_Texture* texture3;
//extern SDL_Texture* texture4;
extern void copy_screen_big(int x1, int y1);
extern void render_minimap_big();
//sprite editor index, forbidden sequence, colour to display when hiding tiles
extern int spri, edit_forbiddenseq, tile_hide_col;
extern editor_screen ramscreens[769];
extern editor_screen savestate[128];
extern void update_minimap_view();
extern void editor_load_screen_debug(int num);