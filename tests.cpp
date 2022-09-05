#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <sstream>

#include "code.cpp"

namespace Status {

char const * const clean =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -0
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const * const locallyModified =
R"(# branch.oid d67d49339dfd71e38a3137d6c88c3f2cbeed3919
# branch.head trunk
# branch.upstream origin/trunk
# branch.ab +0 -0
1 AM N... 000000 100644 100644 0000000000000000000000000000000000000000 e69de29bb2d1d6434b8b29ae775ad8c2e48c5391 status_examples/branch_equal_to_origin
1 AM N... 000000 100644 100644 0000000000000000000000000000000000000000 e69de29bb2d1d6434b8b29ae775ad8c2e48c5391 status_examples/branch_without_upstream
? .clang-format
? cmake-build-release/
)";

char const * const withoutUpstream =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const * const equalToOrigin =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -0
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const * const ahead =
R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +1 -0
)";

char const * const behind =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -1
? dummy-client/conanfile_windows.txt
? ref-data/datasets/samples/JPG/2022Apr12_11-48-21.log
)";

char const * const aheadAndBehind =
R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +2 -13
)";

}


TEST_CASE("git status") {

  auto call = [](char const * const gitStatusOutput) { std::stringstream is(gitStatusOutput); return Git::getStatus(is); };

  CHECK(call(Status::clean) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::locallyModified) == Git::Status{"trunk", Git::WorkingDirectoryStatus::Modified, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::withoutUpstream) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Unset, 0, 0});
  CHECK(call(Status::equalToOrigin) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::ahead) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 1, 0});
  CHECK(call(Status::behind) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 1});
  CHECK(call(Status::aheadAndBehind) == Git::Status{"GSD-2808_filter", Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 2, 13});
}

using StringVector = std::vector<std::string>;

class PromptVisitor {
public:
  void textColor(Color color);
  void resetColors() { calls.emplace_back("resetColors()"); };
  void text(std::string const & t);

  StringVector calls;
};

bool checkCalls(StringVector const & calls, StringVector const & expected) {
  if(expected.empty() && !calls.empty()) {
    UNSCOPED_INFO("no calls were expected");
    return false;
  }

  for(std::size_t i=0; i < calls.size() && i < expected.size(); ++i) {
    if(calls[i] != expected[i]) {
      UNSCOPED_INFO("first difference at index " << i << ": expected \"" << expected[i] << "\" but got \"" << calls[i] << '\"');
      return false;
    }
  }

  if(calls.size() < expected.size()) {
    UNSCOPED_INFO("too short, expected to continue at index " << calls.size() << " with \"" << expected[calls.size()] << "\"");
    return false;
  }

  if(calls.size() > expected.size()) {
    UNSCOPED_INFO("too long, expected to stop at index " << expected.size() - 1 << " with \"" << expected.back() << "\"");
    return false;
  }

  return true;
}

TEST_CASE("prompt") {
  PromptVisitor promptVisitor;

  Git::Status gitStatus;
  gitStatus.branchName = "GSD-2020_fix";
  gitStatus.workingDirectoryStatus = Git::WorkingDirectoryStatus::Clean;
  gitStatus.upstreamState = Git::UpstreamStatus::Set;
  gitStatus.nbCommitsAhead = 0;
  gitStatus.nbCommitsBehind = 0;

  getPrompt(gitStatus, "/home/phil", promptVisitor);


  CHECK(checkCalls(promptVisitor.calls, StringVector{
                                            "foreColor(gitBack)",
                                            "openBranch()",
                                            "foreColor(bright)",
                                            "backColor(gitBack)",
                                            "text( GSD-2020_fix )",
                                            "resetColors()",
                                            "foreColor(gitBack)",
                                            "closeBranch()",
                                            "resetColors()",
                                            "text(\n)",
                                            "foreColor(bright)",
                                            "backColor(wdBack)",
                                            "text( / > home > phil )",
                                            "resetColors()",
                                            "foreColor(wdBack)",
                                            "closeWd()"
                                            "text(\n$ )",
                                  }));

  "<fore gitBack><openBranch><fore bright><back gitBack> GSD-2020_fix <reset><fore gitBack><closeBranch><reset>\n"
  "<fore bright><back wdBack> / <chevron> home <chevron> phil <reset><fore wdBack><fullChevron><reset>\n"
  "$ "
}
