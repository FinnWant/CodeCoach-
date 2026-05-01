#include "PayloadRenderer.h"

#include <QStringList>
#include <algorithm>

namespace PayloadRenderer {

// ---------------------------------------------------------------------------
// Box-drawing helpers
// ---------------------------------------------------------------------------

static int maxLineLen(const QStringList &lines) {
    int m = 0;
    for (const auto &l : lines) m = std::max(m, static_cast<int>(l.length()));
    return m;
}

// Draws a labelled box:
//  ┌──────────────────────────┐
//  │ header                   │
//  ├──────────────────────────┤
//  │  body line               │
//  └──────────────────────────┘
static QString drawBox(const QString &header, const QStringList &bodyLines,
                       int minWidth = 32) {
    int innerW = std::max({static_cast<int>(header.length()) + 2,
                           maxLineLen(bodyLines) + 3,
                           minWidth});

    QString h   = QString("─").repeated(innerW);
    QString out;

    out += "┌" + h + "┐\n";
    out += "│ " + header.leftJustified(innerW - 1) + "│\n";

    if (!bodyLines.isEmpty()) {
        out += "├" + h + "┤\n";
        for (const auto &line : bodyLines) {
            out += "│  " + line.leftJustified(innerW - 2) + "│\n";
        }
    }

    out += "└" + h + "┘\n";
    return out;
}

// Splits a raw code string into indented QStringList, clamping very long lines.
static QStringList codeLines(const std::string &raw, const QString &indent = "  ") {
    QStringList result;
    if (raw.empty()) { result << indent + "(empty)"; return result; }
    for (const QString &l : QString::fromStdString(raw).split('\n')) {
        result << indent + l;
    }
    return result;
}

// ---------------------------------------------------------------------------
// Scratch renderer
//
//   Sprite: Cat
//   ┌──────────────────────────────────┐
//   │ when green flag clicked          │
//   ├──────────────────────────────────┤
//   │  move 10 steps                   │
//   │  turn 15 degrees                 │
//   └──────────────────────────────────┘
// ---------------------------------------------------------------------------

static QString renderScratch(const nlohmann::json &payload) {
    if (!payload.contains("visual") || !payload["visual"].contains("stage"))
        return "  (no stage data in payload)";

    QString out;

    for (const auto &spriteJ : payload["visual"]["stage"]) {
        QString spriteName =
            QString::fromStdString(spriteJ.value("sprite", "Sprite"));

        out += "  Sprite: " + spriteName + "\n\n";

        if (!spriteJ.contains("scripts") || spriteJ["scripts"].empty()) {
            out += "    (no scripts)\n\n";
            continue;
        }

        for (const auto &scriptJ : spriteJ["scripts"]) {
            QString trigger =
                QString::fromStdString(scriptJ.value("when", "(unknown trigger)"));

            QStringList blocks;
            if (scriptJ.contains("blocks")) {
                for (const auto &b : scriptJ["blocks"])
                    blocks << QString::fromStdString(b.get<std::string>());
            }
            if (blocks.isEmpty()) blocks << "(no blocks)";

            out += drawBox(trigger, blocks) + "\n";
        }
    }

    return out.trimmed();
}

// ---------------------------------------------------------------------------
// PixelPad renderer
//
//   ╔═══════════════════╗
//   ║  Class: Player    ║
//   ╚═══════════════════╝
//
//   ┌── def start() ────────────────────────┐
//   │    self.x = 100                       │
//   │    self.y = 100                       │
//   └───────────────────────────────────────┘
//
//   ┌── def loop() ─────────────────────────┐
//   │    if buttonPressed("A"):             │
//   │        jump()                         │
//   └───────────────────────────────────────┘
// ---------------------------------------------------------------------------

static QString renderPixelPad(const nlohmann::json &payload) {
    if (!payload.contains("classes"))
        return "  (no classes in payload)";

    QString out;

    for (const auto &classJ : payload["classes"]) {
        QString className =
            QString::fromStdString(classJ.value("name", "Unknown"));
        QString classType =
            QString::fromStdString(classJ.value("type", "Class"));

        // Class banner
        QString label   = "  " + classType + ": " + className + "  ";
        QString bannerH = QString("═").repeated(static_cast<int>(label.length()));
        out += "╔" + bannerH + "╗\n";
        out += "║" + label   + "║\n";
        out += "╚" + bannerH + "╝\n\n";

        if (!classJ.contains("methods") || classJ["methods"].empty()) {
            out += "    (no methods)\n\n";
            continue;
        }

        for (const auto &methodJ : classJ["methods"]) {
            QString methodName =
                QString::fromStdString(methodJ.value("name", "unknown"));
            std::string rawCode = methodJ.value("code", "");

            out += drawBox("def " + methodName + "()", codeLines(rawCode)) + "\n";
        }

        out += "\n";
    }

    return out.trimmed();
}

// ---------------------------------------------------------------------------
// Unity renderer
//
//   ◆ GameObject: Player
//   ├── ▣ Rigidbody2D
//   │
//   └── ▣ PlayerController
//          public class PlayerController {
//              void Update() { Move(); }
//          }
// ---------------------------------------------------------------------------

static QString renderUnity(const nlohmann::json &payload) {
    if (!payload.contains("objects"))
        return "  (no objects in payload)";

    QString out;

    for (const auto &objJ : payload["objects"]) {
        QString objName = QString::fromStdString(objJ.value("name", "Unknown"));
        out += "  ◆ GameObject: " + objName + "\n";

        if (!objJ.contains("components") || objJ["components"].empty()) {
            out += "  └── (no components)\n\n";
            continue;
        }

        const auto &comps = objJ["components"];
        for (size_t i = 0; i < comps.size(); ++i) {
            const auto &compJ = comps[i];
            QString compType  = QString::fromStdString(compJ.value("type", ""));
            bool    isLast    = (i == comps.size() - 1);

            QString branch = isLast ? "  └── " : "  ├── ";
            QString indent = isLast ? "         " : "  │      ";

            bool hasCode = compJ.contains("code") &&
                           compJ["code"].is_string() &&
                           !compJ["code"].get<std::string>().empty();

            out += branch + "▣ " + compType + "\n";

            if (hasCode) {
                QString code = QString::fromStdString(compJ["code"].get<std::string>());
                for (const QString &line : code.split('\n'))
                    out += indent + line + "\n";
                out += "\n";
            }
        }
        out += "\n";
    }

    return out.trimmed();
}

// ---------------------------------------------------------------------------
// Public dispatcher
// ---------------------------------------------------------------------------

QString render(const std::string &platform, const nlohmann::json &payload) {
    try {
        if (platform == "scratch")  return renderScratch(payload);
        if (platform == "pixelpad") return renderPixelPad(payload);
        if (platform == "unity")    return renderUnity(payload);
        return "(unsupported platform: " + QString::fromStdString(platform) + ")";
    } catch (const std::exception &e) {
        return "(render error: " + QString::fromStdString(e.what()) + ")";
    }
}

} // namespace PayloadRenderer
