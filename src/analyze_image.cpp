#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../backend/ImageAnalyzer.h"
#include <nlohmann/json.hpp>

static std::string generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis;

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << dis(gen) << "-";
    ss << std::setw(4) << (dis(gen) & 0xFFFFu) << "-";
    ss << std::setw(4) << ((dis(gen) & 0x0FFFu) | 0x4000u) << "-";
    ss << std::setw(4) << ((dis(gen) & 0x3FFFu) | 0x8000u) << "-";
    ss << std::setw(8) << dis(gen) << std::setw(4) << (dis(gen) & 0xFFFFu);
    return ss.str();
}

static std::string nowISO8601() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
    return buf;
}

static void printUsage(const char *prog) {
    std::cerr << "Usage: " << prog
              << " <image_path> [--title TITLE] [--author AUTHOR]"
                 " [--platform PLATFORM]\n"
              << "  PLATFORM override: scratch | pixelpad | unity\n";
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string imagePath = argv[1];
    std::string title = "Untitled";
    std::string author;
    std::string platformOverride;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--title" || arg == "-t") && i + 1 < argc) {
            title = argv[++i];
        } else if ((arg == "--author" || arg == "-a") && i + 1 < argc) {
            author = argv[++i];
        } else if ((arg == "--platform" || arg == "-p") && i + 1 < argc) {
            platformOverride = argv[++i];
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    try {
        codecoach::AnalysisResult result = codecoach::analyzeImage(imagePath);

        if (!platformOverride.empty()) {
            result.platform = platformOverride;
        }

        nlohmann::json output;
        output["id"]           = generateUUID();
        output["title"]        = title;
        output["platform"]     = result.platform;
        output["confidence"]   = result.confidence;
        output["payload"]      = result.payload;
        output["raw_text"]     = result.rawText;
        output["created_at"]   = nowISO8601();
        output["source_image"] = imagePath;

        if (!author.empty()) {
            output["author"] = author;
        }

        std::cout << output.dump(2) << "\n";
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
