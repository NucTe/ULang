#include "compiler.h"

#include <filesystem>

namespace UraniumLang {

  void Help(CompilerOptions &options, const char *cmd) {
    const std::vector<std::pair<std::string, std::string>> options_ =
    {
      { "--h",       "Display this information." },
      { "--v",       "Display version." },
      { "-o <file>", "Place the output into <file>." },
    };

    std::stringstream msg("");
    auto optFunc = [&](std::string name, std::string description) {
      int nameLen = name.size();
      int spaceLen = nameLen < 25 ? 25 - nameLen : 1; // 20 = space between first character and description
      for (int i = 0; i < 2; ++i) msg << " ";
      msg << name;
      for (int i = 0; i < spaceLen; ++i) msg << " ";
      msg << description << std::endl;
    };

    msg << "Usage: " << cmd << " [options] file..." << std::endl << "Options:" << std::endl;
    for (auto opt : options_) optFunc(opt.first, opt.second);

    options._Error = true;
    options._ErrorMsg = msg.str();
  }

  CompilerOptions ParseArguments(int argc, char** argv) {
    CompilerOptions options{};

    if (argc == 1) {
      Help(options, argv[0]);
    } else {
      for (size_t i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--h") == 0) {
          Help(options, argv[0]);
          break;
        }
        else if (strcmp(argv[i], "-o") == 0) {
          if (i < argc) {
            options.Output = argv[++i];
          } else {
            Help(options, argv[0]);
            break;
          }
        }
        else { // Probably input file
          if (!std::filesystem::exists(argv[i])) {
            options._Error = true;
            options._ErrorMsg = "File " + std::string(argv[i]) + " doesn't exist!\n";
            break;
          }
          options.Input = argv[i];
        }
      }
    }

    return options;
  }

  Compiler::Compiler(const CompilerOptions &options)
    : m_Options(options) {
    m_Parser = std::make_unique<UraniumLang::Parser>(m_Options.Input);
  }

  bool Compiler::Compile() {
    auto progNode = m_Parser->Parse();

    return true;
  }

}