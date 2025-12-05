/* SPDX-License-Identifier: GPL-2.0 OR MIT
 *
 * Copyright (C) 2018-2021 WireGuard LLC. All Rights Reserved.
 */

#ifndef WINTUN_H
#define WINTUN_H

#include <windows.h>
#include <ifdef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MinGW is missing this one, unfortunately. */
#ifndef _Must_inspect_result_
#define _Must_inspect_result_
#endif

/**
 * A handle representing Wintun adapter
 */
typedef struct _WINTUN_ADAPTER *WINTUN_ADAPTER_HANDLE;

/**
 * Creates a new Wintun adapter.
 */
typedef _Must_inspect_result_
_Return_type_success_(return != NULL)
_Post_maybenull_
WINTUN_ADAPTER_HANDLE(WINAPI WINTUN_CREATE_ADAPTER_FUNC)(_In_z_ LPCWSTR Name, _In_z_ LPCWSTR TunnelType, _In_opt_ const GUID *RequestedGUID);

/**
 * Opens an existing Wintun adapter.
 */
typedef _Must_inspect_result_
_Return_type_success_(return != NULL)
_Post_maybenull_
WINTUN_ADAPTER_HANDLE(WINAPI WINTUN_OPEN_ADAPTER_FUNC)(_In_z_ LPCWSTR Name);

/**
 * Releases Wintun adapter resources.
 */
typedef VOID(WINAPI WINTUN_CLOSE_ADAPTER_FUNC)(_In_opt_ WINTUN_ADAPTER_HANDLE Adapter);

/**
 * Deletes the Wintun driver if there are no more adapters in use.
 */
typedef _Return_type_success_(return != FALSE)
BOOL(WINAPI WINTUN_DELETE_DRIVER_FUNC)(VOID);

/**
 * Returns the LUID of the adapter.
 */
typedef VOID(WINAPI WINTUN_GET_ADAPTER_LUID_FUNC)(_In_ WINTUN_ADAPTER_HANDLE Adapter, _Out_ NET_LUID *Luid);

/**
 * Determines the version of the Wintun driver currently loaded.
 */
typedef _Return_type_success_(return != 0)
DWORD(WINAPI WINTUN_GET_RUNNING_DRIVER_VERSION_FUNC)(VOID);

/**
 * Determines the level of logging.
 */
typedef enum
{
    WINTUN_LOG_INFO, /**< Informational */
    WINTUN_LOG_WARN, /**< Warning */
    WINTUN_LOG_ERR   /**< Error */
} WINTUN_LOGGER_LEVEL;

/**
 * Called by internal logger to report diagnostic messages
 */
typedef VOID(CALLBACK *WINTUN_LOGGER_CALLBACK)(
    _In_ WINTUN_LOGGER_LEVEL Level,
    _In_ DWORD64 Timestamp,
    _In_z_ LPCWSTR Message);

/**
 * Sets logger callback function.
 */
typedef VOID(WINAPI WINTUN_SET_LOGGER_FUNC)(_In_ WINTUN_LOGGER_CALLBACK NewLogger);

#define WINTUN_MIN_RING_CAPACITY 0x20000 /* 128kiB */
#define WINTUN_MAX_RING_CAPACITY 0x4000000 /* 64MiB */

/**
 * A handle representing Wintun session
 */
typedef struct _TUN_SESSION *WINTUN_SESSION_HANDLE;

/**
 * Starts Wintun session.
 */
typedef _Must_inspect_result_
_Return_type_success_(return != NULL)
_Post_maybenull_
WINTUN_SESSION_HANDLE(WINAPI WINTUN_START_SESSION_FUNC)(_In_ WINTUN_ADAPTER_HANDLE Adapter, _In_ DWORD Capacity);

/**
 * Ends Wintun session.
 */
typedef VOID(WINAPI WINTUN_END_SESSION_FUNC)(_In_ WINTUN_SESSION_HANDLE Session);

/**
 * Gets Wintun session's read-wait event handle.
 */
typedef HANDLE(WINAPI WINTUN_GET_READ_WAIT_EVENT_FUNC)(_In_ WINTUN_SESSION_HANDLE Session);

#define WINTUN_MAX_IP_PACKET_SIZE 0xFFFF

/**
 * Retrieves one or packet.
 */
typedef _Must_inspect_result_
_Return_type_success_(return != NULL)
_Post_maybenull_
_Post_writable_byte_size_(*PacketSize)
BYTE *(WINAPI WINTUN_RECEIVE_PACKET_FUNC)(_In_ WINTUN_SESSION_HANDLE Session, _Out_ DWORD *PacketSize);

/**
 * Releases internal buffer.
 */
typedef VOID(
    WINAPI WINTUN_RELEASE_RECEIVE_PACKET_FUNC)(_In_ WINTUN_SESSION_HANDLE Session, _In_ const BYTE *Packet);

/**
 * Allocates memory for a packet to send.
 */
typedef _Must_inspect_result_
_Return_type_success_(return != NULL)
_Post_maybenull_
_Post_writable_byte_size_(PacketSize)
BYTE *(WINAPI WINTUN_ALLOCATE_SEND_PACKET_FUNC)(_In_ WINTUN_SESSION_HANDLE Session, _In_ DWORD PacketSize);

/**
 * Sends the packet and releases internal buffer.
 */
typedef VOID(WINAPI WINTUN_SEND_PACKET_FUNC)(_In_ WINTUN_SESSION_HANDLE Session, _In_ const BYTE *Packet);

#ifdef __cplusplus
}
#endif

#endif
