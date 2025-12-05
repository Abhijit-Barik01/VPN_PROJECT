#pragma once
#include <winsock2.h>
#include "wintun.h"
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>

namespace vpn::tun {

    class TunDevice {
    public:
        TunDevice(const std::wstring& name, const std::wstring& tunnel_type);
        ~TunDevice();

        void Start();
        void Stop();

        // Read packet from TUN (blocking or callback)
        // For simplicity, we'll expose a Read method or use a callback.
        // Let's use a callback for the receive loop.
        using ReceiveCallback = std::function<void(const std::vector<uint8_t>&)>;
        void SetReceiveCallback(ReceiveCallback cb);

        // Write packet to TUN
        void Write(const std::vector<uint8_t>& packet);

    private:
        void LoadWintun();
        void ReceiveLoop();

        std::wstring name_;
        std::wstring tunnel_type_;
        HMODULE wintun_lib_ = nullptr;
        WINTUN_ADAPTER_HANDLE adapter_ = nullptr;
        WINTUN_SESSION_HANDLE session_ = nullptr;
        
        std::thread receive_thread_;
        std::atomic<bool> running_ = false;
        ReceiveCallback on_receive_;

        // Function Pointers
        WINTUN_CREATE_ADAPTER_FUNC* WintunCreateAdapter = nullptr;
        WINTUN_OPEN_ADAPTER_FUNC* WintunOpenAdapter = nullptr;
        WINTUN_CLOSE_ADAPTER_FUNC* WintunCloseAdapter = nullptr;
        WINTUN_DELETE_DRIVER_FUNC* WintunDeleteDriver = nullptr;
        WINTUN_START_SESSION_FUNC* WintunStartSession = nullptr;
        WINTUN_END_SESSION_FUNC* WintunEndSession = nullptr;
        WINTUN_GET_READ_WAIT_EVENT_FUNC* WintunGetReadWaitEvent = nullptr;
        WINTUN_RECEIVE_PACKET_FUNC* WintunReceivePacket = nullptr;
        WINTUN_RELEASE_RECEIVE_PACKET_FUNC* WintunReleaseReceivePacket = nullptr;
        WINTUN_ALLOCATE_SEND_PACKET_FUNC* WintunAllocateSendPacket = nullptr;
        WINTUN_SEND_PACKET_FUNC* WintunSendPacket = nullptr;
    };

}
