#include "TunDevice.h"
#include "UdpSocket.h"
#include "Session.h"
#include "Protocol.h"
#include <iostream>
#include <thread>
#include <atomic>

using namespace vpn;

utils::UdpSocket udp_socket;
std::unique_ptr<tun::TunDevice> tun_device;
std::shared_ptr<Session> session;
std::string server_ip = "127.0.0.1";
uint16_t server_port = 51820;

void HandleTunPacket(const std::vector<uint8_t>& packet) {
    if (session && session->IsEstablished()) {
        auto encrypted = session->Encrypt(packet);
        udp_socket.SendTo(server_ip, server_port, encrypted);
    }
}

int main(int argc, char** argv) {
    if (argc > 1) server_ip = argv[1];

    try {
        std::cout << "Starting VPN Client..." << std::endl;

        tun_device = std::make_unique<tun::TunDevice>(L"VPNClient", L"Wintun");
        tun_device->SetReceiveCallback(HandleTunPacket);
        tun_device->Start();

        std::cout << "TUN Device created. Please configure IP: 10.0.0.2/24" << std::endl;
        std::cout << "Command: netsh interface ip set address name=\"VPNClient\" static 10.0.0.2 255.255.255.0" << std::endl;
        system("netsh interface ip set address name=\"VPNClient\" static 10.0.0.2 255.255.255.0");

        session = std::make_shared<Session>(false);
        auto hello = session->InitiateHandshake();
        udp_socket.SendTo(server_ip, server_port, hello);
        std::cout << "Sent Handshake..." << std::endl;

        std::vector<uint8_t> buffer(65535);
        sockaddr_in sender;

        while (true) {
            int bytes = udp_socket.ReceiveFrom(buffer, sender);
            if (bytes > 0) {
                std::vector<uint8_t> packet(buffer.begin(), buffer.begin() + bytes);
                auto pp = protocol::ParsePacket(packet);

                if (pp.type == protocol::PacketType::ServerHello) {
                    std::cout << "Received ServerHello" << std::endl;
                    session->HandleHandshake(packet);
                    if (session->IsEstablished()) {
                        std::cout << "Session Established!" << std::endl;
                    }
                } else if (pp.type == protocol::PacketType::Data) {
                    auto decrypted = session->Decrypt(pp.payload);
                    if (!decrypted.empty()) {
                        tun_device->Write(decrypted);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
