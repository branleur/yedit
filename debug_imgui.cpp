/**
 * Replacement Debug Interface

 * Copyright (C) 2022-2023 Yeoldetoast

 * This file is part of YeoldeDink

 * Yeoldedink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * Yeoldedink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "SDL.h"
#include "random.hpp"

#include "debug_imgui.h"
#include "live_sprites_manager.h"
#include "input.h"
#include "log.h"
#include "gfx.h"
#include "gfx_sprites.h"
#include "live_screen.h"
#include "ImageLoader.h"
#include "sfx.h"
#include "bgm.h"
#include "DMod.h"
#include "paths.h"
#include "dinkini.h"
#include "freedinkedit.h"
#include "hardness_tiles.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

using Random = effolkronium::random_static;

// For the log window
ImGuiTextBuffer Buf;
ImGuiTextFilter Filter;
ImVector <int> LineOffsets;
bool AutoScroll = true;
// For transparent overlay and other window vars
static int corner = 2;
static int navcorn = 1;
char debugbuf[128];
int ts = 1;
int edit_edscale = 1;
int unit;
char sfbuf[128];
char titlebuf[128];
int tileselect;

bool drawhard;
bool globinfo;
int modorder;
int se;
int spri = 1;
int tindexscr = 1;
int mapprevscale = 1;

// Toggleable settings
bool render_tiles = true;
bool debug_meminfo;
bool debug_pinksquares;
bool debug_drawsprites = true;
bool debug_drawgame = true;
bool debug_gamewindow;
bool debug_paused;
bool debug_conpause;
bool debug_scripboot;
bool debug_input;
bool edit_nohit;
bool edit_hard;
bool edit_viewsprites = 1;
bool edit_fillhard = 1;
bool edit_minimapupdate;
bool edit_minimapbig;
bool edit_minimapbigrend;
bool edit_viewinvis;
bool curnodraw;
bool edit_renderinterp;
int edit_forbiddenseq = 0;
bool debug_texturefilter;
bool edit_drawtiles;
int tile_hide_col = 114;
bool edit_cornertile;
bool edit_drawvis;

// Toggles for windows
bool ultcheat;
bool graphicsopt;
bool dswitch = true;
bool logwindow;
bool keeb;
bool scripfo;
bool sprinfo;
bool varinfo;
bool sfxinfo;
bool metrics;
bool mapsquares;
bool maptiles;
bool sysinfo;
bool mapsprites;
bool spriedit;
bool mapprev;
bool sprioverlay;
bool spriplacer;
bool htileindex;
bool htiles;
bool sprvis;
bool ddatind;
bool screenprop;
bool bigmapprev;
bool spremove;
bool screentime;

// For the sprite placer
int spseq, spfrs = 1, spfre = 1, spxs, spxe, spys, spye, spruns, sphard, sptype;
char spscript[13];
bool allscreens;

// Sprite editor
bool sprprev;
bool autotouch;
bool edit_spboundaries;
static float boundcol[3] = {33.0 / 255, 41.0 / 255, 16.0 / 255};
enum Brain { Brainless, Human, Bounce, Duck, Pig, SeqCorpse, Repeat, SeqNoTrace, Text, EnemyDiag, EnemyLateral, MissileRepeat, Scale, Mouse, Button, Shadow, Smart, MissileOnce, Brain_COUNT };
const char* brain_names[Brain_COUNT] = { "No Brain", "Human/Dink", "Bounce", "Duck", "Pig", "Sequence (corpse)", "Repeat", "Sequence (no trace)", "Text", "Enemy: Diagonal", "Enemy: Lateral", "Missile: Repeat", "Scale", "Mouse", "Button", "Shadow", "NPC Smart", "Missile: once" };
bool slider_brain;

// sprie list
//int select = -1;

// Hard
int htile;

// For the editor itself
char msg[500];
int scr;
int num;
// Texture 3 holds the minimap. Texture 1 is for the sprite preview. 4 is for the full-size preview.
SDL_Texture *texture = NULL;
SDL_Texture *texture2 = NULL;
SDL_Texture *texture3 = NULL;
static SDL_Texture *texture4;
SDL_Texture *hard[800];

// For testing loading screens to RAM first for the minimap preview. Provides a marginal speed boost (1-2 seconds) at best
editor_screen ramscreens[769];
// 128 save states ought to be enough for anyone
editor_screen savestate[128];

// Slowly leaks RAM but not much so w/e
void render_sprite_preview() {
    if (cur_ed_screen.sprite[spri].seq > 1 && cur_ed_screen.sprite[spri].frame > 0 && seq[cur_ed_screen.sprite[spri].seq].frame[cur_ed_screen.sprite[spri].frame] != 0 && cur_ed_screen.sprite[spri].vision == 0) {
        //texture = SDL_CreateTexture(SDL_GetRenderer(g_display->window), g_display->getFormat(), SDL_TEXTUREACCESS_STREAMING, 600, 400);
        if (texture != NULL) {
        SDL_DestroyTexture(texture);
        }
        texture = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), GFX_k[seq[cur_ed_screen.sprite[spri].seq].frame[cur_ed_screen.sprite[spri].frame]].k->screenshot());
    }
    else {
    SDL_DestroyTexture(texture);
    }
}

void update_minimap_view() {
    if (texture3 != NULL)
        SDL_UpdateTexture(texture3, NULL, IOGFX_tmp2->screenshot()->pixels, IOGFX_tmp2->screenshot()->pitch);
    else
        texture3 = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), IOGFX_tmp2->screenshot());

}

void render_minimap_big() {
   // if (texture4 != NULL) {
    //SDL_UpdateTexture(texture4, NULL, source->pixels, source->pitch);
   // }
   // else {
        //texture4 = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), source);
   // }
    

}

// Used for the sprite preview to determine frames in seq
int get_seq_max_frames(int s) {
    int ul;
    for (int i=0; i < 70; i++) {
    if (seq[s].frame[i] != 0)
        ul++;
    }
    return ul;
}

//Loads all screens into RAM
void editor_load_screen_debug(int num) {
    if (load_screen_to(g_dmod.map.map_dat.c_str(), num, &ramscreens[num]) < 0)
        return;
    //fill_whole_hard();
}

void copy_screen_big(int x1, int y1) {
    //log_debug("X is %d, Y value is %d", x1, y1);
    {
        SDL_Rect src, dst;
        src.x = playl;
        src.y = 0;
        src.w = playx - playl;
        src.h = 400;
        dst.x = x1;
        dst.y = y1;
        dst.w = 600;
        dst.h = 400;
        IOGFX_tmp3->blit(IOGFX_background, &src, &dst);
    }  // yeolde: copies to the temp buffer so it may be seen separately. a hackup of shrink_screen
}

// Imgui stuff all starts here. Called from SW renderer.
void dbg_imgui(void) {
//Main Menu bar
ImGui::BeginMainMenuBar();
if (ImGui::BeginMenu("File"))
{
    //if (ImGui::MenuItem("Leave debug mode"))
    //log_debug_off();
    //ImGui::MenuItem("Pause", NULL, &debug_paused);
    if (ImGui::MenuItem("Quit (without saving)")) {
        SDL_Event ev;
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
    }
    ImGui::EndMenu();
}

if (ImGui::BeginMenu("Options")) {	
    ImGui::MenuItem("Bypass editor keyboard input", NULL, &debug_input);
    ImGui::MenuItem("Render editor to screen window", NULL, &debug_drawgame);
    //ImGui::MenuItem("Turn off texture filtering", NULL, &debug_texturefilter);
    ImGui::MenuItem("Auto-update minimap", NULL, &edit_minimapupdate);
    //ImGui::MenuItem("Minimap render screen boundaries only", NULL, &edit_minimapbigrend);
    ImGui::MenuItem("Draw current tile in screen corner", NULL, &edit_cornertile);
    if (ImGui::MenuItem("Draw Sprites", NULL, &edit_viewsprites) && editor_mode == 4)
    draw_screen_editor();
    if (ImGui::MenuItem("Draw Invisible Sprites", NULL, &edit_viewinvis) && editor_mode == 4)
    draw_screen_editor();
    ImGui::MenuItem("Draw hard on screen load", NULL, &edit_fillhard);
    if (ImGui::MenuItem("Draw all visions", NULL, &edit_drawvis) && editor_mode == 4)
    draw_screen_editor();
    //ImGui::MenuItem("Sprite 1 Nodraw", NULL, &curnodraw);
    //ImGui::MenuItem("Tiles and background", NULL, &render_tiles);
    ImGui::Separator();
    if (ImGui::MenuItem("Set window to 640x480")) {
        SDL_SetWindowSize(g_display->window, 640, 480);
        SDL_SetWindowPosition(g_display->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    if (ImGui::MenuItem("Set window to 1280x960")) {
    SDL_SetWindowSize(g_display->window, 1280, 960);
    SDL_SetWindowPosition(g_display->window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    ImGui::EndMenu();
}

if (ImGui::BeginMenu("Windows"))
{
    
    ImGui::MenuItem("Editor pop-out", NULL, &debug_gamewindow);
    ImGui::MenuItem("Minimap", NULL, &mapprev);
    //ImGui::MenuItem("Screen boundaries preview...", NULL, &bigmapprev);
    ImGui::MenuItem("Debug Log", NULL, &logwindow);
    ImGui::MenuItem("Dink.dat", NULL, &ddatind);
    ImGui::MenuItem("System Information", NULL, &sysinfo);
    ImGui::EndMenu();
}
/*
if (ImGui::BeginMenu("Log Level"))
{
    if (ImGui::MenuItem("Errors only"))
        log_debug_on(0);
    if (ImGui::MenuItem("Standard"))
        log_debug_on(1);
    if (ImGui::MenuItem("Verbose"))
        log_debug_on(2);

    ImGui::EndMenu();
}
*/

