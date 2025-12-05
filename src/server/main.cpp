#include "TunDevice.h"
#include "UdpSocket.h"
#include "Session.h"
#include "Protocol.h"
#include <iostream>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>

using namespace vpn;

// Helper to compare sockaddr_in
struct SockAddrCmp {
    bool operator()(const sockaddr_in& a, const sockaddr_in& b) const {
        if (a.sin_addr.s_addr != b.sin_addr.s_addr) return a.sin_addr.s_addr < b.sin_addr.s_addr;
        return a.sin_port < b.sin_port;
    }
};

std::map<sockaddr_in, std::shared_ptr<Session>, SockAddrCmp> sessions;
std::map<uint32_t, std::shared_ptr<Session>> ip_sessions; // Virtual IP -> Session
std::mutex sessions_mutex;

utils::UdpSocket udp_socket;
std::unique_ptr<tun::TunDevice> tun_device;

void HandleTunPacket(const std::vector<uint8_t>& packet) {
    if (packet.size() < 20) return; // Min IPv4 header

    // Extract Dest IP (Offset 16)
    uint32_t dest_ip = *reinterpret_cast<const uint32_t*>(packet.data() + 16);
    // dest_ip is network byte order? Yes.
    // Our map should store network byte order.

    std::lock_guard<std::mutex> lock(sessions_mutex);
    auto it = ip_sessions.find(dest_ip);
    if (it != ip_sessions.end()) {
        auto session = it->second;
        if (session->IsEstablished()) {
            auto encrypted = session->Encrypt(packet);
            // We need to know the endpoint.
            // Session doesn't store endpoint. We need to store it in the map or session.
            // Let's iterate sessions map to find the endpoint? Slow.
            // Better: Store endpoint in Session or separate map.
            // For now, let's just find it in sessions map by value? No.
            // Let's add endpoint to Session class or just keep a reverse map.
            // Actually, we can just store the endpoint in the session object if we modify it, 
            // or use a map: Session* -> Endpoint.
        }
    }
}
// Wait, I need to send to the correct endpoint.
// I will modify the map structure.
// Map: VirtualIP -> {Session, Endpoint}

struct ClientContext {
    std::shared_ptr<Session> session;
    sockaddr_in endpoint;
};

std::map<uint32_t, ClientContext> clients; // Virtual IP -> Context
std::map<sockaddr_in, ClientContext*, SockAddrCmp> endpoint_map; // Endpoint -> Context

void HandleTunPacket_Revised(const std::vector<uint8_t>& packet) {
    if (packet.size() < 20) return;
    uint32_t dest_ip = *reinterpret_cast<const uint32_t*>(packet.data() + 16);

    std::lock_guard<std::mutex> lock(sessions_mutex);
    auto it = clients.find(dest_ip);
    if (it != clients.end()) {
        auto& ctx = it->second;
        if (ctx.session->IsEstablished()) {
            auto encrypted = ctx.session->Encrypt(packet);
            udp_socket.SendTo(ctx.endpoint, encrypted);
        }
    }
}

int main() {
    try {
        std::cout << "Starting VPN Server..." << std::endl;

        // Initialize TUN
        // Name: "VPNServer", Type: "Wintun"
        tun_device = std::make_unique<tun::TunDevice>(L"VPNServer", L"Wintun");
        tun_device->SetReceiveCallback(HandleTunPacket_Revised);
        tun_device->Start();
        
        // Configure IP for TUN (User must do this manually or we use netsh)
        std::cout << "TUN Device created. Please configure IP: 10.0.0.1/24" << std::endl;
        std::cout << "Command: netsh interface ip set address name=\"VPNServer\" static 10.0.0.1 255.255.255.0" << std::endl;
        // We could run this command automatically.
        system("netsh interface ip set address name=\"VPNServer\" static 10.0.0.1 255.255.255.0");

        // Bind UDP
        udp_socket.Bind(51820);
        std::cout << "Listening on UDP 51820" << std::endl;

        std::vector<uint8_t> buffer(65535);
        sockaddr_in sender;

        while (true) {
            int bytes = udp_socket.ReceiveFrom(buffer, sender);
            if (bytes > 0) {
                std::vector<uint8_t> packet(buffer.begin(), buffer.begin() + bytes);
                
                std::lock_guard<std::mutex> lock(sessions_mutex);
                auto it = endpoint_map.find(sender);
                
                if (it != endpoint_map.end()) {
                    // Existing client
                    auto& ctx = *it->second;
                    auto pp = protocol::ParsePacket(packet);
                    
                    if (pp.type == protocol::PacketType::Data) {
                        auto decrypted = ctx.session->Decrypt(pp.payload);
                        if (!decrypted.empty()) {
                            tun_device->Write(decrypted);
                            
                            // Update Virtual IP map if needed (Source IP learning)
                            if (decrypted.size() >= 20) {
                                uint32_t src_ip = *reinterpret_cast<const uint32_t*>(decrypted.data() + 12);
                                if (clients.find(src_ip) == clients.end()) {
                                    clients[src_ip] = ctx;
                                }
                            }
                        }
                    }
                } else {
                    // New client?
                    auto pp = protocol::ParsePacket(packet);
                    if (pp.type == protocol::PacketType::ClientHello) {
                        std::cout << "New Client Handshake" << std::endl;
                        auto session = std::make_shared<Session>(true);
                        auto response = session->HandleHandshake(packet);
                        
                        if (!response.empty()) {
                            udp_socket.SendTo(sender, response);
                            
                            // Create Context
                            // We don't know VIP yet. We can assign one or wait for first packet.
                            // Let's just store in endpoint map for now.
                            // We need a temporary storage or just use a dummy VIP or just endpoint map.
                            // But HandleTunPacket needs VIP.
                            // We'll assign a dynamic VIP? Or assume client configures 10.0.0.x
                            // For now, we rely on Source IP learning from first data packet.
                            
                            // We need to store the session to handle Data packets.
                            // We'll use a temporary context in endpoint_map.
                            // But we can't put it in clients map yet.
                            // We need a way to store "Unassigned" clients.
                            // Actually, we can just new up a context.
                            
                            // Hack: We'll use the endpoint map to hold the session.
                            // The clients map (VIP) will be populated when we see the first decrypted packet.
                            
                            // Wait, we need to store the context somewhere persistent.
                            // Let's make a list of contexts?
                            // Or just `new` it and manage memory?
                            // Let's use shared_ptr.
                            
                            // We need a container for contexts.
                            // Let's use a list of contexts, and maps point to them.
                            // For simplicity:
                            // We'll just put it in endpoint_map.
                            // But endpoint_map value is ClientContext*.
                            // We need to own the ClientContext.
                            // Let's make endpoint_map value ClientContext (copy) or shared_ptr.
                            // Let's change map to shared_ptr.
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
