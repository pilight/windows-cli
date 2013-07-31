/*
	Copyright (C) 2013 CurlyMo

	This file is part of the Raspberry Pi 433.92Mhz transceiver.

    Raspberry Pi 433.92Mhz transceiver is free software: you can redistribute
	it and/or modify it under the terms of the GNU General Public License as
	published by the Free Software Foundation, either version 3 of the License,
	or (at your option) any later version.

    Raspberry Pi 433.92Mhz transceiver is distributed in the hope that it will
	be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Raspberry Pi 433.92Mhz transceiver. If not, see
	<http://www.gnu.org/licenses/>
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <winsock.h>
#include <winsock2.h>
#include <sys/types.h>
#include <sys/time.h>

#define SHUT_RD   SD_RECEIVE 
#define SHUT_WR   SD_SEND 
#define SHUT_RDWR SD_BOTH 

#define MSG_DONTWAIT 	0x40
#define MSG_NOSIGNAL 	0x2000

#include "config.h"
#include "log.h"
#include "socket.h"

/* http://stackoverflow.com/a/15370175 */

#define NS_INADDRSZ  4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ   2

int inet_pton4(const char *src, char *dst) {
    uint8_t tmp[NS_INADDRSZ], *tp;

    int saw_digit = 0;
    int octets = 0;
    *(tp = tmp) = 0;

    int ch;
    while((ch = *src++) != '\0') {
        if(ch >= '0' && ch <= '9') {
            uint32_t n = *tp * 10 + (ch - '0');

            if(saw_digit && *tp == 0)
                return 0;

            if(n > 255)
                return 0;

            *tp = n;
            if(!saw_digit) {
                if(++octets > 4)
                    return 0;
                saw_digit = 1;
            }
        } else if(ch == '.' && saw_digit) {
            if(octets == 4)
                return 0;
            *++tp = 0;
            saw_digit = 0;
        } else {
            return 0;
		}
    }
    if(octets < 4) {
        return 0;
	}
    memcpy(dst, tmp, NS_INADDRSZ);
    return 1;
}

int inet_pton6(const char *src, char *dst) {
    static const char xdigits[] = "0123456789abcdef";
    uint8_t tmp[NS_IN6ADDRSZ];

    uint8_t *tp = (uint8_t*) memset(tmp, '\0', NS_IN6ADDRSZ);
    uint8_t *endp = tp + NS_IN6ADDRSZ;
    uint8_t *colonp = NULL;
	int i = 0;

    /* Leading :: requires some special handling. */
    if(*src == ':') {
        if(*++src != ':')
            return 0;
    }

    const char *curtok = src;
    int saw_xdigit = 0;
    uint32_t val = 0;
    int ch;
    while((ch = tolower(*src++)) != '\0') {
        const char *pch = strchr(xdigits, ch);
        if(pch != NULL) {
            val <<= 4;
            val |= (pch - xdigits);
            if(val > 0xffff) {
                return 0;
			}
            saw_xdigit = 1;
            continue;
        }
        if(ch == ':') {
            curtok = src;
            if(!saw_xdigit) {
                if(colonp) {
                    return 0;
				}
                colonp = tp;
                continue;
            } else if (*src == '\0') {
                return 0;
            }
            if(tp + NS_INT16SZ > endp)
                return 0;
            *tp++ = (uint8_t) (val >> 8) & 0xff;
            *tp++ = (uint8_t) val & 0xff;
            saw_xdigit = 0;
            val = 0;
            continue;
        }
        if(ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
                inet_pton4(curtok, (char*) tp) > 0) {
            tp += NS_INADDRSZ;
            saw_xdigit = 0;
            break; /* '\0' was seen by inet_pton4(). */
        }
        return 0;
    }
    if(saw_xdigit) {
        if(tp + NS_INT16SZ > endp) {
            return 0;
		}
        *tp++ = (uint8_t) (val >> 8) & 0xff;
        *tp++ = (uint8_t) val & 0xff;
    }
    if(colonp != NULL) {
        /*
         * Since some memmove()'s erroneously fail to handle
         * overlapping regions, we'll do the shift by hand.
         */
        const int n = tp - colonp;

        if(tp == endp) {
            return 0;
		}

        for(i = 1; i <= n; i++)  {
            endp[-i] = colonp[n - i];
            colonp[n - i] = 0;
        }
        tp = endp;
    }
    if (tp != endp) {
        return 0;
	}
    memcpy(dst, tmp, NS_IN6ADDRSZ);
    return 1;
}

