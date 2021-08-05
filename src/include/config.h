#pragma once
#include <filesystem>
#include <string>
#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

class Config {
public:
  std::filesystem::path root_path, config_path, images_path, debug_images;
  cv::Size boardSize;
  float squareSize;
  static Config &
  get_single(std::filesystem::path root = std::filesystem::path("D:\\debug"),
             const cv::Size &boardSize = cv::Size(8, 2),
             float squareSize = 30.8) {
    static Config single(root, boardSize, squareSize);
    return single;
  }
  ~Config() {}

private:
  Config(std::filesystem::path root, const cv::Size &boardSize,
         float squareSize)
      : root_path(root), config_path(root / "config"),
        images_path(root / "images"), debug_images(root / "debug_images"),
        boardSize(boardSize), squareSize(squareSize) {
    init_directory();
  }
  Config(const Config &) = delete;
  Config &operator=(const Config &) const = delete;
  void init_directory() {
    auto create_and_permiss = [](std::filesystem::path path) {
      std::filesystem::create_directories(path);
      std::filesystem::permissions(path,
                                   std ::filesystem::perms::owner_all |
                                       std::filesystem::perms::group_all,
                                   std::filesystem::perm_options::add);
    };
    create_and_permiss(root_path);
    create_and_permiss(config_path);
    create_and_permiss(images_path);
    create_and_permiss(debug_images);
  }
public:
  void save_img(std::string path, std::string name, cv::Mat &img) const {
    auto create_and_permiss = [](std::filesystem::path path) {
        std::filesystem::create_directories(path);
        std::filesystem::permissions(path,
                                    std ::filesystem::perms::owner_all |
                                        std::filesystem::perms::group_all,
                                    std::filesystem::perm_options::add);
    };
    std::filesystem::path fs(path);
    create_and_permiss(fs);
    fs/= name;
    std::cout <<fs.string() << std::endl;
    cv::imwrite(fs.string(),img);
  }
};