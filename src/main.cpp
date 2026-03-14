#include <iostream>
#include <exception>
#include <string>
#include "../backend/Database.h"

int main(int argc, char** argv) {
    try {
        // Ensure data directory exists at runtime (creator will handle in real app)
        Database db("data/codecoach.db");
        db.initSchema();
        std::cout << "CodeCoach+ ready" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Startup error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
