#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace vpn::utils {

    class UdpSocket {
    public:
        UdpSocket();
        ~UdpSocket();

        void Bind(uint16_t port);
        void SendTo(const std::string& ip, uint16_t port, const std::vector<uint8_t>& data);
        void SendTo(const sockaddr_in& dest, const std::vector<uint8_t>& data);
        
        // Returns bytes read, fills sender info
        int ReceiveFrom(std::vector<uint8_t>& buffer, sockaddr_in& sender);

    private:
        SOCKET sock_ = INVALID_SOCKET;
    };

}
