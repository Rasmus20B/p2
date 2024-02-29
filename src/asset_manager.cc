module;

#include <filesystem>
#include <fstream>
#include <print>
#include <vector>
#include <thread>

#include <raylib.h>

import types;

export module asset_manager;

export struct Asset_Manager {

  template<typename T>
  struct Asset {
    std::string path;
    std::filesystem::file_time_type ftime;
    T data;
  };

  void init_asset_manager() {
    for(const auto& dirEntry : std::filesystem::recursive_directory_iterator("../assets/")) {
      auto path = dirEntry.path().string();
      std::println("FILE FOUND: {}", path);
      if(path.ends_with(".png")) {
        sprites.push_back({
            path.c_str(),
            dirEntry.last_write_time(),
            LoadTexture(dirEntry.path().string().c_str()),
        });
      } else if(path.ends_with(".dml")) {
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<u8> script((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        scripts.push_back({
            path.c_str(),
            dirEntry.last_write_time(),
            script,
            });
      }
    }
  }

  void check_files() {
    for(auto &s: sprites) {
      auto handle = std::filesystem::directory_entry(s.path);
      if(handle.exists() && s.ftime < handle.last_write_time()) {
        s.data = LoadTexture(s.path.data());
        s.ftime = handle.last_write_time();
      }
    }
  }

  Texture2D get_sprite(size_t idx) {
    return sprites[idx].data;
  }
  

  std::vector<Asset<Texture2D>> sprites;
  std::vector<Asset<std::vector<u8>>> scripts;
};

export inline Asset_Manager assets;
