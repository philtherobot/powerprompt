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

struct Color {
  int red = 0;
  int green = 0;
  int blue = 0;
};

bool operator ==(Color const & left, Color const & right) {
  return left.red == right.red && left.green == right.green && left.blue == right.blue;
}

namespace Colors {

namespace Details {
Color const WHITE{211,215,207};
Color const RED{228,26,28};
Color const CYAN{6,152,154};
Color const GREEN{77,175,74};
Color const BLUE{55,126,184};
}

Color const BRIGHT = Details::WHITE;
Color const MEDALLION = Details::RED;
Color const BRANCH = Details::CYAN;
Color const WD = Details::GREEN;
Color const HISTORY_SHARED = Details::WHITE;
Color const HISTORY_GROWTH_LOCAL = Details::BLUE;
Color const HISTORY_GROWTH_ORIGIN = Details::GREEN;
}

namespace Symbols {

namespace Details {

std::string const CHEVRON_RIGHT_FULL = "\xee\x82\xb0";
std::string const CHEVRON_RIGHT_LINE = "\xee\x82\xb1";

std::string const OPENING_BUBBLE =  "\xee\x82\xb6";
std::string const CLOSING_BUBBLE = "\xee\x82\xb4";

std::string const GIFT = "\xef\x90\xb6";

std::string const ANGLE_UP_DOUBLE = "\xef\x84\x82";  // those are really small
std::string const ANGLE_UP = "\xef\x84\x86";

std::string const BATTERY_10 = "\xef\x95\xba"; // UF57A
std::string const BATTERY_50 = "\xef\x95\xbd"; // UF57D
std::string const BATTERY_90 = "\xef\x96\x81"; // UF581
std::string const BATTERY_100 = "\xef\x95\xb8"; // UF578

std::string const BLOCK_LOWER_HALF = "\xe2\x96\x84"; // U2584
std::string const BLOCK_FULL = "\xe2\x96\x88"; // U2588, visually bad, optical effect of not going high enough, cuts the background color

std::string const FLAG = "\xee\x8f\x84"; // UE3C4  the single flag intended for less work actually looks bigger and more advanced than "stacked"
std::string const FLAG_STACKED = "\xee\x8f\x85";

// asterisk fbc2  or  f069   or F881
// angle double up  f102  ro F63E
// angle single up  f106
// temparature?   F2C7

}

std::string const DIR_SEPARATOR_INLINE = Details::CHEVRON_RIGHT_LINE;
std::string const DIR_SEPARATOR_FINAL = Details::CHEVRON_RIGHT_FULL;
std::string const BRANCH_OPEN = Details::OPENING_BUBBLE;
std::string const BRANCH_CLOSE = Details::CLOSING_BUBBLE;
std::string const MODIFIED = Details::GIFT;
std::string const HISTORY_SHARED = Details::BATTERY_10;
std::string const HISTORY_GROWTH = Details::BATTERY_90;
}

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
  std::string branchName;
  WorkingDirectoryStatus workingDirectoryStatus = WorkingDirectoryStatus::Clean;
  UpstreamStatus upstreamStatus = UpstreamStatus::Unset;
  unsigned int nbCommitsAhead = 0;
  unsigned int nbCommitsBehind = 0;
};

bool operator==(Status const &left, Status const &right) {
  return left.branchName == right.branchName &&
      left.workingDirectoryStatus == right.workingDirectoryStatus &&
      left.upstreamStatus == right.upstreamStatus &&
      left.nbCommitsAhead == right.nbCommitsAhead &&
      left.nbCommitsBehind == right.nbCommitsBehind;
}

std::ostream & operator <<(std::ostream & os, Status const &status) {
  os << "Git::Status{" <<status.branchName ;
  os << " " << (status.workingDirectoryStatus == WorkingDirectoryStatus::Clean ? "clean" : "modified");
  os << " " << (status.upstreamStatus == UpstreamStatus::Set ? "upstream branch set" : "no upstream branch");
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

UpstreamStatus getUpstreamStatus(std::vector<std::string> const & gitStatusOutput) {
  return std::any_of(std::begin(gitStatusOutput), std::end(gitStatusOutput),
                       [](std::string const & line) {
                         return line.starts_with("# branch.upstream");
                       }) ? Git::UpstreamStatus::Set : Git::UpstreamStatus::Unset;
}

std::string getBranchName(std::vector<std::string> const & gitStatusOutput) {

    std::regex branchMatcher("# branch.head (.+)");
    for(std::string const & line: gitStatusOutput) {
      std::smatch match;
      if(std::regex_match(line, match, branchMatcher)) {
        return match[1];
      }
    }
    return {};
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
      getBranchName(lines),
      getWorkingDirectoryStatus(lines),
      getUpstreamStatus(lines),
      ahead,
      behind
  };
}

Status getStatus() {
  bp::ipstream is;
  bp::system("git status --porcelain=2 -b", bp::std_err > bp::null, bp::std_out > is);
  return getStatus(is);
}

}

/////////////////////////////////////////////////////////

std::vector<std::string> getWorkingDirectoryChain(fs::path const & wd) {
  
  std::vector<std::string> result;

  std::transform(std::begin(wd), std::end(wd), std::back_inserter(result), [](fs::path const &dir) {
    return dir.string();
  });

  return result;
}

enum class Where { fore, back };

std::string setTerminalColor(Color color, Where where) {
  int where_code = where == Where::fore ? 38 : 48;

  std::ostringstream os;
  os << "\x1B[" << where_code << ";2;" << color.red << ';' << color.green << ';' << color.blue << 'm';
  return os.str();
}

std::string resetColors() {
  return "\x1B[0m";
}

