// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/torque/source-positions.h"
#include "src/torque/torque-compiler.h"

#include <iterator>
#include <filesystem>

namespace v8 {
namespace internal {
namespace torque {

std::string ErrorPrefixFor(TorqueMessage::Kind kind) {
  switch (kind) {
    case TorqueMessage::Kind::kError:
      return "Torque Error";
    case TorqueMessage::Kind::kLint:
      return "Lint error";
  }
}

static std::vector<std::string> SplitFiles(std::string s)
{
  std::stringstream ss(s);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  return std::vector<std::string>(begin, end);
}

int WrappedMain(int argc, const char** argv) {
  TorqueCompilerOptions options;
  options.collect_language_server_data = false;
  options.force_assert_statements = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; ++i) {
    // Check for options
    const std::string argument(argv[i]);
    if (argument == "-o") {
      options.output_directory = argv[++i];
    } else if (argument == "-v8-root") {
      options.v8_root = std::string(argv[++i]);
    } else if (argument == "-m32") {
      options.force_32bit_output = true;
    } else {
      // Otherwise it's a .tq file. Remember it for compilation.

      for (const auto& f: SplitFiles(std::move(argument)))      
      {
        if (!StringEndsWith(f, ".tq")) {
          std::cerr << "Unexpected command-line argument \"" << f
                    << "\", expected a .tq file.\n";
          base::OS::Abort();
        }
        files.push_back(f);
      }
    }

    std::set<std::string> paths;
    if (options.output_directory.length())
    {
      for(const auto& f : files)
      {
        auto rootPath = std::filesystem::path(options.output_directory);
        auto filePath = rootPath / f;
        auto pathStr = filePath.parent_path().string();
        if (paths.count(pathStr) == 0)
          paths.insert(pathStr);
      }

      for(const auto& p : paths)
      {
        std::filesystem::create_directory(p);
      }
    }
  }

  TorqueCompilerResult result = CompileTorque(files, options);

  // PositionAsString requires the SourceFileMap to be set to
  // resolve the file name. Needed to report errors and lint warnings.
  SourceFileMap::Scope source_file_map_scope(*result.source_file_map);

  for (const TorqueMessage& message : result.messages) {
    if (message.position) {
      std::cerr << *message.position << ": ";
    }

    std::cerr << ErrorPrefixFor(message.kind) << ": " << message.message
              << "\n";
  }

  if (!result.messages.empty()) v8::base::OS::Abort();

  return 0;
}

}  // namespace torque
}  // namespace internal
}  // namespace v8

int main(int argc, const char** argv) {
  return v8::internal::torque::WrappedMain(argc, argv);
}
