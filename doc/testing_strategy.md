# Testing Strategy

This document describes the testing strategy for the SPLED project, including
the quality gate markers used to control which tests run in different CI contexts.

## Two-Dimensional Marker Strategy

Tests use a **two-dimensional marker strategy** combining *type markers* (WHAT to test)
and *gate markers* (WHEN to test):

- **Type markers** describe what kind of test it is:
  `build_debug`, `build_release`, `unittests`, `reports`, `static_analysis`, `sca_options_file`, `sca_timeout`

- **Gate markers** describe when the test should run in CI:
  `gate_develop_pr`, `gate_develop_push`, `gate_develop_nightly`, `gate_release_pr`, `gate_release`

Each test method carries both a type marker and one or more gate markers. This allows
the CI pipeline to automatically select the right tests based on the build context, while
developers can still run tests by type for manual debugging.

## Quality Gate Definitions

| Gate | CI Context | Trigger | Purpose |
|------|-----------|---------|---------|
| `gate_develop_pr` | PR targeting develop | PR created/updated | Fast feedback, blocks merge |
| `gate_develop_push` | Develop branch push | Push to `develop` | Standard CI verification |
| `gate_develop_nightly` | Nightly scheduled build | Cron (midnight) on `develop` | Comprehensive analysis |
| `gate_release_pr` | PR targeting release | PR created/updated | Release qualification + fast feedback |
| `gate_release` | Release branch push | Push to `release/*` | Release qualification |

## Gate Marker Assignment Matrix

The following table shows which gate markers are assigned to each test across all variants.
Variants with a full test suite (Disco, Sleep, Spa, IDEA/Sloemada) share the same marker
assignments. The Base/Dev variant has a reduced test set as noted.

| Test | Type Marker | `gate_develop_pr` | `gate_develop_push` | `gate_develop_nightly` | `gate_release_pr` | `gate_release` |
|------|-------------|:-----------------:|:-------------------:|:---------------------:|:-----------------:|:--------------:|
| test_build (Debug) | `build_debug` | ✅ | ✅ | ✅ | ✅ | ✅ |
| test_build (Release) | `build_release` | | | | ✅ | ✅ |
| test_unittests | `unittests` | | | | | |
| test_reports | `reports` | ✅ | ✅ | ✅ | ✅ | ✅ |
| test_static_analysis | `static_analysis` | | | ✅ | | |
| test_sca_options_file | `sca_options_file` | | | | | |
| test_sca_check_pr_changes | *(none)* | ✅ | | | ✅ | |
| test_analysis_timeout | `sca_timeout` | | | | | |

> **Note:** The Base/Dev variant does not include `test_build`, `test_sca_options_file`,
> or `test_sca_check_pr_changes`. All other markers apply identically.

### Design Rationale

- **`gate_develop_pr`** runs the debug build, report generation, and PR-scoped SCA comparison
  to give fast feedback and block merges with regressions.
- **`gate_develop_push`** runs the debug build and report generation after merge,
  providing standard verification on the develop branch.
- **`gate_develop_nightly`** adds static analysis on top of the develop push gate,
  catching issues that are too expensive for every push.
- **`gate_release_pr`** mirrors gate_develop_pr but adds the release build,
  ensuring both build types work before merging to a release branch.
- **`gate_release`** mirrors gate_develop_push but adds the release build,
  providing release qualification on push to release branches.

## CI Pipeline Integration

In the Jenkins pipeline, the gate marker is **auto-detected** from the build context:

```text
NIGHTLY=true                → gate_develop_nightly
PR targeting release/*      → gate_release_pr
PR targeting develop        → gate_develop_pr
Push on release/* branch    → gate_release
Push on develop (default)   → gate_develop_push
```

The `MARKERS` Jenkins parameter allows manual override with any valid
[pytest marker expression](https://docs.pytest.org/en/stable/how-to/mark.html):

```powershell
# Run all nightly tests except the timeout test
.\build.ps1 -selftests -marker "gate_develop_nightly and not sca_timeout"

# Run only unit tests (by type, regardless of gate)
.\build.ps1 -selftests -marker "unittests"

# Combine gate and type
.\build.ps1 -selftests -marker "gate_develop_push and not reports"

# Filter by variant
.\build.ps1 -selftests -filter "Disco" -marker "gate_develop_pr"
```

## Adding New Tests

When adding a new test to a variant test file:

1. **Add a type marker** describing what the test does (e.g., `@pytest.mark.my_new_check`)
2. **Register the marker** in `pytest.ini` under `markers =`
3. **Add gate markers** to control when it runs — consult the matrix above for guidance:
   - Must it run on every PR? Add `@pytest.mark.gate_develop_pr` and `@pytest.mark.gate_release_pr`
   - Standard verification after merge? Add `@pytest.mark.gate_develop_push` and `@pytest.mark.gate_release`
   - Only needed in comprehensive nightly runs? Add `@pytest.mark.gate_develop_nightly`
   - Release-only (e.g., release build)? Add `@pytest.mark.gate_release_pr` and `@pytest.mark.gate_release`
4. **Update this document** with the new test row in the matrix
