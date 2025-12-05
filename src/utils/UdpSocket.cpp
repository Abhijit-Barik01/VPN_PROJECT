#include "UdpSocket.h"
#include <stdexcept>
#include <iostream>

namespace vpn::utils {

    UdpSocket::UdpSocket() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock_ == INVALID_SOCKET) {
            throw std::runtime_error("Failed to create socket");
        }
    }

    UdpSocket::~UdpSocket() {
        if (sock_ != INVALID_SOCKET) closesocket(sock_);
        WSACleanup();
    }

    void UdpSocket::Bind(uint16_t port) {
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(sock_, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            throw std::runtime_error("Failed to bind socket");
        }
    }

    void UdpSocket::SendTo(const std::string& ip, uint16_t port, const std::vector<uint8_t>& data) {
        sockaddr_in dest = {};
        dest.sin_family = AF_INET;
        inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);
        dest.sin_port = htons(port);

        SendTo(dest, data);
    }

    void UdpSocket::SendTo(const sockaddr_in& dest, const std::vector<uint8_t>& data) {
        sendto(sock_, (const char*)data.data(), (int)data.size(), 0, (sockaddr*)&dest, sizeof(dest));
    }

    int UdpSocket::ReceiveFrom(std::vector<uint8_t>& buffer, sockaddr_in& sender) {
        int sender_len = sizeof(sender);
        int bytes = recvfrom(sock_, (char*)buffer.data(), (int)buffer.size(), 0, (sockaddr*)&sender, &sender_len);
        return bytes;
    }

}
