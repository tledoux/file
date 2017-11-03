/*
 * Copyright (c) Christos Zoulas 2017.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice immediately at the beginning of the file, without modification,
 *    this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "file.h"

#ifndef	lint
FILE_RCSID("@(#)$File: buffer.c,v 1.2 2017/11/03 00:18:55 christos Exp $")
#endif	/* lint */

#include "magic.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

void
buffer_init(struct buffer *b, int fd, const void *data, size_t len)
{
	b->fd = fd;
	b->fbuf = data;
	b->flen = len;
	b->ebuf = NULL;
	b->elen = 0;
}

void
buffer_fini(struct buffer *b)
{
	free(b->ebuf);
}

int
buffer_fill(const struct buffer *bb)
{
	// XXX: should be cached?
	struct stat st;
	struct buffer *b = CCAST(struct buffer *, bb);

	if (b->elen != 0)
		return b->elen == (size_t)~0 ? -1 : 0;

	if (b->fd == -1 || fstat(b->fd, &st) == -1 || !S_ISREG(st.st_mode))
		goto out;

	b->elen =  (size_t)st.st_size < b->flen ? (size_t)st.st_size : b->flen;
	if ((b->ebuf = malloc(b->elen)) == NULL)
		goto out;

	if (pread(b->fd, b->ebuf, b->elen, st.st_size - b->elen) == -1) {
		free(b->ebuf);
		goto out;
	}

	return 0;
out:
	b->elen = (size_t)~0;
	return -1;
}