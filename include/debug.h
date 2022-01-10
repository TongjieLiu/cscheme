/* debug.h -- cscheme debug mode

   Copyright (C) 2022 Tongjie Liu <tongjieandliu@gmail.com>.

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

#ifndef __CSCM_DEBUG_H__

#define __CSCM_DEBUG_H__




#include "ef.h"
#include "object.h"




#define CSCM_DEBUG_MAX_CMD_COUNT		8
#define CSCM_DEBUG_MAX_OPTION_LEN		16




#define CSCM_DEBUG_SHELL_PROMPT			"DEBUG-SHELL>> "




#define CSCM_DEBUG_CMD_RET_EXIT			0
#define CSCM_DEBUG_CMD_RET_CONTINUE		1




typedef int (*CSCM_DEBUG_CMD_HANDLER)(CSCM_OBJECT *env);


struct _CSCM_DEBUG_CMD {
	int flag_last;

	char *name;
	CSCM_DEBUG_CMD_HANDLER handler;
};


typedef struct _CSCM_DEBUG_CMD CSCM_DEBUG_CMD;




extern int cscm_debug_mode;




void cscm_debug_shell_start(CSCM_EF *ef, CSCM_OBJECT *env);




#define CSCM_ERROR_DEBUG_CMD_TOO_LONG		"command is too long"
#define CSCM_ERROR_DEBUG_OPTION_TOO_LONG	"command option is too long"




#define CSCM_ERROR_DEBUG_UNKNOWN_CMD		"unknown command"
#define CSCM_ERROR_DEBUG_OPTION_N		"incorrect number of command options"




#define CSCM_ERROR_DEBUG_CMD_NEXT_NOT_TIMES	"bad times"
#define CSCM_ERROR_DEBUG_CMD_NEXT_NEG_TIMES	"negative times"


#define CSCM_ERROR_DEBUG_CMD_FRAME_INDEX	"bad index"




#endif
