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
#include <map>
#include <string>
#include <set>

#include <pkg.h>

using namespace std;

#define PLUGIN_NAME "compare"


static char cmd_name[] = "compare";
static char cmd_desc[] = "Compare local and remote package";
static char name[] = "compare";
static char version[] = "1.0.0";
static char description[] = "Plugin for comparing local and remote pkgs";

struct pkg_plugin *self;

struct pkg_info {
    string version;
    map<string, string> options;
    set<string> shlibs;
    map<string, string> deps;
};


static int
get_info(const char *name, struct pkg_info *info, bool remote) {
    struct pkgdb *db = NULL;
    if (pkgdb_open_all(&db, PKGDB_REMOTE, NULL) != EPKG_OK) {
        return (EX_IOERR);
    }

    struct pkgdb_it *pkg_it;
    if (remote) {
        pkg_it = pkgdb_repo_query(db, name, MATCH_EXACT, NULL);
    } else {
        pkg_it = pkgdb_query(db, name, MATCH_EXACT);
    }

    bool found = false;
    struct pkg *pkg = NULL;
    while (pkgdb_it_next(pkg_it, &pkg,
                PKG_LOAD_BASIC|
                PKG_LOAD_DEPS|
                PKG_LOAD_SHLIBS_REQUIRED|
                PKG_LOAD_OPTIONS) == EPKG_OK) {
        found = true;

        char *ver;
        pkg_asprintf(&ver, "%v", pkg);
        info->version = ver;
        free(ver);

        struct pkg_option *option = NULL;
        while (pkg_options(pkg, &option) == EPKG_OK) {
            char *opt_name;
            pkg_asprintf(&opt_name, "%On", option);
            char *opt_value;
            pkg_asprintf(&opt_value, "%Ov", option);

            info->options.insert(pair<string, string>(opt_name, opt_value));
            free(opt_name);
            free(opt_value);
        }

        char *shlib = NULL;
        while (pkg_shlibs_required(pkg, &shlib) == EPKG_OK) {
            info->shlibs.insert(shlib);
        }

        struct pkg_dep *dep = NULL;
        while (pkg_deps(pkg, &dep) == EPKG_OK) {
            info->deps.insert(pair<string, string>(pkg_dep_name(dep),
                                                   pkg_dep_version(dep)));
        }

        // TODO: handle multiple matches
        break;
    }
    pkgdb_it_free(pkg_it);
    pkgdb_close(db);

    if (!found)
        return (EX_DATAERR);

    return 0;
}

template<class T>
static void
compare_maps(const T& local, const T& remote, const char *appear,
        const char *diff_str) {
    for (const auto& it : local) {
        if (remote.count(it.first) == 0) {
            printf("\tOnly %s in local: %s = %s\n",
                    appear, it.first.c_str(), it.second.c_str());
        }
    }
    for (const auto& it : remote) {
        if (local.count(it.first) == 0) {
            printf("\tOnly %s in remote: %s = %s\n",
                    appear, it.first.c_str(), it.second.c_str());
        }
    }
    if (diff_str) {
        for (const auto& it : local) {
            const auto &rit = remote.find(it.first);
            if (rit != remote.end() && rit->second != it.second) {
                printf("\tDifferent %s for %s: local %s remote %s\n",
                        diff_str, it.first.c_str(), it.second.c_str(),
                        rit->second.c_str());
            }
        }
    }
}

static void
compare_options(const struct pkg_info &local, const struct pkg_info &remote) {
    compare_maps(local.options, remote.options, "available", "value");
}

static void
compare_deps(const struct pkg_info &local, const struct pkg_info &remote) {
    compare_maps(local.deps, remote.deps, "depend", NULL);
}

static void
compare_version(const struct pkg_info &local, const struct pkg_info &remote) {
    if (local.version != remote.version) {
        printf("\tVersion difference: local %s remote %s\n",
                local.version.c_str(), remote.version.c_str());
    }
}

static void
compare_shlibs(const struct pkg_info &local, const struct pkg_info &remote) {
    for (const auto& lib : local.shlibs) {
        if (remote.shlibs.count(lib) == 0) {
            printf("\tOnly required by local: %s\n", lib.c_str());
        }
    }
    for (const auto& lib : remote.shlibs) {
        if (local.shlibs.count(lib) == 0) {
            printf("\tOnly required by remote: %s\n", lib.c_str());
        }
    }
}

static int
compare_pkg(const char *name) {
    struct pkg_info local, remote;
    int err;

    if ((err = get_info(name, &local, false)) != 0 ||
            (err = get_info(name, &remote, true)) != 0) {
        fprintf(stderr, "Fail to get information for %s\n", name);
        return err;
    }

    fprintf(stderr, "Comparing %s:\n", name);
    compare_version(local, remote);
    compare_options(local, remote);
    compare_shlibs(local, remote);
    compare_deps(local, remote);

    return 0;
}

static int
plugin_compare_callback(int argc, char **argv)
{
    for (int i=1; i<argc; i++) {
        compare_pkg(argv[i]);
    }
    return (EPKG_OK);
}

extern "C" {

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

} // extern "C"
