.include <bsd.own.mk>

PREFIX?=	/usr/local
LIBDIR=		${PREFIX}/lib/pkg/
SHLIB_DIR?=	${LIBDIR}/
SHLIB_NAME?=	${PLUGIN_NAME}.so
LIB_CXX=  ${PLUGIN_NAME}
MK_PROFILE=	no
MK_INSTALLLIB=	no

PLUGIN_NAME=	compare
SRCS=		compare.cpp

PKGFLAGS!=	pkgconf --cflags pkg
CFLAGS+=	${PKGFLAGS}

CFLAGS+=	-fno-exceptions -lc++

beforeinstall:
	${INSTALL} -d ${LIBDIR}

.include <bsd.lib.mk>
