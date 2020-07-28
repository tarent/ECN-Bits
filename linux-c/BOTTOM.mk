# -*- mode: make -*-
#-
# Copyright © 2020
#	mirabilos <t.glaser@tarent.de>
# Licensor: Deutsche Telekom
#
# Provided that these terms and disclaimer and all copyright notices
# are retained or reproduced in an accompanying document, permission
# is granted to deal in this work without restriction, including un‐
# limited rights to use, publicly perform, distribute, sell, modify,
# merge, give away, or sublicence.
#
# This work is provided “AS IS” and WITHOUT WARRANTY of any kind, to
# the utmost extent permitted by applicable law, neither express nor
# implied; without malicious intent or gross negligence. In no event
# may a licensor, author or contributor be held liable for indirect,
# direct, other damage, loss, or other issues arising in any way out
# of dealing in the work, even if advised of the possibility of such
# damage or existence of a defect, except proven that it results out
# of said person’s immediate fault when using the work as intended.

# include ${TOP}/BOTTOM.mk

ifdef PROG
SRCS?=		${PROG}.c
endif

OBJS?=		${SRCS:.c=.o}

ifdef LIB
CLEANFILES+=	lib${LIB}.a
all: lib${LIB}.a
lib${LIB}.a: ${OBJS}
	${AR} rc $@ ${OBJS}
	${RANLIB} $@
endif

ifdef PROG
LIBS+=		-lecn-bits
CLEANFILES+=	${PROG}
all: ${PROG}
${PROG}: ${OBJS} ${TOP}/lib/libecn-bits.a
	${LINK.c} -o $@ ${OBJS} ${LIBS}
endif

clean:
	-rm -f ${CLEANFILES}