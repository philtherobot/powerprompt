#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>
#include <variant>

#include "VariantStream.hpp"
#include "code.cpp"

namespace Status {

char const *const clean =
    R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -0
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const *const locallyModified =
    R"(# branch.oid d67d49339dfd71e38a3137d6c88c3f2cbeed3919
# branch.head trunk
# branch.upstream origin/trunk
# branch.ab +0 -0
1 AM N... 000000 100644 100644 0000000000000000000000000000000000000000 e69de29bb2d1d6434b8b29ae775ad8c2e48c5391 status_examples/branch_equal_to_origin
1 AM N... 000000 100644 100644 0000000000000000000000000000000000000000 e69de29bb2d1d6434b8b29ae775ad8c2e48c5391 status_examples/branch_without_upstream
? .clang-format
? cmake-build-release/
)";

char const *const withoutUpstream =
    R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const *const ahead =
    R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +1 -0
)";

char const *const behind =
    R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -1
? dummy-client/conanfile_windows.txt
? ref-data/datasets/samples/JPG/2022Apr12_11-48-21.log
)";

char const *const aheadAndBehind =
    R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +2 -13
)";

}// namespace Status

TEST_CASE("git status") {

  auto call = [](char const *const gitStatusOutput) { std::stringstream is(gitStatusOutput); return Git::getStatus(is); };

  CHECK(call(Status::clean) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::locallyModified) == Git::Status{"trunk", Git::WorkingDirectoryStatus::Modified, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::withoutUpstream) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Unset, 0, 0});
  CHECK(call(Status::ahead) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 1, 0});
  CHECK(call(Status::behind) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 1});
  CHECK(call(Status::aheadAndBehind) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 2, 13});
}

struct Branch {
  Git::Status status;
  bool operator==(Branch const &other) const { return status == other.status; }
};
std::ostream &operator<<(std::ostream &os, Branch const &) {
  os << "Branch";
  return os;
}

struct BranchMedallion {
  Git::Status status;
  bool operator==(BranchMedallion const &other) const { return status == other.status; }
};
std::ostream &operator<<(std::ostream &os, BranchMedallion const &) {
  os << "BranchMedallion";
  return os;
}

struct NewLine {
  bool operator==(NewLine const &) const { return true; }
};
std::ostream &operator<<(std::ostream &os, NewLine const &) {
  os << "NewLine";
  return os;
}

struct WorkingDirectory {
  std::filesystem::path wd;
  bool operator==(WorkingDirectory const &other) const { return wd == other.wd; }
};
std::ostream &operator<<(std::ostream &os, WorkingDirectory const &) {
  os << "WorkingDirectory";
  return os;
}

struct Cue {
  bool operator==(Cue const &) const { return true; }
};
std::ostream &operator<<(std::ostream &os, Cue const &) {
  os << "Cue";
  return os;
}

