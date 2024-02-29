module;

#include <filesystem>
#include <print>
#include <vector>

#include <raylib.h>

export module asset_manager;

export struct Asset_Manager {
  std::vector<Texture2D> sprites;
  std::vector<std::vector<std::byte>> scripts;

  void init_asset_manager() {
    for(const auto& dirEntry : std::filesystem::recursive_directory_iterator("../assets/")) {
      if(dirEntry.path().string().ends_with(".png")) {
        sprites.push_back(LoadTexture(dirEntry.path().string().c_str()));
      }
      std::println("FILE FOUND: {}", dirEntry.path().string());
    }
  }
};

export inline Asset_Manager assets;
