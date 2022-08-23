// Install with:
// `PS1='$(/cygdrive/c/dev/powerprompt/cmake-build-debug/bin/powerprompt.exe)'`

#include <algorithm>
#include <boost/process.hpp>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>

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

bool areThereUncommittedFiles() {
  bp::ipstream is;
  bp::system("git status --porcelain", bp::std_err > bp::null, bp::std_out > is);
  while(is) {
    std::string line;
    std::getline(is, line);
    if(!line.empty() && line.front() != '?') return true;
  }
  return false;

}

struct Color {
  int red = 0;
  int green = 0;
  int blue = 0;
};

enum class Where { fore, back };

std::string set_terminal_color(Color color, Where where) {
  int where_code = where == Where::fore ? 38 : 48;

  std::ostringstream os;
  os << "\x1B[" << where_code << ";2;" << color.red << ';' << color.green << ';' << color.blue << 'm';
  return os.str();
}

std::string text_color(Color color) {
    return set_terminal_color(color, Where::fore);
}

std::string back_color(Color color) {
  return set_terminal_color(color, Where::back);
}

std::string set_colors(Color fore, Color back) {
  return text_color(fore) + back_color(back);
}

std::string reset_colors() {
  return "\x1B[0m";
}

std::string transition_in(std::string const & symbol, Color fore, Color back) {
  return text_color(back) + symbol + set_colors(fore, back);
}

std::string transition_out(std::string const & symbol, Color previous_back, Color fore, Color back) {
  return set_colors(previous_back, back) + symbol + text_color(fore);
}

std::string transition_out(std::string const symbol, Color previous_back) {
  return reset_colors() + text_color(previous_back) + symbol + reset_colors();
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

int main() {

  auto const branch_name = get_git_branch();

  using std::cout;
  
  if(!branch_name.empty()) {
    cout << transition_in(Symbols::OPENING_BUBBLE, Colors::bright, Colors::branch);
    cout << ' ' << branch_name << ' ';

    if(areThereUncommittedFiles()) {
      cout << transition_in(Symbols::OPENING_BUBBLE, Colors::bright, Colors::notification);
      cout << ' ' << Symbols::GIFT << ' ';
      cout << transition_out(Symbols::CLOSING_BUBBLE, Colors::notification, Colors::bright, Colors::branch);
      cout << ' ';
    }

    cout << transition_out(Symbols::CLOSING_BUBBLE, Colors::branch);
    cout << '\n';
  }

  auto const wd_chain = get_working_directory_chain();

  if(!wd_chain.empty()) {
    cout << set_colors(Colors::bright, Colors::wd);
    cout << ' ' << wd_chain.front();

    if(wd_chain.size() > 1) {
      auto const dir_separator = " " + Symbols::CHEVRON_RIGHT_LINE + " ";
      cout << dir_separator;

      std::for_each(
          std::begin(wd_chain) + 1,
          std::end(wd_chain) - 1,
          [&](auto const & e) {
            std::cout << e;
            std::cout << dir_separator;
          });

      cout << wd_chain.back();
    }

    cout << ' ';
    cout << transition_out(Symbols::CHEVRON_RIGHT_FULL, Colors::wd);
  }
  // else, do nothing

  cout << "\n$ ";

  return 0;
}