//if (ImGui::BeginMenu("Tiles"))
//{
    //ImGui::MenuItem("Tile Preview", NULL, &maptiles);
    //ImGui::MenuItem("Tile Index", NULL, &mapsquares);
    //

//	ImGui::EndMenu();
//}

if (ImGui::BeginMenu("Sprites"))
{
    //ImGui::MenuItem("Preview Sequences", NULL, &mapsprites);
    ImGui::MenuItem("Edit...", NULL, &spriedit);
    ImGui::MenuItem("List...", NULL, &sprinfo);
    //ImGui::MenuItem("Visual Sprite List", NULL, &sprvis);
    ImGui::MenuItem("Batch add...", NULL, &spriplacer);
    ImGui::Separator();
    ImGui::MenuItem("Default Nohit", NULL, &edit_nohit);
    ImGui::MenuItem("Default Hard", NULL, &edit_hard);

    ImGui::EndMenu();
}

if (ImGui::BeginMenu("Screen")) {
    if (editor_mode < 4)
    ImGui::BeginDisabled();
    ImGui::MenuItem("Screen Properties...", NULL, &screenprop);
    if (ImGui::MenuItem("Redraw"))
    draw_screen_editor();
    if (ImGui::MenuItem("Update on minimap")) {
        copy_front_to_two();
        shrink_screen_to_these_cords((scr - 1) * 20 -((((scr - 1) / 32) * 640)), ((((scr - 1) / 32)) * 20));
    }
    if (ImGui::MenuItem("Hide tiles", NULL, &edit_drawtiles))
    draw_screen_editor();
    
    ImGui::MenuItem("Show Collision Overlaid", "Space", &drawhard);
    if (ImGui::MenuItem("Reset forbidden sequence")) {
        edit_forbiddenseq = 0;
    }

    if (editor_mode < 4)
    ImGui::EndDisabled();

    ImGui::EndMenu();
}

if (ImGui::BeginMenu("Hard.dat"))
{
    ImGui::MenuItem("Tiles...", NULL, &htiles);
    ImGui::MenuItem("Tile index...", NULL, &htileindex);
    ImGui::EndMenu();
}
ImGui::EndMainMenuBar();

//ImGui::ShowDemoWindow();

//Transparent overlay info window, stolen from the demo
if (dswitch) {
ImGuiIO& io = ImGui::GetIO();
ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
if (corner != -1)
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin("Example: Simple overlay", NULL, window_flags))
    {
        //ImGui::Text("Debug mode enabled. Press Alt+D to disable");
        //ImGui::Separator();
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("%s", msg);
        //if (ImGui::IsMousePosValid())
        ImGui::Text("Cursor Position: (%d,%d)", spr[1].x, spr[1].y);
        //else
        //    ImGui::Text("Mouse Position: <invalid>");
        ImGui::Text("Screen: %i", scr);
        //ImGui::SameLine();
        //if (ImGui::Button("<-")) {
        //	if (g_dmod.map.loc[scr-1] != 0)
        //	editor_load_screen_debug(scr-1);
        //	scr--;
        //	draw_screen_editor();
        //}
        if (debug_input)
        ImGui::TextColored(ImVec4((int)255,0,0,125), "Editor Input Bypassed!");
        ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom",       NULL, corner == -1)) corner = -1;
            if (ImGui::MenuItem("Top-left",     NULL, corner == 0)) corner = 0;
            if (ImGui::MenuItem("Top-right",    NULL, corner == 1)) corner = 1;
            if (ImGui::MenuItem("Bottom-left",  NULL, corner == 2)) corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, corner == 3)) corner = 3;
            if (ImGui::MenuItem("Close")) dswitch = false;
            ImGui::EndPopup();
        }

    }
    ImGui::End();
}
//Keyboard state and gamepad
if (keeb) {
ImGui::Begin("Keyboard and Joystick", &keeb);
int x;
for (x = 0; x < 256; x++)
    if (input_getcharstate(x))
    ImGui::Text("Key %i", x);
    if (jinfo) {
            ImGui::Text("Name: %s", SDL_JoystickName(jinfo));
            ImGui::Text("Controller rumble available: %d", SDL_JoystickHasRumble(jinfo));
            ImGui::Text("Power level: %d", SDL_JoystickCurrentPowerLevel(jinfo));
            if (ImGui::Button("Test rumble")) {
                SDL_JoystickRumble(jinfo, 0xFFFF, 0xFFFF, 800);
                }
            }
    else {
        ImGui::Text("Joystick/Gamepad not available");
    }
ImGui::End();
}

