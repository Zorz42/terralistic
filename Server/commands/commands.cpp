#include "commands.hpp"

void Commands::onEvent(ServerChatEvent& event) {
    if(event.message[0] == '/') {
        startCommand(event.message, event.sender);
        event.cancelled = true;
    }
}

void Commands::init() {
    chat->chat_event.addListener(this);
    
    commands.push_back(&tp_command);
    commands.push_back(&give_command);
    commands.push_back(&setblock_command);
    commands.push_back(&health_command);
}

void Commands::stop() {
    chat->chat_event.removeListener(this);
}

int formatCoord(std::string coord_str, int curr_coord) {
    int coord = 0;
    if(coord_str[0] == '~') {
        coord += curr_coord;
        coord_str.erase(coord_str.begin());
    }
    if(!coord_str.empty())
        coord += std::stoi(coord_str);
    return coord;
}

void TpCommand::onCommand(const std::vector<std::string>& args, ServerPlayer* executor) {
    if(args.size() >= 2) {
        int x_coord = formatCoord(args[0], executor->getX() / 16), y_coord = formatCoord(args[1], -executor->getY() / 16 + blocks->getHeight());
        ServerPlayer* to_teleport = executor;
        if(args.size() >= 3)
            to_teleport = players->getPlayerByName(args[2]);
        sf::Packet feedback_message;
        feedback_message << ServerPacketType::CHAT << "Successfully teleported " + to_teleport->name + " to " + std::to_string(x_coord) + " " + std::to_string(y_coord);
        //feedback_message << ServerPacketType::CHAT <<
        executor->getConnection()->send(feedback_message);
        y_coord = -y_coord + blocks->getHeight();
        entities->setX(to_teleport, x_coord * 16);
        entities->setY(to_teleport, y_coord * 16);
        return;
    }
    sf::Packet error_message;
    error_message << ServerPacketType::CHAT << "Arguments not formatted correctly. Type /help tp for a list of formats.";
    executor->getConnection()->send(error_message);
}

void GiveCommand::onCommand(const std::vector<std::string>& args, ServerPlayer* executor) {
    if(args.size() >= 1) {
        ItemType* item = items->getItemTypeByName(args[0]);
        int quantity = 1;
        if(args.size() >= 2)
            quantity = std::stoi(args[1]);
        executor->inventory.addItem(item, quantity);
    }
}

void SetHealthCommand::onCommand(const std::vector<std::string>& args, ServerPlayer* executor) {
    if(args.size() == 1) {
        executor->setPlayerHealth(std::stoi(args[0]));
    }
    else{
        players->getPlayerByName(args[0])->setPlayerHealth(std::stoi(args[1]));
    }

}

void SetblockCommand::onCommand(const std::vector<std::string>& args, ServerPlayer* executor) {
    if(args.size() >= 3) {
        int x_coord = formatCoord(args[0], executor->getX() / 16), y_coord = formatCoord(args[1], -executor->getY() / 16 + blocks->getHeight());
        BlockType* block = blocks->getBlockTypeByName(args[2]);
        y_coord = -y_coord + blocks->getHeight();
        blocks->setBlockType(x_coord, y_coord, block);
    }
}

void Commands::startCommand(std::string message, ServerPlayer* player) {
    std::vector<std::string> args;
    size_t pos = message.find(' ');
    while (pos != std::string::npos) {
        args.push_back(message.substr(0, pos));
        message.erase(0, pos + 1);
        pos = message.find(' ');
    }
    args.push_back(message.substr(0, pos));
    
    std::string indentifier = args[0];
    args.erase(args.begin());
    indentifier.erase(indentifier.begin());

    for(int i = 0; i < commands.size(); i++)
        if(commands[i]->indetifier == indentifier) {
            commands[i]->onCommand(args, player);
            return;
        }
    sf::Packet error_message;
    error_message << ServerPacketType::CHAT << "Command not recognised. Type /help for a list of commands.";
    player->getConnection()->send(error_message);
}


































