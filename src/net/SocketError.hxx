/*
 * Copyright 2015-2021 Max Kellermann <max.kellermann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SOCKET_ERROR_HXX
#define SOCKET_ERROR_HXX

#include "util/Compiler.h"
#include "system/Error.hxx"

#ifdef _WIN32
#include <winsock2.h>
typedef DWORD socket_error_t;
#else
#include <cerrno>
typedef int socket_error_t;
#endif

gcc_pure
static inline socket_error_t
GetSocketError() noexcept
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

gcc_const
static inline bool
IsSocketErrorAgain(socket_error_t code) noexcept
{
#ifdef _WIN32
	return code == WSAEINPROGRESS;
#else
	return code == EAGAIN;
#endif
}

gcc_const
static inline bool
IsSocketErrorInterruped(socket_error_t code) noexcept
{
#ifdef _WIN32
	return code == WSAEINTR;
#else
	return code == EINTR;
#endif
}

gcc_const
static inline bool
IsSocketErrorClosed(socket_error_t code) noexcept
{
#ifdef _WIN32
	return code == WSAECONNRESET;
#else
	return code == EPIPE || code == ECONNRESET;
#endif
}

/**
 * Helper class that formats a socket error message into a
 * human-readable string.  On Windows, a buffer is necessary for this,
 * and this class hosts the buffer.
 */
class SocketErrorMessage {
#ifdef _WIN32
	static constexpr unsigned msg_size = 256;
	char msg[msg_size];
#else
	const char *const msg;
#endif

public:
	explicit SocketErrorMessage(socket_error_t code=GetSocketError()) noexcept;

	operator const char *() const {
		return msg;
	}
};

gcc_const
static inline std::system_error
MakeSocketError(socket_error_t code, const char *msg) noexcept
{
#ifdef _WIN32
	return MakeLastError(code, msg);
#else
	return MakeErrno(code, msg);
#endif
}

gcc_pure
static inline std::system_error
MakeSocketError(const char *msg) noexcept
{
	return MakeSocketError(GetSocketError(), msg);
}

#endif