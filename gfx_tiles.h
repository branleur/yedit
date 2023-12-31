/**
 * Draw background from tiles

 * Copyright (C) 2007, 2008  Sylvain Beucler
 * Copyright (C) 2022 Yeoldetoast
 * This file is part of GNU FreeDink

 * GNU FreeDink is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.

 * GNU FreeDink is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _GFX_TILES_H
#define _GFX_TILES_H

/* #include <ddraw.h> */
#include "SDL.h"
#include "IOGfxSurface.h"

#define GFX_TILES_NB_SETS 41

/* 96 = 12 * 8 tile squares; 1 tile square = 50x50 pixels */
#define GFX_TILES_SCREEN_W 12
#define GFX_TILES_SCREEN_H 8
#define GFX_TILES_PER_SCREEN (GFX_TILES_SCREEN_W * GFX_TILES_SCREEN_H)
#define GFX_TILES_SQUARE_SIZE 50
/* 96 tiles per tileset but indexed in slots of 128 in the code (aka memory holes)
so 128*(41-1)+12*8=5216 indexes used, instead of 12*8*41=3936 */
//Yeolde: got rid of the -1 so that it calculates them properly
#define GFX_TILES_NB_SQUARES                                                   \
	(128 * (GFX_TILES_NB_SETS) + GFX_TILES_SCREEN_W * GFX_TILES_SCREEN_H)

extern void process_animated_tiles(IOGfxSurface** gfx_tiles,
								struct editor_screen_tilerefs* tilerefs,
								Uint32 thisTickCount);
extern void gfx_tiles_draw_screen(IOGfxSurface** gfx_tiles,
								struct editor_screen_tilerefs* tilerefs);
extern void gfx_tiles_draw(int srctileset_idx0, int srctile_square_idx0,
						int dsttile_square_idx0);
extern int gfx_tiles_memusage(IOGfxSurface** gfx_tiles);

#endif
