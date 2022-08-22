// Install with:
// `PS1='$(/cygdrive/c/dev/powerprompt/cmake-build-debug/bin/powerprompt.exe)'`

#include <algorithm>
#include <boost/process.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace bp = boost::process;
namespace fs = std::filesystem;

std::string get_git_branch() {
  bp::ipstream is;
  bp::system("git branch --show-current", bp::std_err > bp::null, bp::std_out > is);
  std::string line;
  std::getline(is, line);
  return line;
}

std::vector<std::string> get_working_directory_chain() {

  std::vector<std::string> result;

  fs::path wd = std::getenv("PWD");

  std::transform(std::begin(wd), std::end(wd), std::back_inserter(result), [](fs::path const &dir) {
    return dir.string();
  });

  return result;
}

int main() {

  auto const branch = get_git_branch();

  std::string const BRIGHT_TEXT_COLOR = "\x1B[38;2;211;215;207m";
  std::string const BRANCH_TEXT = "\x1B[38;2;6;152;154m";
  std::string const BRANCH_BACKGROUND = "\x1B[48;2;6;152;154m";
  std::string const WD_TEXT = "\x1B[38;2;78;154;6m";
  std::string const WD_BACKGROUND = "\x1B[48;2;78;154;6m";
  std::string const RESET_COLORS = "\x1B[0m";

  std::string const CHEVRON_RIGHT_FULL = "\xee\x82\xb0";
  std::string const CHEVRON_RIGHT_LINE = "\xee\x82\xb1";

  std::string const OPENING_BUBBLE =  "\xee\x82\xb6";
  std::string const CLOSING_BUBBLE = "\xee\x82\xb4";

  if(!branch.empty()) {
    std::cout << BRANCH_TEXT << OPENING_BUBBLE;
    std::cout << BRIGHT_TEXT_COLOR << BRANCH_BACKGROUND;
    std::cout << ' ' << branch << ' ';
    std::cout << RESET_COLORS;
    std::cout << BRANCH_TEXT << CLOSING_BUBBLE;
    std::cout << RESET_COLORS;
    std::cout << '\n';
  }

  auto const wd_chain = get_working_directory_chain();

  auto const dir_separator = " " + CHEVRON_RIGHT_LINE + " ";
  auto const final_separator = " " + CHEVRON_RIGHT_FULL;

  if(wd_chain.size() == 1) {
    std::cout << BRIGHT_TEXT_COLOR << WD_BACKGROUND;
    std::cout << ' ' << wd_chain.front();
    std::cout << ' ';
    std::cout << RESET_COLORS << WD_TEXT;
    std::cout << CHEVRON_RIGHT_FULL;
    std::cout << RESET_COLORS;
  }
  else if(wd_chain.size() > 1) {
    std::cout << BRIGHT_TEXT_COLOR << WD_BACKGROUND;
    std::cout << ' ' << wd_chain.front();
    std::cout << dir_separator;

    std::for_each(
        std::begin(wd_chain) + 1,
        std::end(wd_chain) - 1,
        [&](auto const & e) {
          std::cout << e;
          std::cout << dir_separator;
    });

    std::cout << wd_chain.back();

    std::cout << ' ';
    std::cout << RESET_COLORS << WD_TEXT;
    std::cout << CHEVRON_RIGHT_FULL;
    std::cout << RESET_COLORS;
  }
  // else, do nothing

  std::cout << "\n$ ";

  return 0;
}
