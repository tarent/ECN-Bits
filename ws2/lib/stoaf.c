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
#endif
#include <errno.h>

#include "ecn-bitw.h"

#if (AF_INET != -1) && (AF_INET6 != -1)
ECNBITS_EXPORTAPI int
ecnbits_stoaf(SOCKET socketfd)
{
	struct sockaddr_storage ss;
	socklen_t slen = sizeof(ss);

	if (getsockname(socketfd, (void *)&ss, &slen) == 0)
		switch (ss.ss_family) {
		case AF_INET:
		case AF_INET6:
			return (ss.ss_family);
		default:
#if defined(_WIN32) || defined(WIN32)
			WSASetLastError(WSAEAFNOSUPPORT);
#endif
			errno = WSAEAFNOSUPPORT;
		}
	return (-1);
}
#else
# error AF_INET or AF_INET6 conflict with the error return value
#endif
