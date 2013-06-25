/* Arcan-fe, scriptable front-end engine
 *
 * Arcan-fe is the legal property of its developers, please refer
 * to the COPYRIGHT file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fmt_open(int flags, mode_t mode, const char* fmt, ...)
{
	int rv = -1;

	unsigned cc;
	va_list args;
	va_start( args, fmt );
		cc = vsnprintf(NULL, 0,  fmt, args );
	va_end( args);

	char* dbuf;
	if (cc > 0 && (dbuf = (char*) malloc(cc + 1)) ) {
		va_start(args, fmt);
			vsprintf(dbuf, fmt, args);
		va_end(args);

		rv = open(dbuf, flags, mode);
		free(dbuf);
	}

	return rv;
}