int inet_pton(int af, const char *src, char *dst) {
    switch (af) {
		case AF_INET:
			return inet_pton4(src, dst);
		case AF_INET6:
			return inet_pton6(src, dst);
		default:
			return -1;
    }
}


int connect_to_server(char *address, unsigned short port) {
	struct sockaddr_in serv_addr;
	int sockfd;
	int optval = 0;
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 1), &wsaData);
	
	/* Try to open a new socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, NULL)) < 0) {
        logprintf(LOG_ERR, "could not create socket");
		return -1;
    }

	/* Clear the server address */
    memset(&serv_addr, '\0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, (char *)&serv_addr.sin_addr);

	/* Connect to the server */
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		return -1;
    } else {
		getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(int));
		return sockfd;
	}
}


void socket_close(int sockfd) {
	int i = 0;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	if(sockfd > 0) {
		getpeername(sockfd, (struct sockaddr*)&address, &addrlen);
		shutdown(sockfd, SHUT_WR);
		closesocket(sockfd);
		WSACleanup();
		logprintf(LOG_INFO, "client disconnected, ip %s, port %d", inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	}
}

void socket_write(int sockfd, const char *msg, ...) {
	char message[BUFFER_SIZE];
	va_list ap;

	if(strlen(msg) > 0 && sockfd > 0) {
		memset(message, '\0', BUFFER_SIZE);

		va_start(ap, msg);
		vsprintf(message, msg, ap);
		va_end(ap);
		strcat(message, "\n");

		if(send(sockfd, message, strlen(message), 0) == -1) {
			logprintf(LOG_DEBUG, "socket write failed: %s", message);
			socket_close(sockfd);
		} else {
			if(strcmp(message, "BEAT\n") != 0) {
				logprintf(LOG_DEBUG, "socket write succeeded: %s", message);
			}
			// Prevent buffering of messages
			usleep(250);
		}
	}
}

void socket_write_big(int sockfd, const char *msg, ...) {
	char message[BIG_BUFFER_SIZE];
	va_list ap;

	if(strlen(msg) > 0 && sockfd > 0) {
		memset(message, '\0', BIG_BUFFER_SIZE);

		va_start(ap, msg);
		vsprintf(message, msg, ap);
		va_end(ap);
		strcat(message, "\n");

		if(send(sockfd, message, strlen(message), 0) == -1) {
			logprintf(LOG_DEBUG, "socket write failed: %s", message);
			socket_close(sockfd);
		} else {
			if(strcmp(message, "BEAT\n") != 0) {
				logprintf(LOG_DEBUG, "socket write succeeded: %s", message);
			}
			// Prevent buffering of messages
			usleep(250);
		}
	}
}

char *socket_read(int sockfd) {
	char *recvBuff = malloc((sizeof(char)*BUFFER_SIZE));
	memset(recvBuff, '\0', BUFFER_SIZE);

	if(recv(sockfd, recvBuff, BUFFER_SIZE, 0) < 1) {
		return NULL;
	} else {
		return recvBuff;
	}
}

char *socket_read_big(int sockfd) {
	char *recvBuff = malloc((sizeof(char)*BIG_BUFFER_SIZE));
	memset(recvBuff, '\0', BIG_BUFFER_SIZE);

	if(recv(sockfd, recvBuff, BIG_BUFFER_SIZE, 0) < 1) {
		return NULL;
	} else {
		return recvBuff;
	}
}

int socket_msgcmp(char *a, char *b) {
	char tmp[BUFFER_SIZE];
	strcpy(tmp, b);
	strcat(tmp, "\n");
	return strcmp(a, tmp);
}