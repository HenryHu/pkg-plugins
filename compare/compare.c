/*
 * Copyright (c) 2015 Henry Hu <henry.hu.sh@gmail.com>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <libutil.h>

#include <pkg.h>

#define PLUGIN_NAME "compare"

static char cmd_name[] = "compare";
static char cmd_desc[] = "Compare local and remote package";
static char name[] = "compare";
static char version[] = "1.0.0";
static char description[] = "Plugin for comparing local and remote pkgs";

struct pkg_plugin *self;

int
pkg_plugin_init(struct pkg_plugin *p)
{
	self = p;
	pkg_plugin_set(p, PKG_PLUGIN_NAME, name);
	pkg_plugin_set(p, PKG_PLUGIN_DESC, description);
	pkg_plugin_set(p, PKG_PLUGIN_VERSION, version);

	return (EPKG_OK);
}

int
pkg_plugin_shutdown(struct pkg_plugin *p __unused)
{
	/* nothing to be done here */

	return (EPKG_OK);
}

static int
plugin_compare_callback(int argc, char **argv)
{
    return (EPKG_OK);
}

int
pkg_register_cmd_count() {
    return 1;
}

int
pkg_register_cmd(int id, const char **name, const char **desc,
        int (**exec)(int argc, char **argv))
{
	*name = cmd_name;
	*desc = cmd_desc;
	*exec = plugin_compare_callback;

	return (EPKG_OK);
}

