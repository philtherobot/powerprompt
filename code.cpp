// Install with:
// `PS1='$(/cygdrive/c/dev/powerprompt/cmake-build-debug/bin/powerprompt.exe)'`

#include <algorithm>
#include <boost/process.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <regex>
#include <sstream>
#include <tuple>

namespace bp = boost::process;
namespace fs = std::filesystem;

namespace Git {

enum class WorkingDirectoryStatus {
  Modified,
  Clean
};

enum class UpstreamStatus {
  Set,
  Unset
};

struct Status {
  WorkingDirectoryStatus workingDirectoryStatus = WorkingDirectoryStatus::Clean;
  UpstreamStatus upstreamState = UpstreamStatus::Unset;
  unsigned int nbCommitsAhead = 0;
  unsigned int nbCommitsBehind = 0;
};

bool operator==(Status const &left, Status const &right) {
  return left.workingDirectoryStatus == right.workingDirectoryStatus &&
      left.upstreamState == right.upstreamState &&
      left.nbCommitsAhead == right.nbCommitsAhead &&
      left.nbCommitsBehind == right.nbCommitsBehind;
}

std::ostream & operator <<(std::ostream & os, Status const &status) {
  os << "Git::Status{" << static_cast<int>(status.workingDirectoryStatus);
  os << " " << (status.upstreamState == UpstreamStatus::Set ? "upstream branch set" : "no upstream branch");
  os << " ahead " << status.nbCommitsAhead;
  os << " behind " << status.nbCommitsBehind;
  os << "}";
  return os;
}

WorkingDirectoryStatus getWorkingDirectoryStatus(std::vector<std::string> const & gitStatusOutput) {
  return std::all_of(std::begin(gitStatusOutput), std::end(gitStatusOutput),
                     [](std::string const & line) {
                       return line.starts_with("#") || line.starts_with("?");
                     }) ? Git::WorkingDirectoryStatus::Clean : Git::WorkingDirectoryStatus::Modified;
}

UpstreamStatus getUpstreamState(std::vector<std::string> const & gitStatusOutput) {
  return std::any_of(std::begin(gitStatusOutput), std::end(gitStatusOutput),
                       [](std::string const & line) {
                         return line.starts_with("# branch.upstream");
                       }) ? Git::UpstreamStatus::Set : Git::UpstreamStatus::Unset;
}

std::tuple<unsigned int, unsigned int> getBranchRelativeHistory(std::vector<std::string> const & gitStatusOutput) {

  std::regex aheadBehindMatcher("# branch.ab \\+([\\d]+) -([\\d]+)");
  for(std::string const & line: gitStatusOutput) {
    std::smatch match;
    if(std::regex_match(line, match, aheadBehindMatcher)) {
      return {
          static_cast<unsigned int>(std::stoul(match[1])),
          static_cast<unsigned int>(std::stoul(match[2])),
          };
    }
  }
  return {0,0};
}

Status getStatus(std::istream & gitStatusOutput) {

  std::vector<std::string> lines;
  while(gitStatusOutput) {
    std::string line;
    std::getline(gitStatusOutput, line);
    lines.push_back(line);
  }

  if(!lines.empty() && lines.back().empty()) {
    lines.pop_back();
  }

  unsigned int ahead = 0;
  unsigned int behind = 0;
  std::tie(ahead, behind) = Git::getBranchRelativeHistory(lines);

  return {
      getWorkingDirectoryStatus(lines),
      getUpstreamState(lines),
      ahead,
      behind
  };
}

}

/////////////////////////////////////////////////////////

std::string getGitBranch() {
  bp::ipstream is;
  bp::system("git branch --show-current", bp::std_err > bp::null, bp::std_out > is);
  std::string line;
  std::getline(is, line);
  return line;
}

