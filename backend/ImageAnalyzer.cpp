#include "ImageAnalyzer.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace codecoach {

// ---------------------------------------------------------------------------
// OCR
// ---------------------------------------------------------------------------

static std::string runOCR(const std::string &imagePath) {
    Pix *image = pixRead(imagePath.c_str());
    if (!image) {
        throw std::runtime_error("Could not load image: " + imagePath);
    }

    // Minimal preprocessing: convert to 8-bit grayscale
    Pix *gray = pixConvertTo8(image, 0);
    pixDestroy(&image);
    if (!gray) {
        throw std::runtime_error("Could not convert image to grayscale: " + imagePath);
    }

    tesseract::TessBaseAPI tess;
    if (tess.Init(nullptr, "eng") != 0) {
        pixDestroy(&gray);
        throw std::runtime_error("Could not initialize Tesseract (is tesseract-ocr-eng installed?)");
    }

    tess.SetImage(gray);
    char *rawText = tess.GetUTF8Text();
    std::string text = rawText ? rawText : "";
    delete[] rawText;

    tess.End();
    pixDestroy(&gray);
    return text;
}

// ---------------------------------------------------------------------------
// Platform detection heuristics
// ---------------------------------------------------------------------------

static std::string detectPlatform(const std::string &text, int &confidence) {
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lower.size() < 10) {
        confidence = 0;
        return "unknown";
    }

    int scratch = 0, pixelpad = 0, unity = 0;

    // --- Scratch (block-programming, distinctive trigger phrases) ---
    if (lower.find("when green flag") != std::string::npos)  scratch += 10;
    if (lower.find("when i receive")  != std::string::npos)  scratch += 8;
    if (lower.find("broadcast")       != std::string::npos)  scratch += 7;
    if (lower.find("costume")         != std::string::npos)  scratch += 7;
    if (lower.find("glide")           != std::string::npos)  scratch += 6;
    if (lower.find("change x by")     != std::string::npos)  scratch += 6;
    if (lower.find("set x to")        != std::string::npos)  scratch += 6;
    if (lower.find("degrees") != std::string::npos &&
        lower.find("turn")    != std::string::npos)          scratch += 6;
    if (lower.find("say") != std::string::npos &&
        lower.find("secs") != std::string::npos)             scratch += 5;
    if (lower.find("steps")           != std::string::npos)  scratch += 5;
    if (lower.find("forever")         != std::string::npos)  scratch += 5;
    if (lower.find("touching")        != std::string::npos)  scratch += 4;
    if (lower.find("sprite")          != std::string::npos)  scratch += 4;
    if (lower.find("repeat")          != std::string::npos)  scratch += 3;

    // --- PixelPad (Python-based, characteristic method names) ---
    if (lower.find("def loop")         != std::string::npos) pixelpad += 10;
    if (lower.find("def start")        != std::string::npos) pixelpad += 10;
    if (lower.find("pixelpad")         != std::string::npos) pixelpad += 10;
    if (lower.find("import pixelpad")  != std::string::npos) pixelpad += 10;
    if (lower.find("self.")            != std::string::npos) pixelpad += 5;
    if (lower.find("class ") != std::string::npos &&
        lower.find("def ")   != std::string::npos)           pixelpad += 5;
    if (lower.find("def ")             != std::string::npos) pixelpad += 3;

    // --- Unity / C# ---
    if (lower.find("monobehaviour")    != std::string::npos) unity += 10;
    if (lower.find("using unityengine") != std::string::npos) unity += 10;
    if (lower.find("input.getaxis")    != std::string::npos) unity += 8;
    if (lower.find("void update")      != std::string::npos) unity += 8;
    if (lower.find("void start")       != std::string::npos) unity += 8;
    if (lower.find("getcomponent")     != std::string::npos) unity += 7;
    if (lower.find("gameobject")       != std::string::npos) unity += 7;
    if (lower.find("rigidbody")        != std::string::npos) unity += 7;
    if (lower.find("public class")     != std::string::npos) unity += 5;
    if (lower.find("vector2") != std::string::npos ||
        lower.find("vector3") != std::string::npos)          unity += 5;
    if (lower.find("transform")        != std::string::npos) unity += 3;

    int total = scratch + pixelpad + unity;
    if (total == 0) {
        confidence = 0;
        return "unknown";
    }

    int best = std::max({scratch, pixelpad, unity});
    confidence = static_cast<int>((static_cast<double>(best) / total) * 100.0);

    if (scratch == best)  return "scratch";
    if (pixelpad == best) return "pixelpad";
    return "unity";
}

