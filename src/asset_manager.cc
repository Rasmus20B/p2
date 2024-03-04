module;

#include <utility>
#include <filesystem>
#include <fstream>
#include <print>
#include <vector>
#include <thread>
#include <memory>

#include <raylib.h>

import types;

export module asset_manager;

export enum class SpriteRef {
  PLAYER1,
  ORB1,
  ORB2,
  ENM1,
  DROP1,
};

struct Asset_Manager {

  template<typename T>
  struct Asset {
    std::string path;
    std::filesystem::file_time_type ftime;
    std::shared_ptr<T> data;
  };

  void init_asset_manager() {
    for(const auto& dirEntry : std::filesystem::recursive_directory_iterator("../assets/")) {
      auto path = dirEntry.path().string();
      std::println("FILE FOUND: {}", path);
      if(path.ends_with(".png")) {
        Asset tmp = {
            path,
            dirEntry.last_write_time(),
            std::make_shared<Texture2D>(LoadTexture(dirEntry.path().string().c_str()))
            };
        sprites.push_back( tmp );
        ;
      } else if(path.ends_with(".dml")) {
        std::ifstream instream(path, std::ios::in | std::ios::binary);
        std::vector<u8> script((std::istreambuf_iterator<char>(instream)), std::istreambuf_iterator<char>());
        scripts.push_back({
            path.c_str(),
            dirEntry.last_write_time(),
            std::make_shared<std::vector<u8>>(script),
            });
      }
    }
  }

  void update_files() {
    for(auto &s: sprites) {
      auto handle = std::filesystem::directory_entry(s.path);
      if(handle.exists() && s.ftime < handle.last_write_time()) {
        s.data.reset();
        s.data = std::make_shared<Texture2D>(LoadTexture(s.path.data()));
        s.ftime = handle.last_write_time();
      }
    }
  }

  std::shared_ptr<Texture2D> get_sprite(SpriteRef idx) {
    return sprites[std::to_underlying(idx)].data;
  }

  std::vector<Asset<Texture2D>> sprites;
  std::vector<Asset<std::vector<u8>>> scripts;
};

export inline Asset_Manager assets;