std::vector<std::string> getWorkingDirectoryChain() {

  std::vector<std::string> result;

  fs::path wd = std::getenv("PWD");

  std::transform(std::begin(wd), std::end(wd), std::back_inserter(result), [](fs::path const &dir) {
    return dir.string();
  });

  return result;
}

bool areThereUncommittedFiles(std::istream & gitStatusOutput) {
  while(gitStatusOutput) {
    std::string line;
    std::getline(gitStatusOutput, line);
    if(!line.empty() && line.front() != '?') return true;
  }
  return false;
}

bool areThereUncommittedFiles() {
  bp::ipstream is;
  bp::system("git status --porcelain", bp::std_err > bp::null, bp::std_out > is);
  return areThereUncommittedFiles(is);
}

struct Color {
  int red = 0;
  int green = 0;
  int blue = 0;
};

enum class Where { fore, back };

std::string setTerminalColor(Color color, Where where) {
  int where_code = where == Where::fore ? 38 : 48;

  std::ostringstream os;
  os << "\x1B[" << where_code << ";2;" << color.red << ';' << color.green << ';' << color.blue << 'm';
  return os.str();
}

std::string textColor(Color color) {
    return setTerminalColor(color, Where::fore);
}

std::string backColor(Color color) {
  return setTerminalColor(color, Where::back);
}

std::string setColors(Color fore, Color back) {
  return textColor(fore) + backColor(back);
}

std::string resetColors() {
  return "\x1B[0m";
}

std::string transitionIn(std::string const & symbol, Color fore, Color back) {
  return textColor(back) + symbol + setColors(fore, back);
}

std::string transitionOut(std::string const & symbol, Color previous_back, Color fore, Color back) {
  return setColors(previous_back, back) + symbol + textColor(fore);
}

std::string transitionOut(std::string const symbol, Color previous_back) {
  return resetColors() + textColor(previous_back) + symbol + resetColors();
}

namespace Colors {
Color const bright{211,215,207};
Color const notification{239,41,41};
Color const branch{6,152,154};
Color const wd{78,154,6};
}

namespace Symbols {

std::string const CHEVRON_RIGHT_FULL = "\xee\x82\xb0";
std::string const CHEVRON_RIGHT_LINE = "\xee\x82\xb1";

std::string const OPENING_BUBBLE =  "\xee\x82\xb6";
std::string const CLOSING_BUBBLE = "\xee\x82\xb4";

std::string const GIFT = "\xef\x90\xb6";
}

int program() {

  auto const branchName = getGitBranch();

  using std::cout;
  
  if(!branchName.empty()) {
    cout << transitionIn(Symbols::OPENING_BUBBLE, Colors::bright, Colors::branch);
    cout << ' ' << branchName << ' ';

    if(areThereUncommittedFiles()) {
      cout << transitionIn(Symbols::OPENING_BUBBLE, Colors::bright, Colors::notification);
      cout << ' ' << Symbols::GIFT << ' ';
      cout << transitionOut(Symbols::CLOSING_BUBBLE, Colors::notification, Colors::bright, Colors::branch);
      cout << ' ';
    }

    cout << transitionOut(Symbols::CLOSING_BUBBLE, Colors::branch);
    cout << '\n';
  }

  auto const wdChain = getWorkingDirectoryChain();

  if(!wdChain.empty()) {
    cout << setColors(Colors::bright, Colors::wd);
    cout << ' ' << wdChain.front();

    if(wdChain.size() > 1) {
      auto const dirSeparator = " " + Symbols::CHEVRON_RIGHT_LINE + " ";
      cout << dirSeparator;

      std::for_each(
          std::begin(wdChain) + 1,
          std::end(wdChain) - 1,
          [&](auto const & e) {
            std::cout << e;
            std::cout << dirSeparator;
          });

      cout << wdChain.back();
    }

    cout << ' ';
    cout << transitionOut(Symbols::CHEVRON_RIGHT_FULL, Colors::wd);
  }
  // else, do nothing

  cout << "\n$ ";

  return 0;
}
