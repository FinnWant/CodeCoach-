#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "../backend/Database.h"
#include "../backend/EntryFactory.h"
#include "../backend/Repository.h"
#include "../backend/Validation.h"

int main(int argc, char **argv) {
  try {
    std::filesystem::create_directories("data");
    Database db("data/codecoach.db");
    Repository repository(db);
    repository.initSchema();

    auto validateAndSave = [&](const Entry &entry) {
      std::string error;
      if (!codecoach::validateEntry(entry, error)) {
        throw std::runtime_error("Validation failed for " + entry.id + ": " +
                                 error);
      }
      repository.saveEntry(entry);
    };

    ScratchPayload scratchPayload;
    scratchPayload.visual.thumbnail = "media/scratch-001.png";
    scratchPayload.visual.stage = {
        ScratchStageSprite{"Cat",
                           {ScratchScript{"when green flag clicked",
                                          {"move 10 steps", "turn 15 degrees"}},
                            ScratchScript{"when I receive start",
                                          {"repeat 10", "change x by 10"}}}}};
    scratchPayload.metadata =
        ScratchMetadata{"2026-04-22T00:00:00Z", "media/scratch-001_src.jpg"};

    Entry scratchEntry = codecoach::makeScratchEntry(
        "scratch-001", "Scratch Runner", std::move(scratchPayload), "Student A",
        "A simple Scratch movement script.", {"scratch", "movement", "loops"});

    validateAndSave(scratchEntry);

    PixelPadPayload pixelpadPayload;
    pixelpadPayload.classes = {PixelPadClass{
        "Player",
        "Class",
        {PixelPadMethod{"loop", "if buttonPressed(\"A\") then jump();"}}}};
    pixelpadPayload.assets = PixelPadAssets{{"media/player.png"}};
    pixelpadPayload.metadata =
        PixelPadMetadata{"pixelpad", "media/pixelpad-001_src.jpg"};

    Entry pixelpadEntry = codecoach::makePixelPadEntry(
        "pixelpad-001", "PixelPad Jumper", std::move(pixelpadPayload),
        "Student B", "A PixelPad class with update logic.",
        {"pixelpad", "class", "game"});

    validateAndSave(pixelpadEntry);

    UnityPayload unityPayload;
    unityPayload.objects = {UnityObject{
        "Player",
        {UnityComponent{"Rigidbody2D", std::nullopt},
         UnityComponent{
             "PlayerController",
             "public class PlayerController { void Update() { Move(); } }"}}}};
    unityPayload.metadata = UnityMetadata{"c#", "media/unity-001_src.jpg"};

    Entry unityEntry = codecoach::makeUnityEntry(
        "unity-001", "Unity Platformer", std::move(unityPayload), "Student C",
        "A basic Unity object with a player controller.",
        {"unity", "c#", "platformer"});

    validateAndSave(unityEntry);

    auto entries = repository.listEntries();
    std::cout << "Saved " << entries.size() << " entries." << std::endl;
    for (const auto &entry : entries) {
      std::cout << "- " << entry.id << ": " << entry.title << " ["
                << entry.platform << "]" << std::endl;
    }

  } catch (const std::exception &e) {
    std::cerr << "Startup error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
