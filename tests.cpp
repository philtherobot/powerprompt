#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "code.cpp"

namespace Status {

char const * const clean =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter_invalid_packets_based_on_date
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
# branch.head GSD-2808_filter_invalid_packets_based_on_date
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const * const equalToOrigin =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter_invalid_packets_based_on_date
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -0
? ../../../../dummy-client/conanfile_windows.txt
? ../JPG/2022Apr12_11-48-21.log
)";

char const * const ahead =
R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter_invalid_packets_based_on_date
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +1 -0
)";

char const * const behind =
R"(# branch.oid b6537cc298777bf35ca3d64ab519d1ad98a5ec45
# branch.head GSD-2808_filter_invalid_packets_based_on_date
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +0 -1
? dummy-client/conanfile_windows.txt
? ref-data/datasets/samples/JPG/2022Apr12_11-48-21.log
)";

char const * const aheadAndBehind =
R"(# branch.oid f27c9a2d1527f1bbc16c97132c92b893cfac71fe
# branch.head GSD-2808_filter_invalid_packets_based_on_date
# branch.upstream origin/GSD-2808_filter_invalid_packets_based_on_date
# branch.ab +2 -13
)";

}


TEST_CASE("git status") {

  auto call = [](char const * const gitStatusOutput) { std::stringstream is(gitStatusOutput); return Git::getStatus(is); };

  CHECK(call(Status::clean) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::locallyModified) == Git::Status{Git::WorkingDirectoryStatus::Modified, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::withoutUpstream) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Unset, 0, 0});
  CHECK(call(Status::equalToOrigin) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 0});
  CHECK(call(Status::ahead) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 1, 0});
  CHECK(call(Status::behind) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 0, 1});
  CHECK(call(Status::aheadAndBehind) == Git::Status{Git::WorkingDirectoryStatus::Clean, Git::UpstreamStatus::Set, 2, 13});
}
