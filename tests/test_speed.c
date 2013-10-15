/* Copyright (c) 2013, Vsevolod Stakhov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *       * Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 *       * Redistributions in binary form must reproduce the above copyright
 *         notice, this list of conditions and the following disclaimer in the
 *         documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "ucl.h"

int
main (int argc, char **argv)
{
	void *map;
	struct ucl_parser *parser;
	ucl_object_t *obj;
	int fin;
	UT_string *err = NULL;
	unsigned char *emitted;
	struct stat st;
	const char *fname_in = NULL;
	int ret = 0;
	struct timespec start, end;
	double seconds;

	switch (argc) {
	case 2:
		fname_in = argv[1];
		break;
	}

	fin = open (fname_in, O_RDONLY);
	if (fin == -1) {
		perror ("open failed");
		exit (EXIT_FAILURE);
	}
	parser = ucl_parser_new (0);

	(void)fstat (fin, &st);
	map = mmap (NULL, st.st_size, PROT_READ, MAP_SHARED, fin, 0);
	if (map == MAP_FAILED) {
		perror ("mmap failed");
		exit (EXIT_FAILURE);
	}

	close (fin);

	clock_gettime (CLOCK_MONOTONIC, &start);
	ucl_parser_add_chunk (parser, map, st.st_size, &err);

	obj = ucl_parser_get_object (parser, &err);
	clock_gettime (CLOCK_MONOTONIC, &end);

	seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.;
	printf ("ucl: parsed input in %.4f seconds\n", seconds);

	clock_gettime (CLOCK_MONOTONIC, &start);
	emitted = ucl_object_emit (obj, UCL_EMIT_CONFIG);
	clock_gettime (CLOCK_MONOTONIC, &end);

	seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.;
	printf ("ucl: emitted config in %.4f seconds\n", seconds);

	free (emitted);

	clock_gettime (CLOCK_MONOTONIC, &start);
	emitted = ucl_object_emit (obj, UCL_EMIT_JSON);
	clock_gettime (CLOCK_MONOTONIC, &end);

	seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.;
	printf ("ucl: emitted json in %.4f seconds\n", seconds);

	free (emitted);

	clock_gettime (CLOCK_MONOTONIC, &start);
	emitted = ucl_object_emit (obj, UCL_EMIT_JSON_COMPACT);
	clock_gettime (CLOCK_MONOTONIC, &end);

	seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.;
	printf ("ucl: emitted compact json in %.4f seconds\n", seconds);

	free (emitted);

	clock_gettime (CLOCK_MONOTONIC, &start);
	emitted = ucl_object_emit (obj, UCL_EMIT_YAML);
	clock_gettime (CLOCK_MONOTONIC, &end);

	seconds = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.;
	printf ("ucl: emitted yaml in %.4f seconds\n", seconds);

	free (emitted);

	ucl_parser_free (parser);
	ucl_obj_unref (obj);

	munmap (map, st.st_size);

	return ret;
}