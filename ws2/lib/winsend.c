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
#pragma warning(push,1)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma warning(pop)

#include "ecn-bitw.h"

ECNBITS_EXPORTAPI SSIZE_T
ecnws2_sendmsg(SOCKET fd, LPWSAMSG msg, int flags)
{
	DWORD numbytes = 0;

	if (WSASendMsg(fd, msg, flags, &numbytes, NULL, NULL) == 0)
		return ((SSIZE_T)numbytes);
	/* caller still must use WSAGetLastError() but signalling as in SUS */
	return ((SSIZE_T)-1);
}