//Log Window
if (logwindow) {
ImGui::Begin("Log", &logwindow);
ImGui::Text("Log Level:");
ImGui::SameLine();
if (ImGui::Button("Errors Only"))
log_debug_on(0);
ImGui::SameLine();
if (ImGui::Button("All Debug and Application Info"))
log_debug_on(1);
ImGui::SameLine();
if (ImGui::Button("Verbose"))
log_debug_on(2);
ImGui::Separator();
ImGui::Text("Log:");
ImGui::SameLine();
ImGui::Checkbox("Meminfo", &debug_meminfo);
ImGui::SameLine();
ImGui::Separator();
ImGui::Checkbox("Auto-scroll", &AutoScroll);
ImGui::SameLine();
if (ImGui::Button("Clear"))
memset(&Buf, 0, sizeof(debugbuf));
ImGui::SameLine();
if (ImGui::Button("Copy last message to clipboard")) {
ImGui::LogToClipboard();
ImGui::LogText(log_getLastLog());
ImGui::LogFinish();
ImGui::Separator();
}
ImGui::Separator();
//Child window with text in it
ImGui::BeginChild("scrolling");
const char* buf = Buf.begin();
const char* buf_end = Buf.end();
ImGui::TextUnformatted(buf, buf_end);
if (AutoScroll)
ImGui::SetScrollHereY(1.0f);

ImGui::EndChild();
ImGui::End();
}
//Sprite Information
if (sprinfo) {
    ImGui::Begin("Screen Sprites", &sprinfo, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::Button("Remove all screen sprites")) {
        for (int i=1; i<100; i++)
        cur_ed_screen.sprite[i].active = 0;
        
        draw_screen_editor();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Equivalent to pressing Alt+Delete in sprite mode");
    ImGui::SameLine();
    if (ImGui::Button("Remove off-screen sprites")) {
        for (int j=1; j<100; j++)
        if (cur_ed_screen.sprite[j].x < 0 || cur_ed_screen.sprite[j].x > 620 || cur_ed_screen.sprite[j].y > 400)
        cur_ed_screen.sprite[j].active = 0;
        
        draw_screen_editor();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Be careful if you have screenmatched sprites");
    ImGui::SameLine();
    if (ImGui::Button("Remove duplicate sprites")) {
        for (int m=1; m<100;m++) {
            for (int n=1; n<100; n++) {
                if (memcmp(&cur_ed_screen.sprite[m], &cur_ed_screen.sprite[n], sizeof(cur_ed_screen.sprite[n])) == 0 && cur_ed_screen.sprite[n].active == 1 && (n != m))
                //log_debug("Sprite is %d vs %d and equal", m, n);
                cur_ed_screen.sprite[n].active = 0;
            }
        }
        draw_screen_editor();
    }

    if (ImGui::Button("Randomise X and Y placement values")) {
        for (int v=1; v<100; v++) {
            if (cur_ed_screen.sprite[v].active == 1) {
                cur_ed_screen.sprite[v].x = Random::get(1, 620);
                cur_ed_screen.sprite[v].y = Random::get(1, 400);
            }
        }
        draw_screen_editor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Recover removed sprites")) {
        for (int k=1; k<100; k++) {
        if (cur_ed_screen.sprite[k].active == 0 && cur_ed_screen.sprite[k].x != 0)
        cur_ed_screen.sprite[k].active = 1;
        }
        draw_screen_editor();
    }
    if (ImGui::BeginTable("table1", 16, ImGuiTableFlags_Borders | ImGuiTableFlags_Reorderable)) {
        ImGui::TableSetupColumn("No.");
        ImGui::TableSetupColumn("x");
        ImGui::TableSetupColumn("y");
        ImGui::TableSetupColumn("Seq");
        ImGui::TableSetupColumn("Frame");
        ImGui::TableSetupColumn("Timing");
        ImGui::TableSetupColumn("Script");
        ImGui::TableSetupColumn("Brain");
        ImGui::TableSetupColumn("HP");
        //ImGui::TableSetupColumn("Direction");
        ImGui::TableSetupColumn("que");
        //ImGui::TableSetupColumn("Freeze");
        ImGui::TableSetupColumn("Size");
        ImGui::TableSetupColumn("Special");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Vision");
        ImGui::TableSetupColumn("Nohit");
        ImGui::TableSetupColumn("Hard");
        ImGui::TableHeadersRow();
        for (int x = 1; x < 100; ++x) {
            if (cur_ed_screen.sprite[x].active) {
                ImGui::TableNextColumn();
                ImGui::Text("%d", x);
                if (x == spri)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImVec4(0.7f, 0.3f, 0.3f, 0.65f)));
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].x);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].y);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].seq);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].frame);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].timing);
                ImGui::TableNextColumn();
                //ImGui::Text("%s", rinfo[cur_ed_screen.sprite[x].script]->name);
                ImGui::Text("%s", cur_ed_screen.sprite[x].script);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].brain);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].hitpoints);
                ImGui::TableNextColumn();
                //ImGui::Text("%d", cur_ed_screen.sprite[x].dir);
                //ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].que);
                ImGui::TableNextColumn();
                //ImGui::Text("%d", cur_ed_screen.sprite[x].freeze);
                //ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].size);
                ImGui::TableNextColumn();
                //ImGui::Text("%s", cur_ed_screen.sprite[x].text);
                //ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].special);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].type);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].vision);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].nohit);
                ImGui::TableNextColumn();
                ImGui::Text("%d", cur_ed_screen.sprite[x].hard);
            }
        }
        
    
    ImGui::EndTable();
    ImGui::End();
    }
}

if (sysinfo) {
    ImGui::Begin("System Information", &sysinfo, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::BulletText("Dear ImGui Release: %s", IMGUI_VERSION);
    ImGui::Text("SDL Revision: %s", SDL_GetRevision());
    ImGui::Text("Platform: %s", SDL_GetPlatform());
    ImGui::Text("Video driver: %s", SDL_GetCurrentVideoDriver());
    ImGui::Text("Audio driver: %s", SDL_GetCurrentAudioDriver());
    ImGui::Text("System RAM: %iMiB", SDL_GetSystemRAM());
    ImGui::Spacing();
	ImGui::SeparatorText("Paths");
	ImGui::BulletText("Name is: %s", paths_getdmodname());
	ImGui::BulletText("Location is: %s", paths_getdmoddir());
	ImGui::BulletText("Fallback is: %s", paths_getfallbackdir());
	ImGui::BulletText("Ancillary data is: %s", paths_getpkgdatadir());
	ImGui::Separator();
	if (ImGui::SmallButton("Okay"))
	    sysinfo = false;
    ImGui::End();
}

if (maptiles) {
    ImGui::Begin("Tilescreens", &maptiles);
    if (ImGui::SliderInt("Screen", &ts, 1, 41))
        texture2 = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), g_dmod.bgTilesets.slots[ts]->screenshot());
    ImGui::Image(texture2, ImVec2((int)g_dmod.bgTilesets.slots[ts]->w, g_dmod.bgTilesets.slots[ts]->h));
    //for (int i = 1; i < 96; i++) {
    //	
    //	ImGui::Text("%i", cur_ed_screen.t[i].square_full_idx0);
    //}
    ImGui::End();
}

