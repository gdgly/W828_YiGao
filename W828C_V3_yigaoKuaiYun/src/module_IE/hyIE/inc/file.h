/** 
 * Function prototypes for handling the file protocol.
 *
 * @author Tomas Berndtsson <tomas@nocrew.org>
 */

#ifndef _PROTOCOL_FILE_H_
#define _PROTOCOL_FILE_H_

/*
 * Copyright (C) 1999, Tomas Berndtsson <tomas@nocrew.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

//#include "win.h"
#include "lprotocol.h"
#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes. */
extern int protocol_file_open(char *filename, 
			      struct protocol_http_headers *headers);
extern int protocol_file_close(int fd);
#ifdef __cplusplus
}
#endif

#endif /* _PROTOCOL_FILE_H_ */
