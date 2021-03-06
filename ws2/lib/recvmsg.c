/*-
 * Copyright © 2020
 *	mirabilos <t.glaser@tarent.de>
 * Licensor: Deutsche Telekom
 *
 * Provided that these terms and disclaimer and all copyright notices
 * are retained or reproduced in an accompanying document, permission
 * is granted to deal in this work without restriction, including un‐
 * limited rights to use, publicly perform, distribute, sell, modify,
 * merge, give away, or sublicence.
 *
 * This work is provided “AS IS” and WITHOUT WARRANTY of any kind, to
 * the utmost extent permitted by applicable law, neither express nor
 * implied; without malicious intent or gross negligence. In no event
 * may a licensor, author or contributor be held liable for indirect,
 * direct, other damage, loss, or other issues arising in any way out
 * of dealing in the work, even if advised of the possibility of such
 * damage or existence of a defect, except proven that it results out
 * of said person’s immediate fault when using the work as intended.
 */

#include <sys/types.h>
#if defined(_WIN32) || defined(WIN32)
#pragma warning(push,1)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma warning(pop)
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif
#include <errno.h>
#include <string.h>

#include "ecn-bitw.h"

#if defined(_WIN32) || defined(WIN32)
#define msg_control	Control.buf
#define msg_controllen	Control.len
#define recvmsg		ecnws2_recvmsg
#endif

ECNBITS_EXPORTAPI SSIZE_T
ecnbits_recvmsg(SOCKET s, LPWSAMSG mh, int flags, unsigned short *e)
{
	SSIZE_T rv;
#if defined(_WIN32) || defined(WIN32)
	WSABUF obuf;
#define oldclen obuf.len
#else
	size_t oldclen;
#endif
	char cmsgbuf[2 * ECNBITS_CMSGBUFLEN];

	if (!e)
		return (recvmsg(s, mh, flags));

	if (mh->msg_control)
		return (ecnbits_rdmsg(s, mh, flags, e));

	oldclen = mh->msg_controllen;
	mh->msg_control = cmsgbuf;
	mh->msg_controllen = sizeof(cmsgbuf);
	rv = ecnbits_rdmsg(s, mh, flags, e);
	mh->msg_control = NULL;
	mh->msg_controllen = oldclen;
	return (rv);
}