if (mapprev) {
    ImGui::Begin("Map preview", &mapprev, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize);
    if (texture3 == NULL)
        ImGui::Text("You must still press space to render the minimap");
    else
    ImGui::Text("Last render time was %i seconds", render_time/1000);
    if (ImGui::Button("1x"))
    mapprevscale = 1;
    ImGui::SameLine();
    if (ImGui::Button("2x"))
    mapprevscale = 2;
    //if (ImGui::Button("Redraw texture")) 
    //{
    //	//SDL_UpdateTexture()
    //	SDL_DestroyTexture(texture3);
    //	texture3 = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), IOGFX_tmp2->screenshot());			
    //}
    //ImGui::SameLine();
    //if (ImGui::Button("Destroy Texture"))
        
        
    ImGui::Image(texture3, ImVec2((int)640*mapprevscale,480*mapprevscale));
    ImGui::End();
}

if (bigmapprev) {
    ImGui::Begin("Full-size preview", &bigmapprev);
    ImGui::Text("Re-render the minimap while on the screen you want to see the boundaries of");
    if (ImGui::Button("Refresh preview")) 
    {
        render_minimap_big();		
    }
    //ImGui::Text("%d", IOGFX_tmp1->getMemUsage());
    ImGui::Image(texture4, ImVec2((int)1280,960));
    ImGui::End();
}

//Do not use!
if (mapsprites) {
    ImGui::Begin("Sprites", &mapsprites);
    //struct editor_screen debug_preview;
    if (ImGui::SliderInt("Sprite lot", &num, 1, 1000)) {
        //draw15_debug(num);
        texture2 = SDL_CreateTextureFromSurface(SDL_GetRenderer(g_display->window), IOGFX_tmp1->screenshot());
    }
    ImGui::Image(texture2, ImVec2((int)640, 480));
    //ImGuiIO& io = ImGui::GetIO();
    //ImVec2 pos = ImGui::GetCursorScreenPos();
/* 	if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            float region_sz = 32.0f;
            float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
                float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
                float zoom = 4.0f;
                if (region_x < 0.0f) { region_x = 0.0f; }
                else if (region_x > IOGFX_tmp1->w - region_sz) { region_x = IOGFX_tmp1->w - region_sz; }
                if (region_y < 0.0f) { region_y = 0.0f; }
                else if (region_y > IOGFX_tmp1->h - region_sz) { region_y = IOGFX_tmp1->h - region_sz; }
                ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
                ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
                ImVec2 uv0 = ImVec2((region_x) / IOGFX_tmp1->w, (region_y) / IOGFX_tmp1->h);
                ImVec2 uv1 = ImVec2((region_x + region_sz) / IOGFX_tmp1->w, (region_y + region_sz) / IOGFX_tmp1->h);
                ImGui::Image(texture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1);
                ImGui::EndTooltip();
            }
 */
    ImGui::End();
    
}

if (mapsquares) {
}