// ---------------------------------------------------------------------------
// Structure extraction helpers
// ---------------------------------------------------------------------------

static std::vector<std::string> splitLines(const std::string &text) {
    std::vector<std::string> lines;
    std::istringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
        // strip trailing CR
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

static std::string trimLeft(const std::string &s) {
    size_t pos = s.find_first_not_of(" \t");
    return pos == std::string::npos ? "" : s.substr(pos);
}

static std::string trimRight(const std::string &s) {
    size_t pos = s.find_last_not_of(" \t\r\n");
    return pos == std::string::npos ? "" : s.substr(0, pos + 1);
}

static std::string trim(const std::string &s) { return trimLeft(trimRight(s)); }

static std::string toLower(const std::string &s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return r;
}

// --- Scratch ---
static ScratchPayload extractScratch(const std::string &text) {
    ScratchPayload payload;
    ScratchStageSprite currentSprite;
    currentSprite.sprite = "Sprite1";

    ScratchScript currentScript;
    bool inScript = false;

    for (const auto &raw : splitLines(text)) {
        std::string line = trim(raw);
        if (line.empty()) continue;

        std::string lower = toLower(line);
        if (lower.rfind("when ", 0) == 0) {
            if (inScript && !currentScript.when.empty()) {
                currentSprite.scripts.push_back(currentScript);
            }
            currentScript = ScratchScript{line, {}};
            inScript = true;
        } else if (inScript) {
            currentScript.blocks.push_back(line);
        }
    }

    if (inScript && !currentScript.when.empty()) {
        currentSprite.scripts.push_back(currentScript);
    }

    // Always emit at least one sprite so the payload is structurally valid
    if (currentSprite.scripts.empty()) {
        // No triggers found — treat every non-empty line as a block on a
        // generic script so the raw OCR is preserved in the schema.
        ScratchScript generic{"when green flag clicked", {}};
        for (const auto &raw : splitLines(text)) {
            std::string line = trim(raw);
            if (!line.empty()) generic.blocks.push_back(line);
        }
        if (!generic.blocks.empty()) {
            currentSprite.scripts.push_back(generic);
        }
    }

    payload.visual.stage.push_back(currentSprite);
    return payload;
}

// --- PixelPad ---
static PixelPadPayload extractPixelPad(const std::string &text) {
    PixelPadPayload payload;

    PixelPadClass currentClass;
    currentClass.name = "Unknown";
    currentClass.type = "Class";
    bool hasClass = false;

    PixelPadMethod currentMethod;
    bool inMethod = false;

    auto flushMethod = [&]() {
        if (inMethod && !currentMethod.name.empty()) {
            currentClass.methods.push_back(currentMethod);
            currentMethod = {};
            inMethod = false;
        }
    };

    auto flushClass = [&]() {
        flushMethod();
        if (hasClass || !currentClass.methods.empty()) {
            payload.classes.push_back(currentClass);
        }
        currentClass = {};
        currentClass.type = "Class";
        hasClass = false;
    };

    for (const auto &raw : splitLines(text)) {
        std::string line = trimRight(raw);
        std::string stripped = trimLeft(line);
        std::string lower = toLower(stripped);

        if (lower.rfind("class ", 0) == 0) {
            flushClass();
            std::string rest = stripped.substr(6);
            size_t end = rest.find_first_of("(:"); // up to parent or colon
            currentClass.name = trim(end != std::string::npos ? rest.substr(0, end) : rest);
            if (currentClass.name.empty()) currentClass.name = "Unknown";
            hasClass = true;
        } else if (lower.rfind("def ", 0) == 0) {
            flushMethod();
            std::string rest = stripped.substr(4);
            size_t paren = rest.find('(');
            currentMethod.name = trim(paren != std::string::npos ? rest.substr(0, paren) : rest);
            currentMethod.code = "";
            inMethod = true;
        } else if (inMethod) {
            if (!currentMethod.code.empty()) currentMethod.code += "\n";
            currentMethod.code += line;
        }
    }

    flushClass();

    // Ensure at least one class exists
    if (payload.classes.empty()) {
        PixelPadClass fallback;
        fallback.name = "Unknown";
        fallback.type = "Class";
        fallback.methods.push_back(PixelPadMethod{"unknown", text});
        payload.classes.push_back(fallback);
    }

    return payload;
}

// --- Unity ---
static UnityPayload extractUnity(const std::string &text) {
    UnityPayload payload;

    static const std::vector<std::string> knownComponents = {
        "Rigidbody2D", "Rigidbody", "BoxCollider2D", "CircleCollider2D",
        "Collider2D", "Animator", "SpriteRenderer", "Camera",
        "AudioSource", "NavMeshAgent", "CharacterController", "Transform"
    };

    UnityObject currentObject;
    std::string currentCode;
    bool hasObject = false;

    auto flushObject = [&]() {
        if (!hasObject) return;
        // Add referenced Unity components found in the accumulated code
        for (const auto &comp : knownComponents) {
            if (currentCode.find(comp) != std::string::npos) {
                currentObject.components.push_back(UnityComponent{comp, std::nullopt});
            }
        }
        // Add the class code itself as the script component
        if (!currentCode.empty()) {
            currentObject.components.push_back(
                UnityComponent{currentObject.name, currentCode});
        }
        if (currentObject.components.empty()) {
            currentObject.components.push_back(UnityComponent{"Script", currentCode});
        }
        payload.objects.push_back(currentObject);
        currentObject = {};
        currentCode.clear();
        hasObject = false;
    };

    // Matches "class ClassName" with optional prefix (public/private/internal)
    // and tolerates OCR merging spaces: "publicclass Foo"
    static const std::regex classRegex(
        R"((?:public|private|internal)?\s*class\s+(\w+))",
        std::regex::icase);

    for (const auto &raw : splitLines(text)) {
        std::string line = trimRight(raw);
        std::string stripped = trimLeft(line);

        std::smatch m;
        if (std::regex_search(stripped, m, classRegex)) {
            flushObject();
            currentObject.name = m[1].str().empty() ? "UnknownClass" : m[1].str();
            currentCode = line + "\n";
            hasObject = true;
        } else if (hasObject) {
            currentCode += line + "\n";
        }
    }

    flushObject();

    // Ensure at least one object
    if (payload.objects.empty()) {
        UnityObject obj;
        obj.name = "UnknownObject";
        obj.components.push_back(UnityComponent{"Script", text});
        payload.objects.push_back(obj);
    }

    return payload;
}

// ---------------------------------------------------------------------------
// Public entry point
// ---------------------------------------------------------------------------

AnalysisResult analyzeImage(const std::string &imagePath) {
    AnalysisResult result;

    result.rawText = runOCR(imagePath);
    result.platform = detectPlatform(result.rawText, result.confidence);

    if (result.platform == "scratch") {
        ScratchPayload p = extractScratch(result.rawText);
        result.payload = nlohmann::json(p);
    } else if (result.platform == "pixelpad") {
        PixelPadPayload p = extractPixelPad(result.rawText);
        result.payload = nlohmann::json(p);
    } else if (result.platform == "unity") {
        UnityPayload p = extractUnity(result.rawText);
        result.payload = nlohmann::json(p);
    } else {
        result.payload = nlohmann::json::object();
    }

    return result;
}

} // namespace codecoach