template <typename Visitor>
void getBranchBanner(Git::Status const & status, Visitor & visitor) {
  visitor.foreColor(Colors::BRANCH);
  visitor.branchOpen();
  visitor.foreColor(Colors::BRIGHT);
  visitor.backColor(Colors::BRANCH);

  visitor.text(" ");
  visitor.text(status.branchName);

  visitor.text(" ");
  visitor.branchStatus(status);

  visitor.text(" ");
  visitor.resetColors();
  visitor.foreColor(Colors::BRANCH);
  visitor.branchClose();
  visitor.resetColors();
}

template <typename Visitor>
void getBranchStatusMedallion(Git::Status const & status, Visitor & visitor) {

  if(status.workingDirectoryStatus == Git::WorkingDirectoryStatus::Clean &&
      status.upstreamStatus == Git::UpstreamStatus::Set &&
      status.nbCommitsAhead == 0 && status.nbCommitsBehind == 0)
    return;

  visitor.foreColor(Colors::MEDALLION);
  visitor.branchOpen();
  visitor.foreColor(Colors::BRIGHT);
  visitor.backColor(Colors::MEDALLION);
  visitor.text(" ");

  switch(status.workingDirectoryStatus) {
  default:
  case Git::WorkingDirectoryStatus::Clean: break;
  case Git::WorkingDirectoryStatus::Modified:
    visitor.symbolModified();
    visitor.text(" ");
    break;
  }

  switch(status.upstreamStatus) {
  default:
  case Git::UpstreamStatus::Set: break;
  case Git::UpstreamStatus::Unset:
    visitor.text("no upstream");
    visitor.text(" ");
    break;
  }

  if(status.nbCommitsAhead != 0 || status.nbCommitsBehind != 0) {
    if(status.nbCommitsAhead == 0) {
      visitor.foreColor(Colors::HISTORY_SHARED);
      visitor.symbolHistoryShared();
    }
    else {
      visitor.foreColor(Colors::HISTORY_GROWTH_LOCAL);
      visitor.symbolHistoryGrowth();
    }

    visitor.text(" ");

    if(status.nbCommitsBehind == 0) {
      visitor.foreColor(Colors::HISTORY_SHARED);
      visitor.symbolHistoryShared();
    }
    else {
      visitor.foreColor(Colors::HISTORY_GROWTH_ORIGIN);
      visitor.symbolHistoryGrowth();
    }

    visitor.text(" ");
  }

  visitor.foreColor(Colors::MEDALLION);
  visitor.backColor(Colors::BRANCH);
  visitor.branchClose();
  visitor.foreColor(Colors::BRIGHT);
}

template <typename Visitor>
void getWorkingDirectoryBanner(fs::path const & workingDirectory, Visitor & visitor) {

  auto const wdChain = getWorkingDirectoryChain(workingDirectory);

  if (!wdChain.empty()) {

    visitor.foreColor(Colors::BRIGHT);
    visitor.backColor(Colors::WD);

    visitor.text(" ");
    visitor.text(wdChain.front());

    if (wdChain.size() > 1) {
      auto dirSeparator = [&]() {
        visitor.text(" ");
        visitor.inlineDirSeparator();
        visitor.text(" ");
      };

      dirSeparator();

      std::for_each(
          std::begin(wdChain) + 1,
          std::end(wdChain) - 1,
          [&](auto const &e) {
            visitor.text(e);
            dirSeparator();
          });

      visitor.text(wdChain.back());
    }

    visitor.text(" ");
    visitor.resetColors();
    visitor.foreColor(Colors::WD);
    visitor.finalDirSeparator();
    visitor.resetColors();
  }
}

template <typename Visitor>
void getPrompt(Git::Status const & gitStatus, fs::path const & workingDirectory, Visitor & visitor) {

  if(!gitStatus.branchName.empty()) {
    visitor.branch(gitStatus);
    visitor.newLine();
  }

  visitor.workingDirectory(workingDirectory);
  visitor.cue();
}

class TtyVisitor {
public:
  std::string codes;

  void branch(Git::Status const &status) { 
    TtyVisitor visitor;
    getBranchBanner(status, visitor);
    codes += visitor.codes;
  }

  void branchStatus(Git::Status const &status) { 
    TtyVisitor visitor;
    getBranchStatusMedallion(status, visitor);
    codes += visitor.codes;
  }

  void newLine() { codes += "\n"; }

  void workingDirectory(std::filesystem::path const &wd) { 
    TtyVisitor visitor;
    getWorkingDirectoryBanner(wd, visitor);
    codes += visitor.codes;
  }

  void cue() { codes += "\n$ "; }

  void resetColors() { codes += ::resetColors(); }

  void foreColor(Color const &c) { codes += setTerminalColor(c, Where::fore); }

  void backColor(Color const &c) { codes += setTerminalColor(c, Where::back); }

  void text(std::string const t) { codes += t; }

  void inlineDirSeparator() { codes += Symbols::DIR_SEPARATOR_INLINE; }

  void finalDirSeparator() { codes += Symbols::DIR_SEPARATOR_FINAL; }

  void branchOpen() { codes += Symbols::BRANCH_OPEN; }

  void branchClose() { codes += Symbols::BRANCH_CLOSE; }

  void symbolModified() { codes += Symbols::MODIFIED; }

  void symbolHistoryShared() { codes += Symbols::HISTORY_SHARED; }

  void symbolHistoryGrowth() { codes += Symbols::HISTORY_GROWTH; }
};

int program() {

  auto const gitStatus = Git::getStatus();
  fs::path wd = getenv("PWD"); // check for null!?

  TtyVisitor visitor;
  getPrompt(gitStatus, wd, visitor);
  std::cout << visitor.codes;
  return 0; 
}
