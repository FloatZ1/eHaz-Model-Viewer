#pragma once

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class CFileSystem {
public:
  std::vector<std::string> extensions;
  fs::path root;

  void PrintHelp(const char *exeName) const {
    std::cout << "Usage:\n"
                 "  "
              << exeName
              << " [options]\n\n"
                 "Options:\n"
                 "  --help            Show this help message\n"
                 "  --root <path>     Root directory to scan (default: .)\n"
                 "  --ext <ext...>    File extensions to include\n"
                 "                    Example: --ext .png .jpg .obj\n\n"
                 "Example:\n"
                 "  "
              << exeName << " --root assets --ext .png .jpg\n";
  }

  void SetFromCommandLine(int argc, char **argv) {
    const char *exeName = argv[0];

    for (int i = 1; i < argc; ++i) {
      std::string arg = argv[i];

      if (arg == "--help") {
        PrintHelp(exeName);
        std::exit(0);
      } else if (arg == "--root" && i + 1 < argc) {
        root = argv[++i];
      } else if (arg == "--ext") {
        extensions.clear();

        while (i + 1 < argc && argv[i + 1][0] != '-') {
          extensions.push_back(argv[++i]);
        }
      }
    }

    // Defaults
    if (root.empty())
      root = ".";

    if (extensions.empty())
      extensions = {".hzmdl", ".ahzm", ".glb"};
  }

  std::vector<std::string> GetFilesFromRoot() {
    std::vector<std::string> result;

    if (!fs::exists(root))
      return result;

    for (const auto &entry : fs::recursive_directory_iterator(root)) {
      if (!entry.is_regular_file())
        continue;

      auto ext = entry.path().extension().string();
      if (!extensions.empty() && std::find(extensions.begin(), extensions.end(),
                                           ext) == extensions.end())
        continue;

      result.push_back(fs::relative(entry.path(), root).generic_string());
    }

    return result;
  }
};