struct ResetColors {
  bool operator==(ResetColors const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, ResetColors const &) {
  os << "ResetColors";
  return os;
}

struct ForeColor {
  Color color;
  bool operator==(ForeColor const &other) const { return color == other.color; }
};
std::ostream &operator<<(std::ostream &os, ForeColor const &) {
  os << "ForeColor";
  return os;
}

struct BackColor {
  Color color;
  bool operator==(BackColor const &other) const { return color == other.color; }
};
std::ostream &operator<<(std::ostream &os, BackColor const &) {
  os << "ForeColor";
  return os;
}

struct Text {
  std::string text;
  bool operator==(Text const &other) const { return text == other.text; }
};
std::ostream &operator<<(std::ostream &os, Text const &) {
  os << "Text";
  return os;
}

struct InlineDirSeparator {
  bool operator==(InlineDirSeparator const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, InlineDirSeparator const &) {
  os << "InlineDirSeparator";
  return os;
}

struct FinalDirSeparator {
  bool operator==(FinalDirSeparator const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, FinalDirSeparator const &) {
  os << "FinalDirSeparator";
  return os;
}

struct BranchOpen {
  bool operator==(BranchOpen const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, BranchOpen const &) {
  os << "BranchOpen";
  return os;
}

struct BranchClose {
  bool operator==(BranchClose const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, BranchClose const &) {
  os << "BranchClose";
  return os;
}

struct SymbolModified {
  bool operator==(SymbolModified const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, SymbolModified const &) {
  os << "SymbolModified";
  return os;
}

struct SymbolHistoryShared {
  bool operator==(SymbolHistoryShared const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, SymbolHistoryShared const &) {
  os << "SymbolHistoryShared";
  return os;
}

struct SymbolHistoryGrowth {
  bool operator==(SymbolHistoryGrowth const &other) const { return true; }
};
std::ostream &operator<<(std::ostream &os, SymbolHistoryGrowth const &) {
  os << "SymbolHistoryGrowth";
  return os;
}

using Call = std::variant<
    Branch,
    BranchMedallion,
    NewLine,
    WorkingDirectory,
    Cue,
    ResetColors,
    ForeColor,
    BackColor,
    Text,
    InlineDirSeparator,
    FinalDirSeparator,
    BranchOpen,
    BranchClose,
    SymbolModified,
    SymbolHistoryShared,
    SymbolHistoryGrowth>;

using CallVector = std::vector<Call>;

class Visitor {
public:
  CallVector calls;
  // TODO: refactor all the emplace_back
  void branch(Git::Status const &status) { calls.emplace_back(Branch{status}); }
  void branchStatus(Git::Status const &status) { calls.emplace_back(BranchMedallion{status}); }
  void newLine() { calls.emplace_back(NewLine()); }
  void workingDirectory(std::filesystem::path const &wd) { calls.emplace_back(WorkingDirectory{wd}); }
  void cue() { calls.emplace_back(Cue()); }
  void resetColors() { calls.emplace_back(ResetColors{}); }
  void foreColor(Color const &c) { calls.emplace_back(ForeColor{c}); }
  void backColor(Color const &c) { calls.emplace_back(BackColor{c}); }
  void text(std::string const t) { calls.emplace_back(Text{t}); }
  void inlineDirSeparator() { calls.emplace_back(InlineDirSeparator{}); }
  void finalDirSeparator() { calls.emplace_back(FinalDirSeparator{}); }
  void branchOpen() { calls.emplace_back(BranchOpen()); }
  void branchClose() { calls.emplace_back(BranchClose()); }
  void symbolModified() { calls.emplace_back(SymbolModified()); }
  void symbolHistoryShared() { calls.emplace_back(SymbolHistoryShared()); }
  void symbolHistoryGrowth() { calls.emplace_back(SymbolHistoryGrowth()); }
};

bool checkCalls(CallVector const &calls, CallVector const &expected) {
  if (expected.empty() && !calls.empty()) {
    UNSCOPED_INFO("no calls were expected");
    return false;
  }

  for (std::size_t i = 0; i < calls.size() && i < expected.size(); ++i) {
    if (!(calls[i] == expected[i])) {
      UNSCOPED_INFO("first difference at index " << i << ": expected \"" << streamer{expected[i]} << "\" but got \"" << streamer{calls[i]} << '\"');
      return false;
    }
  }

  if (calls.size() < expected.size()) {
    UNSCOPED_INFO("too short, expected to continue at index " << calls.size() << " with \"" << streamer{expected[calls.size()]} << "\"");
    return false;
  }

  if (calls.size() > expected.size()) {
    UNSCOPED_INFO("too long, expected to stop at index " << expected.size() - 1 << " with \"" << streamer{expected.back()} << "\"");
    return false;
  }

  return true;
}

TEST_CASE("branch banner") {

  Visitor visitor;

  Git::Status const status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0};

  getBranchBanner(status, visitor);

  CHECK(checkCalls(visitor.calls, CallVector{
                                      ForeColor{Colors::branch},
                                      BranchOpen{},
                                      ForeColor{Colors::bright},
                                      BackColor{Colors::branch},
                                      Text{" "},
                                      Text{"GSD-2808_filter"},
                                      Text{" "},
                                      BranchMedallion{status},
                                      Text{" "},
                                      ResetColors{},
                                      ForeColor{Colors::branch},
                                      BranchClose{},
                                      ResetColors{},
                                  }));
}

TEST_CASE("branch medallion") {
  Visitor visitor;

  SECTION("clean") {
    Git::Status const status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0};

    getBranchStatusMedallion(status, visitor);

    CHECK(visitor.calls.empty());
  }