if (spriedit && editor_mode > 2) {
    ImGui::Begin("Sprite Edit", &spriedit, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::SetNextItemWidth(40);
    if (ImGui::DragInt("Number", &spri, 0.1f, 1, 99))
    render_sprite_preview();
    ImGui::SameLine();
    ImGui::PushButtonRepeat(true);
    if (ImGui::ArrowButton("##left", ImGuiDir_Left)) { spri--; render_sprite_preview();}
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right", ImGuiDir_Right)) { spri++; render_sprite_preview();}
    ImGui::PopButtonRepeat();
    ImGui::SameLine();
    if (ImGui::Button("1")) {
    spri = 1;
    render_sprite_preview();
    }
    //Tooltip
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Return to sprite index 1");
    }
    ImGui::SameLine();
    if (ImGui::Button("Touched")) {
        spri = cur_sprite;
        render_sprite_preview();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Jump to the last sprite you clicked on");
    }
    ImGui::SameLine();
    ImGui::Checkbox("Autotouch", &autotouch);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Autojump to last touched sprite");
    if (spri < 1 || spri > 99)
    spri = 1;
    ImGui::SameLine();
    //Button to activate/remove sprite
    if (cur_ed_screen.sprite[spri].active == 1)
    {
    if (ImGui::Button("Remove"))
        cur_ed_screen.sprite[spri].active = 0;
    }
    else {
    if (ImGui::Button("Activate"))
        cur_ed_screen.sprite[spri].active = 1;
    }
    ImGui::SameLine();
    if (ImGui::Button("Make copy")) {
        for (int i=1; i<100; i++) {
            if (cur_ed_screen.sprite[i].active == 0)  {
            memcpy(&cur_ed_screen.sprite[i], &cur_ed_screen.sprite[spri], sizeof(cur_ed_screen.sprite));
            spri = i;
            break;
            }
        }
    }
    //if (ImGui::Checkbox("Preview", &sprprev))
    //render_sprite_preview();
    if (ImGui::IsItemHovered()) 
        ImGui::SetTooltip("See what your sprite looks like, maybe");    
    ImGui::SameLine();
    ImGui::Checkbox("See Boundaries", &edit_spboundaries);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Show sprite boundaries as if pressing [ or ]");
    ImGui::SameLine();
    ImGui::ColorEdit3("Boundary Colour", boundcol, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoInputs);
    ImGui::Separator();
        //Sprite positioning
    ImGui::SliderInt("X", &cur_ed_screen.sprite[spri].x,0,600);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left1px", ImGuiDir_Left)) { cur_ed_screen.sprite[spri].x--;}
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right1px", ImGuiDir_Right)) { cur_ed_screen.sprite[spri].x++;}
    ImGui::SliderInt("Y", &cur_ed_screen.sprite[spri].y,0,400);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##up1px", ImGuiDir_Up)) { cur_ed_screen.sprite[spri].y--;}
    ImGui::SameLine();
    if (ImGui::ArrowButton("##down1px", ImGuiDir_Down)) { cur_ed_screen.sprite[spri].y++;}
    ImGui::SliderInt("Size", &cur_ed_screen.sprite[spri].size, 1, 300);
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
        cur_ed_screen.sprite[spri].size = 100;
    ImGui::InputText("Script", cur_ed_screen.sprite[spri].script, sizeof(cur_ed_screen.sprite[spri].script));
    ImGui::SameLine();
    //ImGui::Checkbox("Input", &debug_input);
    if (ImGui::Button("Clear"))
    //yeah
    strcpy(cur_ed_screen.sprite[spri].script, "             ");
    ImGui::RadioButton("Type 0 (background)", &cur_ed_screen.sprite[spri].type, 0);
    ImGui::SameLine();
    ImGui::RadioButton("1 (normal)", &cur_ed_screen.sprite[spri].type, 1);
    ImGui::SameLine();
    ImGui::RadioButton("2 (invisible)", &cur_ed_screen.sprite[spri].type, 2);
    ImGui::RadioButton("Hard on", &cur_ed_screen.sprite[spri].hard, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Hard off", &cur_ed_screen.sprite[spri].hard, 1);
    ImGui::RadioButton("Nohit on", &cur_ed_screen.sprite[spri].nohit, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Nohit off", &cur_ed_screen.sprite[spri].nohit, 0);
    //Seq and frame and preview underneath
    if (ImGui::DragInt("Sequence", &cur_ed_screen.sprite[spri].seq, 0.1f,0,1000)) {
        sprprev = 0;
    }
    ImGui::SameLine();
    if (ImGui::Button("Forbid")) {
        edit_forbiddenseq = cur_ed_screen.sprite[spri].seq;
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Sequence will not be drawn while editing");
    
    //Get max frames in sequence
    int ul = get_seq_max_frames(cur_ed_screen.sprite[spri].seq);
    if (ul == 0 && cur_ed_screen.sprite[spri].vision == 0)
    ImGui::TextColored(ImVec4(1.0f, 0, 0, 1.0f), "Invalid Sequence!");
    if (ImGui::SliderInt("Frame", &cur_ed_screen.sprite[spri].frame, 1, ul+1)) {
    if (seq[cur_ed_screen.sprite[spri].seq].frame[cur_ed_screen.sprite[spri].frame] == 0) {
        cur_ed_screen.sprite[spri].frame = 1;
        }
        render_sprite_preview();
    }

    
    //Only display the preview if there's a frame selected, and it's on the default vision, and spractive
    if (cur_ed_screen.sprite[spri].frame > 0 && cur_ed_screen.sprite[spri].vision == 0 && cur_ed_screen.sprite[spri].active == 1 && sprprev)
    ImGui::Image(texture, ImVec2((int)GFX_k[seq[cur_ed_screen.sprite[spri].seq].frame[cur_ed_screen.sprite[spri].frame]].k->w, GFX_k[seq[cur_ed_screen.sprite[spri].seq].frame[cur_ed_screen.sprite[spri].frame]].k->h));
    ImGui::SliderInt("Group", &cur_ed_screen.sprite[spri].special, 0, 8);
    ImGui::Separator();
    if (ImGui::TreeNode("Miscellaneous Parameters")) {
    //For brain settings and other things
    ImGui::PushItemWidth(195);
    const char* brain_name = (cur_ed_screen.sprite[spri].brain >= 0 && cur_ed_screen.sprite[spri].brain < Brain_COUNT) ? brain_names[cur_ed_screen.sprite[spri].brain] : "Unknown/Unused";
    if (!slider_brain) {
    ImGui::SliderInt("Brain    ", &cur_ed_screen.sprite[spri].brain, 0, Brain_COUNT, brain_name); 
    ImGui::SameLine();
    }
    else
    ImGui::DragInt("Brain    ", &cur_ed_screen.sprite[spri].brain, 0.1f, 0, 100); ImGui::SameLine();
    ImGui::SliderInt("Depth cue", &cur_ed_screen.sprite[spri].que, -1000, 1000); 
    ImGui::SliderInt("Vision   ", &cur_ed_screen.sprite[spri].vision, 0, 8); ImGui::SameLine();
    ImGui::SliderInt("Timing", &cur_ed_screen.sprite[spri].timing, 0, 100); 
    ImGui::SliderInt("Speed    ", &cur_ed_screen.sprite[spri].speed, 0, 5); ImGui::SameLine();
    ImGui::SliderInt("Sound", &cur_ed_screen.sprite[spri].sound, 0, 50);
    //
    ImGui::DragInt("Hitpoints", &cur_ed_screen.sprite[spri].hitpoints, 0.5f, 0, 1000); ImGui::SameLine();
    ImGui::DragInt("Strength", &cur_ed_screen.sprite[spri].strength, 0.5f, 0, 1000);
    ImGui::DragInt("Defense  ", &cur_ed_screen.sprite[spri].defense, 0.5f, 0, 100); ImGui::SameLine();
    ImGui::DragInt("Exp", &cur_ed_screen.sprite[spri].exp, 0.4f, 0, 500);
    ImGui::DragInt("Gold     ", &cur_ed_screen.sprite[spri].gold, 0.5f, 0, 1000); ImGui::SameLine();
    ImGui::DragInt("Touch damage", &cur_ed_screen.sprite[spri].touch_damage, 0.3f, -1, 1000);
    ImGui::PopItemWidth();
    ImGui::Checkbox("Alternate Brain Input", &slider_brain);
    ImGui::TreePop();
    }
    if (ImGui::TreeNode("Warp Parameters")) {
    ImGui::Text("Use the Alt+W trick to fill these values");
    ImGui::RadioButton("Warp on", &cur_ed_screen.sprite[spri].is_warp, 1);
    ImGui::SameLine();
    ImGui::RadioButton("Warp off", &cur_ed_screen.sprite[spri].is_warp, 0);
    ImGui::SliderInt("Warp map", &cur_ed_screen.sprite[spri].warp_map, 1, 768);
    ImGui::SliderInt("Warp X", &cur_ed_screen.sprite[spri].warp_x, 20, 619);
    ImGui::SliderInt("Warp Y", &cur_ed_screen.sprite[spri].warp_y, 1, 400);
    ImGui::DragInt("Warp/touch sequence", &cur_ed_screen.sprite[spri].parm_seq, 0.5f, -1, 1000);
    ImGui::SameLine();
    if (ImGui::Button("Set to seq"))
        cur_ed_screen.sprite[spri].parm_seq = cur_ed_screen.sprite[spri].seq;
    if (cur_ed_screen.sprite[spri].is_warp == 1 && cur_ed_screen.sprite[spri].hard == 1)
    ImGui::TextColored(ImVec4(1.0f,0,0,1.0f), "Warp must have hard on to activate");
    
    if (ImGui::Button("Reset warp parameters")) {
        cur_ed_screen.sprite[spri].is_warp = 0;
        cur_ed_screen.sprite[spri].warp_map = 0;
        cur_ed_screen.sprite[spri].warp_x = 0;
        cur_ed_screen.sprite[spri].warp_y = 0;
        cur_ed_screen.sprite[spri].parm_seq = 0;
    }
    ImGui::TreePop();
    }
    if (ImGui::TreeNode("Trimming dimensions")) {
    //ImGui::Text("It's preferable to still use Z and X");
    ImGui::DragInt("Trim left", &cur_ed_screen.sprite[spri].alt.left, 0.2f, -600,600);
    ImGui::DragInt("Trim top", &cur_ed_screen.sprite[spri].alt.top, 0.2f, -600, 600);
    ImGui::Text("These two control how much of the sprite is displayed");
    ImGui::DragInt("Trim right", &cur_ed_screen.sprite[spri].alt.right, 0.2f, -600, 600);
    ImGui::DragInt("Trim bottom", &cur_ed_screen.sprite[spri].alt.bottom, 0.2f, -600, 600);
    if (ImGui::Button("Reset trim")) {
        cur_ed_screen.sprite[spri].alt.left = 0;
        cur_ed_screen.sprite[spri].alt.top = 0;
        cur_ed_screen.sprite[spri].alt.right = 0;
        cur_ed_screen.sprite[spri].alt.bottom = 0;
    }
    ImGui::TreePop();
    }
    if (ImGui::TreeNode("Base Sequences")) {
    ImGui::InputInt("Base walk", &cur_ed_screen.sprite[spri].base_walk, 0.1f, -1, 1000);
    ImGui::InputInt("Base hit", &cur_ed_screen.sprite[spri].base_hit, 0.1f, 0, 1000);
    ImGui::InputInt("Base attack", &cur_ed_screen.sprite[spri].base_attack, 0.1f, 0, 1000);
    ImGui::InputInt("Base die", &cur_ed_screen.sprite[spri].base_die, 0.1f, 0, 1000);
    ImGui::InputInt("Base idle", &cur_ed_screen.sprite[spri].base_idle, 0.1f, 0, 1000);
    ImGui::TreePop();
    }
    // This is intensive. Might turn it off eventually
    if (editor_mode == 4)
    draw_screen_editor();

    if (autotouch)
    spri = cur_sprite;
    ImGui::End();
}

//Sprite placer
if (spriplacer) {
    ImGui::Begin("Batch Sprite Placer", &spriplacer);
    ImGui::DragInt("Sequence", &spseq, 0.1f, 0, 1000);
    int ul = get_seq_max_frames(spseq);
    ImGui::DragInt("Frame start", &spfrs, 0.1f, 1, 50);
    ImGui::SliderInt("Frame end", &spfre, 1, ul);
    ImGui::InputText("Script", spscript, sizeof(spscript));
    ImGui::RadioButton("Hard on", &sphard, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Hard off", &sphard, 1);
    ImGui::RadioButton("Type 0 (background)", &sptype, 0);
    ImGui::SameLine();
    ImGui::RadioButton("1 (normal)", &sptype, 1);
    ImGui::SameLine();
    ImGui::RadioButton("2 (invisible)", &sptype, 2);
    ImGui::SliderInt("Starting X", &spxs, 0, 620);
    ImGui::SliderInt("Ending X", &spxe, 0, 620);
    ImGui::VSliderInt("Starting Y", ImVec2(18, 160), &spys, 400, 0);
    ImGui::SameLine();
    ImGui::VSliderInt("Ending Y", ImVec2(18, 160), &spye, 400, 0);
    ImGui::Separator();
    ImGui::SliderInt("Sprites to generate", &spruns, 0, 99);
    ImGui::SameLine();
    //ImGui::Checkbox("Run on all screens", &allscreens);
    if (ul < 1 || spxs > spxe || spye < spys) {
        ImGui::TextColored(ImVec4(1.0f, 0.0,0.0,0.5f), "Invalid Parameters!");
        ImGui::BeginDisabled();
    }

    if (ImGui::Button("Place sprites")) {
        for (int k=0; k < spruns; k++) {
            for (int i=1; i<100; i++) {
                //Find an empty sprite index
                if (cur_ed_screen.sprite[i].active == 0) {
                    cur_ed_screen.sprite[i].active = 1;
                    cur_ed_screen.sprite[i].type = 1;
                    cur_ed_screen.sprite[i].size = 100;
                    cur_ed_screen.sprite[i].seq = spseq;
                    cur_ed_screen.sprite[i].frame = Random::get(spfrs, spfre);
                    cur_ed_screen.sprite[i].hard = sphard;
                    cur_ed_screen.sprite[i].x = Random::get(spxs, spxe);
                    cur_ed_screen.sprite[i].y = Random::get(spys, spye);
                    cur_ed_screen.sprite[i].type = sptype;
                    cur_ed_screen.sprite[i].timing = 33;
                    cur_ed_screen.sprite[i].brain = 0;
                    memcpy(&cur_ed_screen.sprite[i].script, spscript, sizeof(spscript));
                    
                    break;

                }
            }
    }
    draw_screen_editor();
    }
    if (ul < 1 || spxs > spxe || spye < spys)
        ImGui::EndDisabled();
    
    ImGui::End();
}

if (htileindex) {
    ImGui::Begin("Hard.dat Tile Index", &htileindex);
    ImGui::SetNextItemWidth(50);
    ImGui::DragInt("Select Tilescreen", &tindexscr, 0.2f, 1, 41);
    ImGui::SameLine();
    if (ImGui::Button("Clear indices from tilescreen")) {
        for (int i=128 * (tindexscr -1); i < 128 * tindexscr; i++)
        hmap.btile_default[i] = 0;
        save_hard();
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Be careful!");
    if (ImGui::Button("Reset bogus index values to zero")) {
        for (int i=0; i<5248; i++) {
            if (hmap.btile_default[i] < 0 || hmap.btile_default[i] > 799)
            hmap.btile_default[i] = 0;
        }
    }
    if (ImGui::BeginTable("table9", 2, ImGuiTableRowFlags_Headers)) {
        ImGui::TableSetupColumn("Tile");
        ImGui::TableSetupColumn("Index");
        ImGui::TableHeadersRow();
    for (int i=(tindexscr - 1) * 128; i< ((tindexscr - 1) * 128) + 128; i++) {
        ImGui::TableNextColumn();
    ImGui::Text("%d", i);
    ImGui::TableNextColumn();
    ImGui::Text("%hd", hmap.btile_default[i]);
    }
    ImGui::EndTable();
    }
    ImGui::End();
}

if (ddatind) {
    ImGui::Begin("Dink.dat Indices", &ddatind, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::RadioButton("Inside", &g_dmod.map.indoor[scr], 1);
    ImGui::SameLine();
    ImGui::RadioButton("Outside", &g_dmod.map.indoor[scr], 0);
    ImGui::InputInt("MIDI Number", &g_dmod.map.music[scr]);
    ImGui::InputInt("Map.dat screen", &g_dmod.map.loc[scr]);
    if (ImGui::Button("Save changes")) {
        g_dmod.map.save();
    }
    ImGui::Separator();
    ImVec4 current = ImVec4(0,1,0,1);
    ImVec4 used = ImVec4(0.8f, 0.1f, 0.2f, 1.0f);
    ImVec4 empty = ImVec4(0.0f, 0.1f, 1.0f, 1.0f);
    ImVec4 col;
    if (ImGui::BeginTable("table3", 32, ImGuiTableFlags_Borders))
{
    for (int item = 1; item < 769; item++)
    {
        ImGui::TableNextColumn();
        if (item == scr)
        col = current;
        else if (g_dmod.map.loc[item] > 0)
        col = used;
        else
        col = empty;

        ImGui::TextColored(col, "%i", g_dmod.map.loc[item]);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Screen: %i, Indoor: %i, Music: %i", item, g_dmod.map.indoor[item], g_dmod.map.music[item]);
    }
    ImGui::EndTable();
}
ImGui::End();
}

if (screenprop) {
    ImGui::Begin("Screen properties", &screenprop, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::InputText("Script", cur_ed_screen.script, sizeof(cur_ed_screen.script));
    //ImGui::SameLine();
    //ImGui::Checkbox("Enable text input", &debug_input);
    if (ImGui::SliderInt("Tile hide colour", &tile_hide_col, 0, 255) && edit_drawtiles)
        draw_screen_editor();
    ImGui::Separator();
    ImGui::Text("Tile grid");
    ImGui::SetNextItemWidth(50);
    ImGui::DragInt("Individual Tile Index", &tileselect, 0.5f, 0, 5216);
    ImGui::SameLine();
    if (ImGui::Button("Set all tiles")) {
        for (int i=0; i<96; i++)
            cur_ed_screen.t[i].square_full_idx0 = tileselect;
        draw_screen_editor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Set all alt-hard")) {
        for (int i=0; i<96; i++)
            cur_ed_screen.t[i].althard = tileselect;
        draw_screen_editor();
    }
    //if (ImGui::Button("Set Random Tiles")) {
    //	for (int p=1; p< 97; p++)
    //	cur_ed_screen.t[p].square_full_idx0 = rand() % 3936;
    //}
    if (ImGui::BeginTable("table15", 12, ImGuiTableFlags_Borders)) {
        for (int i=0; i < 96; i++) {
            ImGui::TableNextColumn();
            ImGui::Text("%d", i+1);
            //ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Index: %d", cur_ed_screen.t[i].square_full_idx0);
            //ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Alt: %d", cur_ed_screen.t[i].althard);
        }

ImGui::EndTable();
}
        
        ImGui::End();
}

if (htiles) {
    ImGui::Begin("Hard.dat Tile Viewer", &htiles, ImGuiWindowFlags_AlwaysAutoResize);
    //ImGui::Text("These are actually the wrong way round");
    ImVec4 standard = ImVec4(0.97f, 0.86f, 0.38f, 1.0f);
    ImVec4 low = ImVec4(0.3f, 0.4f, 1.0f, 1.0f);
    ImVec4 fire = ImVec4(1.0f, 0.0f, 0.2f, 1.0f);
    ImVec4 col;
    ImGui::SliderInt("Tile", &htile, 0, 799);
    ImGui::SameLine();
    ImGui::DragInt("##hardtile", &htile, 0.1f, 0, 799);
    ImGui::BeginTable("table9", 50);
    int y = 0;
    for (y = 0; y < 50; y++) {
        int x = 0;
        for (x = 0; x < 50; x++) {
            if (hmap.htile[htile].hm[x][y] == 1)
                col = standard;
            else if (hmap.htile[htile].hm[x][y] == 2)
                col = low;
            else if (hmap.htile[htile].hm[x][y] == 3)
                col = fire;
            if (hmap.htile[htile].hm[x][y] > 0)
            ImGui::TextColored(col, "%d", hmap.htile[htile].hm[x][y]);
            else
            ImGui::Text("0");
            ImGui::TableNextColumn();
    }
    ImGui::TableNextRow();
    }
    ImGui::EndTable();
    ImGui::End();
}

if (sprvis) {
    ImGui::Begin("Live Sprite Viewer", &sprvis);
    ImGui::Text("%d", spr[1].x);
    ImGui::Text("%d", spr[1].nodraw);
    ImGui::End();
}

//ImGui::Begin("Load screens to RAM");
//if (ImGui::Button("Load screens")) {
//	for (int i=1; i<769; i++)
//	editor_load_screen_debug(i);
//}
//ImGui::End();


//Conditional stuff that only displays sometimes
//refactor this later
//Only display when in tile mode as the keybinds interfere otherwise
if (editor_mode == 4) {
ImGuiIO& io = ImGui::GetIO();
ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
ImGui::Begin("Map screen navigator");

ImGui::Indent();
if (ImGui::ArrowButton("##uponescreen", ImGuiDir_Up)) {
    if (g_dmod.map.loc[scr - 32] > 0 && ((scr - 32) > 0)) {
        editor_save_screen(g_dmod.map.loc[scr]);
       editor_load_screen(g_dmod.map.loc[scr-32]); 
       lsm_kill_all_nonlive_sprites();
       draw_screen_editor();
       scr -= 32;
    }
}
ImGui::Unindent();
if (ImGui::ArrowButton("##leftonescreen", ImGuiDir_Left)) {
    if (g_dmod.map.loc[scr -1] > 0 && (scr - 1) % 32 != 0) {
    editor_save_screen(g_dmod.map.loc[scr]);
    editor_load_screen(g_dmod.map.loc[scr-1]);
    lsm_kill_all_nonlive_sprites();
    draw_screen_editor();
    scr--;
    spr[1].x -= 20;
    }
}
ImGui::SameLine(50);
if (ImGui::ArrowButton("##rightonescreen", ImGuiDir_Right)) {
    if (g_dmod.map.loc[scr + 1] > 0 && ((scr) % 32) != 0) {
        editor_save_screen(g_dmod.map.loc[scr]);
       editor_load_screen(g_dmod.map.loc[scr+1]); 
       lsm_kill_all_nonlive_sprites();
       draw_screen_editor();
       scr++;
    }
}
ImGui::Indent();
if (ImGui::ArrowButton("##downonescreen", ImGuiDir_Down)) {
    if (g_dmod.map.loc[scr + 32] > 0 && ((scr + 32) <= 768)) {
        editor_save_screen(g_dmod.map.loc[scr]);
       editor_load_screen(g_dmod.map.loc[scr+32]); 
       lsm_kill_all_nonlive_sprites();
       draw_screen_editor();
       scr += 32;
    }
}
ImGui::Unindent();
ImGui::End();
}

//Screen timer autosave thing, plus my debug shit
if (screentime) {
ImGui::Begin("Screen time");
ImGui::Text("Current time for screen is %d", SDL_GetTicks() - screenTime);
if (ImGui::SliderInt("colour", &tile_hide_col, 0, 255))
draw_screen_editor();

if (ImGui::Button("Revert screen to initial state upon game load")) {
    lsm_kill_all_nonlive_sprites();
    memcpy(&cur_ed_screen, &ramscreens[g_dmod.map.loc[scr]], sizeof(cur_ed_screen)); 
    draw_screen_editor();
}
int a, b;
int c, d;
SDL_GetRendererOutputSize(SDL_GetRenderer(g_display->window), &a, &b);
SDL_RenderGetLogicalSize(SDL_GetRenderer(g_display->window), &c, &d);
ImGui::Text("output: x %d, y, %d. 'logical': x %d, y %d. Display w: %d, h: %d", a, b, c,d, g_display->w, g_display->h);
ImGui::Text("Backbuffer w:%d, h: %d. Background w:%d, h: %d", IOGFX_backbuffer->w, IOGFX_backbuffer->h, IOGFX_background->w, IOGFX_background->h);
if (ImGui::Button("Change background/buffer size")) {
    IOGFX_background->w = a;
    IOGFX_background->h = b;
    IOGFX_backbuffer->w = a;
    IOGFX_backbuffer->h = b;
}
ImGui::End();
}

// Tile screen helper. Only shows up in tile selection mode.
if (editor_mode == 2)
{
    ImGui::Begin("Tile selector");
    if (ImGui::SliderInt("Screen", &ts, 1, 41))
    loadtile(ts);
    ImGui::Text("Tile: %d", xy2screentile(spr[1].x, spr[1].y) + 128 * (ts - 1));
    ImGui::Text("Press backspace to return to your last selected tile screen.");
    ImGui::End();
}

// Big fat warning. Could probably be fatter.
if (minimap_status > 1) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Minimap rendering in progress", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Please wait while the minimap renders. %d/768", minimap_status);
    //ImGui::Text("The minimap viewer is available in the 'Windows' menu");
    //ImGui::Text("For faster rendering, temporarily switch off 'draw hard on screen load' in the options menu before pressing Space");
    ImGui::End();
    //Make the box show up
    if (!edit_minimapbigrend && minimap_status > 0)
    mapprev = 1;
    
}


//Stolen from freedinkedit, toggled in display options, requested by drone1400
if (drawhard && (editor_mode == 4 || editor_mode == 5)) {
    int x1, y1;
    for (x1 = 0; x1 < 600; x1++)
        for (y1 = 0; y1 < 400; y1++) {
            if (screen_hitmap[x1][y1] == 1) {
                {
                    SDL_Rect GFX_box_crap;
                    GFX_box_crap.x = x1 + playl;
                    GFX_box_crap.y = y1;
                    GFX_box_crap.w = 1;
                    GFX_box_crap.h = 1;
                    IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[1].r,
                                            GFX_ref_pal[1].g,
                                            GFX_ref_pal[1].b);
                }
            }

            if (screen_hitmap[x1][y1] == 2) {
                {
                    SDL_Rect GFX_box_crap;
                    GFX_box_crap.x = x1 + playl;
                    GFX_box_crap.y = y1;
                    GFX_box_crap.w = 1;
                    GFX_box_crap.h = 1;
                    IOGFX_backbuffer->fillRect(
                            &GFX_box_crap, GFX_ref_pal[128].r,
                            GFX_ref_pal[128].g, GFX_ref_pal[128].b);
                }
            }

            if (screen_hitmap[x1][y1] == 3) {
                {
                    SDL_Rect GFX_box_crap;
                    GFX_box_crap.x = x1 + playl;
                    GFX_box_crap.y = y1;
                    GFX_box_crap.w = 1;
                    GFX_box_crap.h = 1;
                    IOGFX_backbuffer->fillRect(&GFX_box_crap, GFX_ref_pal[45].r,
                                            GFX_ref_pal[45].g,
                                            GFX_ref_pal[45].b);
                }
            }

            if (screen_hitmap[x1][y1] > 100) {

                if (cur_ed_screen.sprite[(screen_hitmap[x1][y1]) - 100]
                            .is_warp == 1) {
                    {
                        SDL_Rect GFX_box_crap;
                        GFX_box_crap.x = x1 + playl;
                        GFX_box_crap.y = y1;
                        GFX_box_crap.w = 1;
                        GFX_box_crap.h = 1;
                        IOGFX_backbuffer->fillRect(
                                &GFX_box_crap, GFX_ref_pal[20].r,
                                GFX_ref_pal[20].g, GFX_ref_pal[20].b);
                    }
                } else {
                    {
                        SDL_Rect GFX_box_crap;
                        GFX_box_crap.x = x1 + playl;
                        GFX_box_crap.y = y1;
                        GFX_box_crap.w = 1;
                        GFX_box_crap.h = 1;
                        IOGFX_backbuffer->fillRect(
                                &GFX_box_crap, GFX_ref_pal[23].r,
                                GFX_ref_pal[23].g, GFX_ref_pal[23].b);
                    }
                }
            }
        }

}
// stolen again, for the boundary preview. Colour gets clamped to the dink palette
                if (edit_spboundaries && editor_mode == 4 && cur_ed_screen.sprite[spri].active == 1) {
                    //lets draw a frame around the sprite we want
                    int dumbpic = 0;
                    int jh;
                    int realpic = 0;
                    rect box_crap, box_real;
                    for (jh = 1; dumbpic != spri; jh++) {
                        if (cur_ed_screen.sprite[jh].active)
                            if (cur_ed_screen.sprite[jh].vision == 0) {
                                dumbpic++;
                                realpic = spri;
                            }
                        //if (jh == 99)
                         //   goto fail;
                    }

                    //last_sprite_added = realpic;
                    int sprite =
                            add_sprite_dumb(cur_ed_screen.sprite[realpic].x,
                                            cur_ed_screen.sprite[realpic].y, 0,
                                            cur_ed_screen.sprite[realpic].seq,
                                            cur_ed_screen.sprite[realpic].frame,
                                            cur_ed_screen.sprite[realpic].size);
                    rect_copy(&spr[sprite].alt,
                            &cur_ed_screen.sprite[realpic].alt);
                    get_box(sprite, &box_crap, &box_real, 0);

                    get_box(sprite, &box_crap, &box_real, 0);
                    box_crap.bottom = box_crap.top + 5;
                    // GFX
                    {
                        SDL_Rect dst;
                        dst.x = box_crap.left;
                        dst.y = box_crap.top;
                        dst.w = box_crap.right - box_crap.left;
                        dst.h = 5;
                        IOGFX_backbuffer->fillRect(&dst, (int)boundcol[0] * 255, (int)boundcol[1] * 255, (int)boundcol[2] * 255);
                    }

                    get_box(sprite, &box_crap, &box_real, 0);
                    box_crap.right = box_crap.left + 5;
                    // GFX
                    {
                        SDL_Rect dst;
                        dst.x = box_crap.left;
                        dst.y = box_crap.top;
                        dst.w = 5;
                        dst.h = box_crap.bottom - box_crap.top;
                        IOGFX_backbuffer->fillRect(&dst, (int)boundcol[0] * 255, (int)boundcol[1] * 255, (int)boundcol[2] * 255);
                    }

                    get_box(sprite, &box_crap, &box_real,0);
                    box_crap.left = box_crap.right - 5;
                    // GFX
                    {
                        SDL_Rect dst;
                        dst.x = box_crap.right - 5;
                        dst.y = box_crap.top;
                        dst.w = 5;
                        dst.h = box_crap.bottom - box_crap.top;
                        IOGFX_backbuffer->fillRect(&dst, (int)boundcol[0] * 255, (int)boundcol[1] * 255, (int)boundcol[2] * 255);
                    }

                    get_box(sprite, &box_crap, &box_real,0);
                    box_crap.top = box_crap.bottom - 5;
                    // GFX
                    {
                        SDL_Rect dst;
                        dst.x = box_crap.left;
                        dst.y = box_crap.bottom - 5;
                        dst.w = box_crap.right - box_crap.left;
                        dst.h = 5;
                        IOGFX_backbuffer->fillRect(&dst, (int)boundcol[0] * 255, (int)boundcol[1] * 255, (int)boundcol[2] * 255);
                    }

                    lsm_remove_sprite(sprite);
                }


}

