# CodeCoach+ — Project Plan

## Overview
CodeCoach+ helps code coaches collect, analyze, and present student project examples across platforms (Scratch, PixelPad, Unity). This plan provides a step-by-step implementation path, suggested technologies, JSON formats for the three target platforms, testing checklist, and clarifying questions.

## Prerequisites
- Development OS: Linux (Ubuntu). macOS/Windows support later.
- Tooling: C++17, Qt 6 Widgets, CMake, SQLite, Git.
- Libraries: `nlohmann/json` (JSON), `tesseract` + `leptonica` (local OCR).
- Optional future: OpenAI / Claude API for AI augmentation (not in current scope).

## High-level phases (summary)
1. Requirements & data model ✅
2. JSON schema design for Scratch, PixelPad, Unity ✅
3. Project scaffold (C++ backend + Qt UI + DB) ✅
4. Core backend: data model + DB ✅
5. Prototype image pipeline (Tesseract OCR + heuristics) ✅
6. ~~AI augmentation~~ → **deferred — see Future Enhancements**
7. ~~CLI + API endpoints~~ → **deferred — direct Qt↔backend linkage used instead**
8. Qt Widgets UI ← **current step**
9. Tests, docs, and packaging

## Step-by-step Implementation

1) [x] Requirements & scope (1–2 days)
- Confirm target platforms (Scratch, PixelPad, Unity) and expected artifacts.
- Decided: local SQLite DB + media folder, local Tesseract OCR (no cloud).
- Deliverable: requirements spec and acceptance criteria.

2) [x] Design JSON schemas (1–2 days)
- Created `schemas/` directory with `.json` schema files for all three platforms.
- Deliverable: `schemas/` directory with `.json` schema files.

3) [x] Project scaffold (1 day)
- CMake project with `backend/`, `src/`, `schemas/`, `data/` directories.
- Deliverable: minimal app that opens and prints "CodeCoach+ ready".

4) [x] Core backend: data model + DB (2–3 days)
- `Model.h` — C++ structs for Scratch, PixelPad, Unity payloads with nlohmann/json serialization.
- `Database.h/.cpp` — SQLite connection + schema migration.
- `Repository.h/.cpp` — save / getEntry / listEntries.
- `EntryFactory.h/.cpp` — platform-specific entry construction.
- `Validation.h/.cpp` — payload validation per platform.
- Deliverable: store/retrieve program entries with JSON payloads.

5) [x] Prototype image pipeline (3–5 days)
- `backend/ImageAnalyzer.h/.cpp` — Leptonica image load → 8-bit grayscale → Tesseract OCR.
- Platform detection: weighted keyword scoring (Scratch / PixelPad / Unity), 0–100 confidence.
- Structure extraction: maps OCR lines into ScratchPayload / PixelPadPayload / UnityPayload.
- `src/analyze_image.cpp` — CLI tool: `analyze_image <image> [--title T] [--author A] [--platform P]`.
- Deliverable: `analyze_image` outputs Entry-compatible JSON to stdout.

6) [DEFERRED] AI augmentation — see Future Enhancements below.

7) [DEFERRED] CLI + API endpoints — Qt UI links the backend library directly (same process);
   no HTTP layer needed for a local desktop app.

8) [ ] Qt Widgets UI (current step)
- Wireframes: Add Entry (image drop + form), Browse/Search, Detail view, Edit.
- `MainWindow` — tab or stacked-widget shell.
- `AddEntryDialog` — file picker, runs analyzeImage(), shows auto-filled form, manual edit, save.
- `BrowseWidget` — searchable table of all entries with thumbnail column.
- `DetailWidget` — full entry view with payload JSON panel.
- Toolchain: Qt 6 Widgets, CMake `find_package(Qt6 COMPONENTS Widgets REQUIRED)`.
- Deliverable: cross-platform Qt app that can add, browse, and view entries.

9) [ ] Testing, docs, packaging (2–4 days)
- Unit tests for parsers, DB layer, and analysis pipeline.
- Integration tests for full flow with sample images.
- Create `README.md`, developer setup guide, and user guide.
- Build installers or publish portable app bundles.

## Suggested JSON schemas (concise examples)

### Scratch
```json
{
  "id": "uuid",
  "title": "Move and Collect",
  "platform": "scratch",
  "author": "Student Name",
  "description": "Brief summary written by coach",
  "tags": ["game","sprite","loops"],
  "visual": {
    "thumbnail": "media/uuid.png",
    "stage": [
      {"sprite": "Cat", "scripts": [ {"when": "greenFlag", "blocks": ["move 10","turn 15"]} ] }
    ]
  },
  "metadata": {"created_at": "2026-03-13T12:00:00Z", "source_image": "media/uuid_src.jpg"}
}
```

### PixelPad
```json
{
  "id": "uuid",
  "title": "Pixel Runner",
  "platform": "pixelpad",
  "classes": [
    {"name": "Player", "type": "Class", "methods": [ {"name":"loop","code":"..."} ] }
  ],
  "assets": {"sprites": ["media/uuid_sprite.png"]},
  "metadata": {"language":"pixelpad","source_image":"media/uuid.jpg"}
}
```

### Unity
```json
{
  "id": "uuid",
  "title": "Simple Platformer",
  "platform": "unity",
  "objects": [
    {"name":"Player","components":[{"type":"Rigidbody2D"},{"type":"PlayerController","code":"public class PlayerController { ... }"}]}
  ],
  "metadata": {"language":"c#","source_image":"media/uuid.png"}
}
```

## Implementation details & engineering notes
- Qt UI links `backend_lib` directly — no HTTP API layer needed.
- All image analysis runs locally via Tesseract; internet not required.
- OCR output may contain minor character errors; the Edit form in the UI is the correction surface.
- For AI features (when added): call Claude or OpenAI API asynchronously from the UI with a loading indicator.

## Testing checklist
- Unit tests for JSON serialization/deserialization.
- Test image pipeline on a variety of photos (good lighting, skewed, mobile photos).
- Manual review and correction flow in UI.

## Deliverables (milestones)
- M1: Repo scaffold + schemas + minimal CLI ✅
- M2: Working `analyze_image` prototype ✅
- M3: Qt Widgets UI with add/browse/view/edit ← in progress
- M4: Tests, docs, packaging

## Future Enhancements (post-MVP)
These were originally Steps 6 and 7. Deferred to keep scope focused on a working local UI.

### AI augmentation
- Use Claude API or OpenAI to clean OCR output, infer program purpose, generate descriptions.
- Add a "Generate description" button in the Edit form that calls the AI API asynchronously.
- Implement prompt templates per platform and fallback heuristics.
- Requires: `ANTHROPIC_API_KEY` or `OPENAI_API_KEY` environment variable.

### HTTP API / CLI enhancements
- Small local REST API (Qt Network or cpp-httplib) for external tool integration.
- Richer CLI: `codecoach add`, `codecoach list`, `codecoach search`.

### Cloud sync & multi-user
- Optional cloud storage for entries and media.
- User accounts and sharing between coaches.

### Offline OCR improvements
- OpenCV preprocessing (deskew, adaptive threshold) for better accuracy on phone photos.
- Platform-specific Tesseract page segmentation modes.
