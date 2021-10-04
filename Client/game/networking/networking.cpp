#include "clientNetworking.hpp"

void NetworkingManager::sendPacket(sf::Packet& packet) {
    sf::Socket::Status status = sf::Socket::Partial;
    while(status == sf::Socket::Partial)
        status = socket.send(packet);
}

void NetworkingManager::checkForPackets() {
    sf::Packet packet;
    
    while(true) {
        sf::Socket::Status status = socket.receive(packet);
        if(status != sf::Socket::NotReady && status != sf::Socket::Disconnected) {
            PacketType packet_type;
            packet >> packet_type;
            ClientPacketEvent event(packet, packet_type);
            packet_event.call(event);
        } else
            break;
    }
}

bool NetworkingManager::establishConnection(const std::string &ip, unsigned short port) {
    return socket.connect(ip, port) == sf::Socket::Done;
}

void NetworkingManager::disableBlocking() {
    socket.setBlocking(false);
}

void NetworkingManager::closeConnection() {
    socket.disconnect();
}

sf::Packet NetworkingManager::getPacket() {
    sf::Packet packet;
    socket.receive(packet);
    return packet;
}

std::vector<char> NetworkingManager::getData() {
    int size;
    std::size_t temp;
    socket.receive((char*)&size, sizeof(int), temp);
    
    std::vector<char> data(size);
    int bytes_received = 0;
    size_t received;
    while(bytes_received < size) {
        socket.receive(&data[bytes_received], size, received);
        bytes_received += received;
    }
    return data;
}
