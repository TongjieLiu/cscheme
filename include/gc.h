/* gc.h -- garbage collection

   Copyright (C) 2021 Tongjie Liu <tongjieandliu@gmail.com>.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#ifndef __CSCM_GC_H__

#define __CSCM_GC_H__




#include "cscheme.h"




#ifdef __CSCM_CSCHEME_DEBUG__
	#define __CSCM_GC_DEBUG__
#endif




void cscm_gc_inc_total_object_count();
void cscm_gc_dec_total_object_count();




void cscm_gc_inc(CSCM_OBJECT *obj);
void cscm_gc_dec(CSCM_OBJECT *obj);


void cscm_gc_free(CSCM_OBJECT *obj);




void cscm_gc_show_total_object_count(char *stage);




#endif
