module;

#include <filesystem>
#include <fstream>
#include <print>
#include <vector>

#include <raylib.h>

import types;

export module asset_manager;

export struct Asset_Manager {
  std::vector<Texture2D> sprites;
  std::vector<std::vector<u8>> scripts;

  void init_asset_manager() {
    for(const auto& dirEntry : std::filesystem::recursive_directory_iterator("../assets/")) {
      auto path = dirEntry.path().string();
      std::println("FILE FOUND: {}", path);
      if(path.ends_with(".png")) {
        sprites.push_back(LoadTexture(dirEntry.path().string().c_str()));
      } else if(path.ends_with(".dml")) {
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<u8> script((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        scripts.push_back(script);
      }
    }
  }
};

export inline Asset_Manager assets;
