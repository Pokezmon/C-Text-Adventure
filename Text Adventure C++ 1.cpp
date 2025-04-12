#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>

class Item {
public:
    std::string name;
    std::string description;

    Item(const std::string& name, const std::string& description)
        : name(name), description(description) {
    }

    void Inspect() const {
        std::cout << description << std::endl;
    }
};

class Room {
public:
    std::string name;
    std::string description;
    std::unordered_map<std::string, Room*> exits;
    std::vector<std::shared_ptr<Item>> items;

    Room(const std::string& name, const std::string& description)
        : name(name), description(description) {
    }

    void Describe() const {
        std::cout << name << ": " << description << std::endl;
        if (!items.empty()) {
            std::cout << "You see:" << std::endl;
            for (const auto& item : items) {
                std::cout << "- " << item->name << std::endl;
            }
        }
        if (!exits.empty()) {
            std::cout << "Exits:" << std::endl;
            for (const auto& pair : exits) {
                std::cout << "- " << pair.first << std::endl;
            }
        }
    }
};

class Game {
private:
    Room* currentRoom;
    std::unordered_map<std::string, std::unique_ptr<Room>> rooms;
    std::vector<std::shared_ptr<Item>> inventory;
    const int InventoryLimit = 10;

public:
    Game() {
        auto foyer = std::make_unique<Room>("Foyer", "A dimly lit entrance hall with a dusty chandelier.");
        auto library = std::make_unique<Room>("Library", "Shelves filled with ancient books cover the walls.");

        auto book = std::make_shared<Item>("Ancient Book", "An old leather-bound book with strange symbols on its cover.");
        library->items.push_back(book);

        foyer->exits["north"] = library.get();
        library->exits["south"] = foyer.get();

        rooms["Foyer"] = std::move(foyer);
        rooms["Library"] = std::move(library);

        currentRoom = rooms["Foyer"].get();
    }

    void Start() {
        std::cout << "Welcome to the adventure game!" << std::endl;
        currentRoom->Describe();
        std::string input;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);
            ProcessCommand(input);
        }
    }

private:
    void ProcessCommand(const std::string& command) {
        if (command == "look") {
            currentRoom->Describe();
        }
        else if (command.rfind("inspect ", 0) == 0) {
            InspectItem(command.substr(8));
        }
        else if (command.rfind("take ", 0) == 0) {
            TakeItem(command.substr(5));
        }
        else if (command.rfind("drop ", 0) == 0) {
            DropItem(command.substr(5));
        }
        else if (IsDirection(command)) {
            Move(command);
        }
        else {
            std::cout << "Unknown command." << std::endl;
        }
    }

    void InspectItem(const std::string& name) {
        for (const auto& item : currentRoom->items) {
            if (ToLower(item->name) == name) {
                item->Inspect();
                return;
            }
        }
        for (const auto& item : inventory) {
            if (ToLower(item->name) == name) {
                item->Inspect();
                return;
            }
        }
        std::cout << "There is no such item here or in your inventory." << std::endl;
    }

    void TakeItem(const std::string& name) {
        if (inventory.size() >= InventoryLimit) {
            std::cout << "Your inventory is full." << std::endl;
            return;
        }
        auto& items = currentRoom->items;
        auto it = std::find_if(items.begin(), items.end(), [&](const std::shared_ptr<Item>& item) {
            return ToLower(item->name) == name;
            });
        if (it != items.end()) {
            inventory.push_back(*it);
            items.erase(it);
            std::cout << "You take the " << name << "." << std::endl;
        }
        else {
            std::cout << "There is no such item here." << std::endl;
        }
    }

    void DropItem(const std::string& name) {
        auto it = std::find_if(inventory.begin(), inventory.end(), [&](const std::shared_ptr<Item>& item) {
            return ToLower(item->name) == name;
            });
        if (it != inventory.end()) {
            currentRoom->items.push_back(*it);
            inventory.erase(it);
            std::cout << "You drop the " << name << "." << std::endl;
        }
        else {
            std::cout << "You don't have that item." << std::endl;
        }
    }

    void Move(const std::string& direction) {
        if (currentRoom->exits.count(direction)) {
            currentRoom = currentRoom->exits[direction];
            std::cout << "You move " << direction << "." << std::endl;
            currentRoom->Describe();
        }
        else {
            std::cout << "You can't go that way." << std::endl;
        }
    }

    bool IsDirection(const std::string& dir) {
        return dir == "north" || dir == "south" || dir == "east" || dir == "west" || dir == "up" || dir == "down";
    }

    std::string ToLower(const std::string& str) {
        std::string lower = str;
        std::transform(str.begin(), str.end(), lower.begin(), ::tolower);
        return lower;
    }
};

int main() {
    Game game;
    game.Start();
    return 0;
}
