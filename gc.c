/* gc.c -- garbage collection

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

#include <stddef.h>
#include <stdio.h>

#include "error.h"
#include "object.h"
#include "bool.h"
#include "pair.h"
#include "env.h"
#include "gc.h"




/*	The 4 objects CSCM_NIL, CSCM_TRUE, CSCM_FALSE, CSCM_UNASSIGNED
 * are stored in heap and not allocated by cscm_object_create(), there-
 * fore are not counted by cscm_gc_inc_total_object_count(). */
size_t _cscm_gc_total_object_count = 4;


void cscm_gc_inc_total_object_count()
{
	_cscm_gc_total_object_count++;

	if ((_cscm_gc_total_object_count % 100) == 0)
		printf("*** GC DEBUG INFO *** total object count: %lu\n", \
			(unsigned long)_cscm_gc_total_object_count);
}


void cscm_gc_dec_total_object_count()
{
	_cscm_gc_total_object_count--;

	if ((_cscm_gc_total_object_count % 100) == 0)
		printf("*** GC DEBUG INFO *** total object count: %lu\n", \
			(unsigned long)_cscm_gc_total_object_count);
}




void cscm_gc_inc(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_gc_inc", \
				CSCM_ERROR_NULL_PTR);


	obj->ref_count++;
}


void cscm_gc_dec(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_gc_dec", \
				CSCM_ERROR_NULL_PTR);


	obj->ref_count--;
}




void cscm_gc_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_gc_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj == CSCM_NIL	\
		|| obj == CSCM_TRUE	\
		|| obj == CSCM_FALSE	\
		|| obj == CSCM_UNASSIGNED)
		return; // these objects will not affect the total object count
	else if (obj->ref_count == 0) {
		cscm_object_free(obj);

		#ifdef __CSCM_GC_DEBUG__
			cscm_gc_dec_total_object_count();
		#endif
	}
}




void cscm_gc_show_total_object_count(char *stage)
{
	if (stage == NULL)
		cscm_error_report("cscm_gc_show_total_object_count", \
				CSCM_ERROR_NULL_PTR);


	puts("------------------------------------------------------");

	printf("Entering <stage %s>, total object count = %lu\n\n",	\
		stage,							\
		(unsigned long)_cscm_gc_total_object_count);
}