  SECTION("locally modified") {
    Git::Status const status{"trunk", Git::WorkingDirectoryStatus::Modified, Git::UpstreamStatus::Set, 0, 0};

    getBranchStatusMedallion(status, visitor);

    CHECK(checkCalls(visitor.calls, CallVector{
                                        ForeColor{Colors::medallion},
                                        BranchOpen{},
                                        ForeColor{Colors::bright},
                                        BackColor{Colors::medallion},
                                        Text{" "},
                                        SymbolModified(),
                                        Text{" "},
                                        ForeColor{Colors::medallion},
                                        BackColor{Colors::branch},
                                        BranchClose{},
                                        ForeColor{Colors::bright},
                                    }));
  }

  SECTION("without upstream") {
    Git::Status const status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Unset, 0, 0};

    getBranchStatusMedallion(status, visitor);

    CHECK(checkCalls(visitor.calls, CallVector{
                                        ForeColor{Colors::medallion},
                                        BranchOpen{},
                                        ForeColor{Colors::bright},
                                        BackColor{Colors::medallion},
                                        Text{" "},
                                        Text{"no upstream"},
                                        Text{" "},
                                        ForeColor{Colors::medallion},
                                        BackColor{Colors::branch},
                                        BranchClose{},
                                        ForeColor{Colors::bright},
                                    }));
  }

  // three colors: "sharedHistory", "localHistoryGrowth" and "remoteHistoryGrowth"
  // two symbols: "sharedHistory" and "historyGrowth"
  // 0/0   sharedHistory ___   sharedHistory ___
  // 1/0     localHistoryGrowth ---   sharedHistory ___
  // 0/1   sharedHistory ___    remoteHistoryGrowth ---
  // 1/1     localHistoryGrowth ___    remoteHistoryGrowth ---

  SECTION("ahead") {
    Git::Status const status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 1, 0};

    getBranchStatusMedallion(status, visitor);

    CHECK(checkCalls(visitor.calls, CallVector{
                                        ForeColor{Colors::medallion},
                                        BranchOpen{},
                                        ForeColor{Colors::bright},
                                        BackColor{Colors::medallion},
                                        Text{" "},
                                        ForeColor{Colors::historyGrowthLocal},
                                        SymbolHistoryGrowth{},
                                        Text{" "},
                                        ForeColor{Colors::historyShared},
                                        SymbolHistoryShared{},
                                        ForeColor{Colors::bright},
                                        Text{" "},
                                        ForeColor{Colors::medallion},
                                        BackColor{Colors::branch},
                                        BranchClose{},
                                        ForeColor{Colors::bright},
                                    }));
  }
}

TEST_CASE("working directory banner") {

  Visitor visitor;

  getWorkingDirectoryBanner("/home/phil", visitor);

  CHECK(checkCalls(visitor.calls, CallVector{
                                      ForeColor{Colors::bright},
                                      BackColor{Colors::wd},
                                      Text{" "},
                                      Text{"/"},
                                      Text{" "},
                                      InlineDirSeparator{},
                                      Text{" "},
                                      Text{"home"},
                                      Text{" "},
                                      InlineDirSeparator{},
                                      Text{" "},
                                      Text{"phil"},
                                      Text{" "},
                                      ResetColors{},
                                      ForeColor{Colors::wd},
                                      FinalDirSeparator{},
                                      ResetColors{},
                                  }));
}

TEST_CASE("prompt") {

  Visitor visitor;

  Git::Status gitStatus;
  gitStatus.branchName = "GSD-2020_fix";
  gitStatus.workingDirectoryStatus = Git::WorkingDirectoryStatus::Clean;
  gitStatus.upstreamStatus = Git::UpstreamStatus::Set;
  gitStatus.nbCommitsAhead = 0;
  gitStatus.nbCommitsBehind = 0;

  getPrompt(gitStatus, "/home/phil", visitor);

  CHECK(checkCalls(visitor.calls, CallVector{Branch{gitStatus}, NewLine(), WorkingDirectory{"/home/phil"}, Cue()}));
}
