#include "TunDevice.h"
#include <iostream>
#include <stdexcept>

namespace vpn::tun {

    TunDevice::TunDevice(const std::wstring& name, const std::wstring& tunnel_type)
        : name_(name), tunnel_type_(tunnel_type) {
        LoadWintun();
        
        // Create or Open Adapter
        // We try to open first, if fails, create.
        // Actually, WintunCreateAdapter will fail if it exists? No, it might create a new one with same name if GUID is null?
        // WireGuard usually creates one.
        // Let's try Create.
        GUID guid;
        CoCreateGuid(&guid); // Or use a fixed GUID if we want persistence.
        
        adapter_ = WintunCreateAdapter(name_.c_str(), tunnel_type_.c_str(), &guid);
        if (!adapter_) {
            // Try opening?
            adapter_ = WintunOpenAdapter(name_.c_str());
            if (!adapter_) {
                throw std::runtime_error("Failed to create or open Wintun adapter");
            }
        }
    }

    TunDevice::~TunDevice() {
        Stop();
        if (adapter_) WintunCloseAdapter(adapter_);
        if (wintun_lib_) FreeLibrary(wintun_lib_);
    }

    void TunDevice::LoadWintun() {
        wintun_lib_ = LoadLibraryA("wintun.dll");
        if (!wintun_lib_) {
            throw std::runtime_error("Failed to load wintun.dll");
        }

        WintunCreateAdapter = (WINTUN_CREATE_ADAPTER_FUNC*)GetProcAddress(wintun_lib_, "WintunCreateAdapter");
        WintunOpenAdapter = (WINTUN_OPEN_ADAPTER_FUNC*)GetProcAddress(wintun_lib_, "WintunOpenAdapter");
        WintunCloseAdapter = (WINTUN_CLOSE_ADAPTER_FUNC*)GetProcAddress(wintun_lib_, "WintunCloseAdapter");
        WintunDeleteDriver = (WINTUN_DELETE_DRIVER_FUNC*)GetProcAddress(wintun_lib_, "WintunDeleteDriver");
        WintunStartSession = (WINTUN_START_SESSION_FUNC*)GetProcAddress(wintun_lib_, "WintunStartSession");
        WintunEndSession = (WINTUN_END_SESSION_FUNC*)GetProcAddress(wintun_lib_, "WintunEndSession");
        WintunGetReadWaitEvent = (WINTUN_GET_READ_WAIT_EVENT_FUNC*)GetProcAddress(wintun_lib_, "WintunGetReadWaitEvent");
        WintunReceivePacket = (WINTUN_RECEIVE_PACKET_FUNC*)GetProcAddress(wintun_lib_, "WintunReceivePacket");
        WintunReleaseReceivePacket = (WINTUN_RELEASE_RECEIVE_PACKET_FUNC*)GetProcAddress(wintun_lib_, "WintunReleaseReceivePacket");
        WintunAllocateSendPacket = (WINTUN_ALLOCATE_SEND_PACKET_FUNC*)GetProcAddress(wintun_lib_, "WintunAllocateSendPacket");
        WintunSendPacket = (WINTUN_SEND_PACKET_FUNC*)GetProcAddress(wintun_lib_, "WintunSendPacket");

        if (!WintunCreateAdapter || !WintunOpenAdapter || !WintunCloseAdapter || !WintunDeleteDriver ||
            !WintunStartSession || !WintunEndSession || !WintunGetReadWaitEvent || !WintunReceivePacket ||
            !WintunReleaseReceivePacket || !WintunAllocateSendPacket || !WintunSendPacket) {
            throw std::runtime_error("Failed to load one or more Wintun functions");
        }
    }

    void TunDevice::Start() {
        if (running_) return;

        session_ = WintunStartSession(adapter_, WINTUN_MAX_RING_CAPACITY);
        if (!session_) {
            throw std::runtime_error("Failed to start Wintun session");
        }

        running_ = true;
        receive_thread_ = std::thread(&TunDevice::ReceiveLoop, this);
    }

    void TunDevice::Stop() {
        running_ = false;
        if (session_) {
            WintunEndSession(session_);
            session_ = nullptr;
        }
        if (receive_thread_.joinable()) {
            receive_thread_.join();
        }
    }

    void TunDevice::SetReceiveCallback(ReceiveCallback cb) {
        on_receive_ = cb;
    }

    void TunDevice::Write(const std::vector<uint8_t>& packet) {
        if (!session_) return;

        DWORD size = static_cast<DWORD>(packet.size());
        BYTE* buffer = WintunAllocateSendPacket(session_, size);
        if (buffer) {
            memcpy(buffer, packet.data(), size);
            WintunSendPacket(session_, buffer);
        } else {
            // Handle error (buffer full, etc.)
            // For now, silently drop or log.
        }
    }

    void TunDevice::ReceiveLoop() {
        HANDLE wait_event = WintunGetReadWaitEvent(session_);

        while (running_) {
            DWORD size;
            BYTE* packet = WintunReceivePacket(session_, &size);
            
            if (packet) {
                if (on_receive_) {
                    std::vector<uint8_t> data(packet, packet + size);
                    on_receive_(data);
                }
                WintunReleaseReceivePacket(session_, packet);
            } else {
                DWORD error = GetLastError();
                if (error == ERROR_NO_MORE_ITEMS) {
                    WaitForSingleObject(wait_event, 100); // Wait for data
                } else {
                    // Error
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }
        }
    }

}
