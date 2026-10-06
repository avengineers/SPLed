# Publishing Variant Reports to GitHub Pages — Design

**Date:** 2026-07-27
**Status:** Proposed

## Context

Every quality gate builds a full Sphinx report site per variant at
`build/<Variant>/test/Debug/reports/html/`. `test_reports` carries all five gate markers
(`gate_develop_pr`, `gate_develop_push`, `gate_develop_nightly`, `gate_release_pr`,
`gate_release`), so **every** gate produces these reports for **every** variant.

On-premise, the reports folders of all variants are deployed to an HTTP server, keeping the
latest state of each baseline (branch, PR, tag). On GitHub nothing equivalent exists. CI today
uploads the reports only inside `*.7z` build artifacts — present, but not browsable: a reviewer
must download and unpack an archive to see a coverage or test report.

This document specifies replacing that gap with a GitHub Pages site that mirrors the on-premise
model: one browsable, permanently-addressable report site per live baseline.

## Goals

- Browsable HTML reports for all five variants, per baseline, at a stable URL.
- Baselines are branches, pull requests and tags — matching on-premise practice.
- Latest state per baseline; superseded runs are overwritten, not accumulated.
- Dead baselines are removed automatically. Stale feature-branch and PR reports must not
  survive for months.
- A root dashboard listing all live baselines with their status, the equivalent of the
  on-premise index.
- **No generated content in the product repository.** SPLed stays source-only; the published site
  lives elsewhere and nothing in SPLed's history grows because of it.

## Non-goals

- **Historical archive.** Only the latest run per baseline is kept. Older runs remain available
  as ordinary workflow artifacts for their retention period.
- **Windows reports.** Linux only (see Decision 2). Windows results stay visible as PR checks
  via `mikepenz/action-junit-report`.
- **Fork pull requests.** Not published in v1 (see Security).
- **Coverage percentages in the dashboard.** Deferred (see Open Questions).

## Measurements

Taken 2026-07-27 from a local full build of all five variants.

| Quantity | Value |
| --- | --- |
| One variant's Sphinx site | 15–17 MB |
| — of which `_static/` | 10.2 MB, **byte-identical across all variants** (10,735,293 B in Disco, Spa and Sleep) |
| — of which `.doctrees/` | 1.6 MB, Sphinx build cache, never published |
| — of which `build/<Variant>/` | 2.8 MB, the variant-specific report content |
| One baseline, 5 variants, `.doctrees` stripped | **73 MB served** |
| 10 baselines | **726 MB served**, **6.6 MB in the Git pack** |

Git storage is a non-issue: identical `_static` blobs are shared across every variant and every
baseline, so an additional baseline costs roughly 250 KB in the pack. *Caveat:* the 10-baseline
figure was produced by perturbing each report page with a unique comment; real runs differ more,
so treat 250 KB/baseline as a floor. Even at 10× it is ~25 MB, so the reports repository stays
small enough to clone casually and cheap enough to push on every run.

**The binding constraint is the served site size, not Git.** Per GitHub's documented limits:

- Published sites may be **no larger than 1 GB**.
- Source repositories have a **recommended limit of 1 GB**.
- **Soft** bandwidth limit of 100 GB/month.
- **Soft** limit of **10 builds per hour**, which "does not apply if you build and publish your
  site with a custom GitHub Actions workflow" — i.e. it *does* apply to the branch source used
  here.

At 73 MB/baseline the 1 GB ceiling arrives at roughly 13 baselines. Retention is therefore load-
bearing, not housekeeping.

## Decisions

### 1. A dedicated reports repository with Pages enabled

Reports are published to **`avengineers/SPLed-reports`**, a repository containing nothing but the
generated site, with Pages enabled on `main` / root. SPLed itself receives no generated content
and no `gh-pages` branch.

GitHub Pages supports two publishing sources: "Deploy from a branch" and "GitHub Actions".
Neither is deprecated; the docs recommend the Actions source when you need a non-Jekyll build
process "or you do not want a dedicated branch to hold your compiled static files".

The Actions source cannot express this requirement. `actions/deploy-pages` **replaces the entire
site** on each deployment. The feature request to publish into a subdirectory while preserving
the rest ([actions/deploy-pages#50]) is closed unimplemented, and native per-PR Pages previews
are an internal-only alpha ([actions/deploy-pages#61]) with no ship date.

Because N baselines must coexist and each is updated independently, some store must remember the
other baselines. The candidates were:

| Option | Verdict |
| --- | --- |
| Dedicated `SPLed-reports` repository | **Chosen.** Keeps generated content out of the product repo; publish jobs need no write access to SPLed; the reports repo gets its own 1 GB and build-rate budget. Costs one deploy key. |
| `gh-pages` branch inside SPLed | Rejected. Technically fine — 6.6 MB at 10 baselines, so clone cost is a non-issue — but it puts generated artifacts in the product repository and requires granting `contents: write` there. |
| Composite site as an Actions artifact, nothing committed | Rejected: moves ~726 MB down **and** up on every publish at 10 baselines, worsening as baselines grow. |
| External host (Cloudflare Pages, Netlify) | Rejected: native previews are nicer, but it puts quality evidence outside GitHub and needs an external account and secret. |

The first two options are both viable and differ only in where the site's state lives.
**[Appendix A](#appendix-a-hosting-alternatives) documents them at equal depth**, including what
is identical between them and what switching would cost, so this decision can be revisited
without repeating the analysis.

The reports repository must be **public**, matching SPLed, so Pages is available without a paid
plan. Its "recommended limit of 1 GB" for source repositories is comfortable given the measured
6.6 MB pack size for 10 baselines.

**Consequence:** `.nojekyll` at the site root is mandatory. Sphinx emits `_static/` and
`_sources/`, and Jekyll silently drops underscore-prefixed directories. Without it every report
loses its CSS and JavaScript.

**Accepted risk:** the 10 builds/hour soft limit applies to branch-source publishing. The
concurrency group serialises publishes; if the limit is hit during a busy period, publication
lags but nothing breaks. The budget belongs to the reports repo alone, so it cannot affect
anything else.

**Cost:** two repositories to relate to one another. Mitigated by a README in the reports repo
pointing back at SPLed and the workflow that fills it, and a link from SPLed's README. All
workflows stay in SPLed — the reports repo holds content and Pages configuration only, never CI.

### 1a. Authentication: deploy key

`GITHUB_TOKEN` is scoped to the repository running the workflow and cannot push to another
repository. A **deploy key** is used instead: an SSH keypair whose public half is registered on
`SPLed-reports` as a deploy key **with write access**, and whose private half is stored in SPLed
as the secret `PAGES_DEPLOY_KEY`.

Chosen over the alternatives because it is scoped to exactly one repository, carries no user
identity, and never expires:

- A **fine-grained PAT** is tied to an individual account and expires — CI breaks when that
  person rotates the token or leaves.
- A **GitHub App** (via `actions/create-github-app-token`) is the most robust option and avoids
  both problems, at the cost of creating and installing an app. Worth switching to if the
  organisation already has an app-based convention.

The publish job configures the key with `webfactory/ssh-agent` (or an explicit
`GIT_SSH_COMMAND`) and clones over SSH. Swapping auth mechanisms touches only this one step.

### 2. Linux only, all five variants

~73 MB per baseline rather than ~146 MB. Linux is the cheaper runner, and Windows correctness is
still gated by its own required check. The published site represents Linux results, and the
dashboard says so explicitly so no reader mistakes it for the full matrix.

### 3. Own publishing tool, not a third-party action

[`rossjrw/pr-preview-action`] implements exactly this pattern on a `gh-pages` branch, which
confirms the approach is current practice rather than a legacy holdover. It is nonetheless a
partial fit: pull requests only — no branches, no tags, no cross-baseline dashboard, and no fork
support. Since branch and tag publishing must be built regardless, and that mechanism handles
PRs for free, adopting it would put **two writers on one branch**, with its documented
`clean-exclude` footgun and a genuine race between them.

The logic therefore lives in `tools/pages_publish.py`, with the workflow reduced to git plumbing
and tool invocations. This matches the "CI is a thin wrapper" principle in AGENTS.md, keeps the
fallible parts unit-testable, and avoids granting a third-party action write access to the repo.

Two ideas are borrowed from it: `auto`-mode semantics (deploy on opened/reopened/synchronize,
remove on closed) and a sticky PR comment carrying the preview link.

### 4. Ordinary commit history, no orphan force-push

At ~250 KB per baseline, history is cheap. A plain push to the reports repo's `main` is simpler
and safer than force-pushing a branch that two jobs might touch concurrently. If the pack ever
grows uncomfortable, a manual squash to a single commit resolves it in one step — and because the
repository holds nothing else, that squash discards nothing of value.

[actions/deploy-pages#50]: https://github.com/actions/deploy-pages/issues/50
[actions/deploy-pages#61]: https://github.com/actions/deploy-pages/pull/61
[`rossjrw/pr-preview-action`]: https://github.com/rossjrw/pr-preview-action

## Site layout

Served from `https://avengineers.github.io/SPLed-reports/`, which is the `main` branch root of the
`avengineers/SPLed-reports` repository:

```
/index.html                      generated dashboard
/.nojekyll                       asserted on every publish
/develop/
    index.html                   baseline landing page (variant chooser)
    manifest.json
    Disco/                       the Sphinx site, verbatim
    Spa/
    Sleep/
    Base/Dev/
    IDEA/Sloemada/
/release/1.2/
/pr/123/
/branch/feature-xyz/
/tag/v1.2.3/
```

Variant directories keep their natural nested paths, so a URL mirrors the build tree:
`/pr/123/Base/Dev/index.html`.

## Baseline identity

| Trigger | Baseline ID |
| --- | --- |
| push to `develop` | `develop` |
| push to `release/<x>` | `release/<x>` |
| pull request (same-repo) | `pr/<number>` |
| push of tag `v*` | `tag/<name>` |
| `workflow_dispatch` on any other branch | `branch/<slug>` |

`<slug>` replaces every character outside `[A-Za-z0-9._-]` with `-`. Path separators in branch
names are preserved as directories only for `release/*`; all other branch names are flattened.
Any candidate containing `..`, a leading `/`, or resolving outside the site root is **rejected
with an error**, not sanitised into something else.

Two trigger changes are required in `ci.yml`:

- Add `push: tags: ["v*"]` so tag baselines exist at all. Tags currently trigger nothing.
- Extend `determine-gate` to map tag refs to `gate_release`. Today a tag push would fall into
  the `else` branch and be treated as `gate_develop_push`.

## CI workflow changes

### `test-on-linux`

Add an artifact upload, `if: always()`:

```yaml
- name: Upload Report Sites
  if: always()
  uses: actions/upload-artifact@v4
  with:
      name: reports-html
      retention-days: 1
      path: |
          build/**/test/Debug/reports/html/**
          build/**/variant-junit.xml
      if-no-files-found: warn
```

`.doctrees` is excluded by the staging tool rather than by glob, so the exclusion is unit-tested
rather than depending on artifact-glob semantics.

### `publish-reports` (new)

```yaml
needs: [determine-gate, test-on-linux]
if: always() && (github.event_name != 'pull_request' ||
    github.event.pull_request.head.repo.full_name == github.repository)
permissions:
    contents: read # SPLed is never written
    pull-requests: write # sticky preview comment
concurrency:
    group: pages-publish
    cancel-in-progress: false
```

`if: always()` is deliberate — a red build is *more* interesting to browse, not less.

Steps:

1. Download the `reports-html` artifact.
2. Load `PAGES_DEPLOY_KEY` into an SSH agent.
3. `git clone --depth 1 git@github.com:avengineers/SPLed-reports.git` into a working directory.
   If the repository is empty, initialise `main` with `.nojekyll` and an empty dashboard.
4. `pages_publish.py stage` — discover variant sites, copy, strip `.doctrees`, write
   `manifest.json`.
5. `pages_publish.py dashboard` — regenerate the root `index.html`.
6. `pages_publish.py check-size --limit-mb 800`.
7. Commit and push, retrying up to three times; on rejection, re-clone and replay steps 4–6
   rather than rebasing a large binary tree.
8. Write the baseline URL and site size to the job summary; for pull requests, upsert a sticky
   comment with the link.

Least privilege is the reason this is a separate job: the deploy key — the only credential that
can write anything — stays off the job that compiles third-party code fetched by CMake
`FetchContent` and poks. No job in SPLed gains write access to SPLed.

**Care is needed when introducing a workflow-level `permissions:` block.** `ci.yml` has none
today, so jobs receive the repository's default token permissions. Declaring
`permissions: contents: read` at workflow level would silently strip what
`mikepenz/action-junit-report` needs to publish its check runs. If the block is added, both test
jobs must explicitly declare `checks: write` and `pull-requests: write`. Verify the existing
"Test Results (Windows)" and "Test Results (Linux)" checks still appear before merging.

### `pages-cleanup.yml` (new)

Triggered by `pull_request: types: [closed]`. Loads the deploy key, clones the reports repo,
removes `pr/<number>/`, regenerates the dashboard, pushes. Separate workflow so that closing a PR
does not trigger a full build. Shares the `pages-publish` concurrency group.

## `tools/pages_publish.py`

Pure logic, no network. Git operations stay in the workflow.

| Command | Behaviour |
| --- | --- |
| `stage` | Discovers `**/test/Debug/reports/html` under the artifact root, derives the variant path from the segments before `/test/`, copies each into `<site>/<baseline>/<variant>/`, deletes `.doctrees`, writes `manifest.json` and the baseline landing page. Replaces any existing baseline directory wholesale so removed files do not linger. |
| `dashboard` | Reads every `*/manifest.json` under the site root and regenerates `index.html`. Asserts `.nojekyll` exists, recreating it if absent. |
| `prune` | Given the site root and a live-refs JSON, computes and applies deletions, printing one line per deletion with its reason. |
| `check-size` | Fails non-zero if the site exceeds the limit, listing the largest baselines. Always prints the total. |

Discovery is path-based rather than assuming the artifact's directory root, because
`actions/upload-artifact` derives the archive root from the common ancestor of matched files —
which shifts if only some variants built.

### Manifest schema

```json
{
  "schema": 1,
  "baseline": "pr/123",
  "kind": "pr",
  "ref": "refs/pull/123/merge",
  "commit": "1d69ac9...",
  "gate_marker": "gate_develop_pr",
  "run_url": "https://github.com/avengineers/SPLed/actions/runs/...",
  "timestamp_utc": "2026-07-27T09:13:00Z",
  "os": "ubuntu-24.04",
  "status": "complete",
  "variants": [
    {"name": "Disco", "path": "Disco",
     "tests": {"total": 42, "failures": 0, "errors": 0, "skipped": 1}}
  ]
}
```

Test counts come from the `variant-junit.xml` files already produced by the build. `status` is
`incomplete` when fewer than five variant sites were found.

### Dashboard

Self-contained HTML with inline CSS and no external assets, so it also renders correctly from a
downloaded copy. One row per baseline: name, kind, short commit, UTC timestamp, gate
marker, per-variant pass/fail summary, link. Ordered `develop`, `release/*`, tags, PRs by number
descending, then branches. Footer states total site size, generation time, and that results are
Linux-only.

**Branch and tag names are HTML-escaped.** A branch named `<script>…` must not inject into the
dashboard; this has an explicit unit test.

## Retention

A nightly `prune-reports` job runs in SPLed, so it can read SPLed's refs and pull requests with
`GITHUB_TOKEN` while writing to the reports repo with the deploy key. It builds a live-refs
document from `git ls-remote --heads` and `gh pr list --state open --json number`, then applies:

| Baseline | Kept when |
| --- | --- |
| `develop` | Always. Never deleted. |
| `release/*` | The branch still exists |
| `pr/<n>` | The PR is open (closed PRs are also removed immediately by `pages-cleanup.yml`) |
| `branch/<slug>` | The branch still exists **and** the baseline was updated within 30 days |
| `tag/<name>` | Among the 10 newest tags by creation date |

Everything else is deleted. Each deletion is logged to the job summary with its reason — no
silent removal.

## Failure modes

| Situation | Behaviour |
| --- | --- |
| Selftests fail, some variants missing | Publish what exists, `status: incomplete`, dashboard flags the baseline. Never fails the publish job. |
| Zero variant sites found | Skip publishing, log clearly, exit 0. Nothing is overwritten with emptiness. |
| Reports repo empty (first run) | Initialise `main` with `.nojekyll` and an empty dashboard. |
| Deploy key missing, revoked or wrong | Publish fails loudly with an explicit "check `PAGES_DEPLOY_KEY`" message. It must never degrade into a silent skip — a quietly stale dashboard is worse than a red job. |
| Concurrent push rejected | Re-clone and replay, three attempts, then fail loudly. |
| Site over 800 MB | Publish job fails with the largest baselines listed — deliberately below the 1 GB hard limit so there is room to react. |
| `.nojekyll` deleted by accident | Recreated on every `dashboard` run. |

## Security

- **Fork pull requests are not published.** Fork workflows cannot access secrets, so the deploy
  key is unavailable to them by construction. Independently, every Pages site under
  `avengineers.github.io` shares one browser origin — hosting untrusted HTML there is not free.
  The secure `workflow_run` pattern is a possible follow-up.
- **No workflow in SPLed gains write access to SPLed.** The only write credential is the deploy
  key, and its reach is exactly one repository that contains no source code. A compromised
  publish job can deface the report site; it cannot touch the product.
- `PAGES_DEPLOY_KEY` is confined to the publish, cleanup and prune jobs. It is a repository
  secret, not an environment-less organisation secret, so its blast radius stays visible.
- Both repositories are public, so publishing reports discloses nothing new. **Before first
  enable, confirm the rendered reports contain no internal hostnames, credentials paths, or
  customer material** — `doc/` is rendered wholesale into every variant site.

## Testing

`test/tooling/test_pages_publish.py`, marked `unittests` plus all five gate markers. Fast, no
network, no git.

- Baseline ID derivation across all triggers, including odd branch names and rejection of
  `..`/absolute-path candidates.
- `stage` against a synthetic reports tree: correct layout, `.doctrees` removed, stale files
  from a previous run gone, `status: incomplete` when variants are missing.
- Manifest generation from synthetic `variant-junit.xml` files, including a malformed one.
- Dashboard rendering: expected rows, ordering, and HTML escaping of a hostile branch name.
- Prune-set computation from synthetic site contents and live refs; asserts `develop` survives
  every input.
- `check-size` above and below the limit.

The workflow YAML itself is not unit-tested; it is validated by the staged rollout.

## Rollout

Steps 1–3 are one-time manual setup requiring organisation admin rights; the rest is ordinary
pull-request work in SPLed.

1. Create the public repository `avengineers/SPLed-reports` with a README explaining that its
   contents are generated by SPLed's `publish-reports` workflow and must not be edited by hand.
   Commit `.nojekyll` and a placeholder dashboard to `main`.
2. Enable Pages on it: Settings → Pages → Source "Deploy from a branch" → `main` / root. Confirm
   the placeholder is reachable at `https://avengineers.github.io/SPLed-reports/` before going
   further — this validates the whole hosting assumption for the cost of one page load.
3. Generate an SSH keypair. Add the public half to `SPLed-reports` as a deploy key **with write
   access**; add the private half to SPLed as the secret `PAGES_DEPLOY_KEY`.
4. Add `tools/pages_publish.py` and its tests. Green locally and in CI before any workflow change.
5. Add `publish-reports` gated to `workflow_dispatch` only; verify the produced site by hand.
6. Enable for `develop` pushes, then for same-repo pull requests.
7. Add `pages-cleanup.yml` and the nightly `prune-reports` job.
8. Add the tag trigger and the `determine-gate` tag mapping.
9. Link the site from SPLed's README and AGENTS.md.

Each step is independently revertible; nothing before step 6 affects normal PR flow.

## Open questions and follow-ups

- **Coverage in the dashboard.** Requires parsing `coverage.rst` or gcovr output; deferred until
  the format is confirmed.
- **`.vscode/tasks.json` references `reports/coverage/index.html`, which does not exist in a
  local full build.** Either a separate coverage target produces it or the path is stale. If it
  is real, that output should be published too.
- **`_static` deduplication** would cut a baseline from 73 MB to ~28 MB, tripling the number of
  baselines that fit under 1 GB. It requires rewriting Sphinx asset paths, so it is deferred
  until the site actually approaches the limit.
- **Fork PR previews** via the `workflow_run` pattern, weighed against the shared-origin risk.
- **Windows reports**, if the Linux-only record ever proves insufficient.

## Appendix A: Hosting alternatives

Two hosting options are viable. They differ only in **where the site's state lives** — not in
what gets published, how it is generated, or how it is pruned. **Option 1 is the chosen design.**
Option 2 is documented at equal depth so the decision can be revisited later without redoing the
analysis, and so the recurring question "why not just use a `gh-pages` branch?" has a written
answer.

The two options fully rejected earlier — reassembling the whole site as an Actions artifact, and
an external host such as Cloudflare Pages or Netlify — are covered in the Decision 1 table and
not repeated here.

### Identical in both options

Everything that carries real implementation risk:

- `tools/pages_publish.py` in full, and every one of its unit tests. The tool operates on a local
  directory and has no notion of a remote.
- Site layout, baseline identity and slug rules, `manifest.json` schema, the dashboard and its
  HTML escaping, retention rules, the 800 MB size guard, and all failure-mode behaviour.
- The three-workflow structure: `publish-reports`, `pages-cleanup.yml`, nightly `prune-reports`.
- The `reports-html` artifact hop out of `test-on-linux`.
- `.nojekyll` at the site root. Both options publish through a branch source, so Jekyll would
  otherwise discard Sphinx's `_static/` and `_sources/` directories.
- The `pages-publish` concurrency group and the re-clone-and-replay push retry.
- Linux-only scope, and the exclusion of fork pull requests.

### Comparison

| Dimension | Option 1: dedicated repository *(chosen)* | Option 2: `gh-pages` branch in SPLed |
| --- | --- | --- |
| State lives in | `avengineers/SPLed-reports`, `main` / root | SPLed, `gh-pages` branch |
| Site URL | `avengineers.github.io/SPLed-reports/` | `avengineers.github.io/SPLed/` |
| Credential | Deploy key, secret `PAGES_DEPLOY_KEY` | `GITHUB_TOKEN`, no secret at all |
| Publish job permissions | `contents: read` on SPLed | `contents: write` on SPLed |
| Write access to product repo | **None** | Required |
| Generated content in SPLed | None | ~6.6 MB of packed history at 10 baselines |
| One-time setup | Create repo, enable Pages, generate and install keypair — needs org admin | Enable Pages, bootstrap orphan branch |
| Ongoing credential upkeep | Deploy keys do not expire; rotate only on compromise | None |
| Build-rate budget (10/hour, soft) | Belongs to the reports repo alone | Shared with any other Pages use of SPLed |
| Source-repo 1 GB recommendation | Applies to a repo holding only reports | Applies to the product repo |
| Fork PRs excluded by | Construction — forks cannot read secrets | An `if:` condition that must be written correctly |
| Repositories to understand | Two, related by convention and READMEs | One |

### Option 1 in detail

See Decision 1, Decision 1a, and rollout steps 1–3. In short: the product repository stays
source-only, no workflow in SPLed can write to SPLed, and the price is one deploy key plus a
second repository that readers must know about.

### Option 2 in detail

Pages is enabled on SPLed with source "Deploy from a branch" → `gh-pages` / root. The branch is
bootstrapped once as an orphan containing `.nojekyll` and a placeholder dashboard.

The publish job becomes:

```yaml
needs: [determine-gate, test-on-linux]
if: always() && (github.event_name != 'pull_request' ||
    github.event.pull_request.head.repo.full_name == github.repository)
permissions:
    contents: write
    pull-requests: write
concurrency:
    group: pages-publish
    cancel-in-progress: false
```

Its steps are the same as the chosen design except that the SSH-agent step disappears and the
clone becomes `actions/checkout` with `ref: gh-pages` (or a `GITHUB_TOKEN` clone into a working
directory). `pages-cleanup.yml` and `prune-reports` change identically. The prune job gets
marginally simpler, since reading SPLed's refs and writing the site both use one credential.

Measured cost, from the same experiment recorded under Measurements: 73 MB served per baseline,
726 MB served at 10 baselines, but only **6.6 MB in the Git pack** — Git shares the identical
10.2 MB `_static` blob set across every variant and every baseline. A `git clone` of SPLed would
therefore not become meaningfully more expensive, which is why this option was rejected on
separation-of-concerns grounds rather than on cost.

**Why it was not chosen:** it places generated artifacts in the product repository and requires
granting `contents: write` there to a job whose input is a build that fetches third-party code
via CMake `FetchContent` and poks. Option 1 removes that grant entirely.

**When to switch to it:** if the second repository proves confusing in practice, if creating one
is blocked by organisation policy, or if deploy-key management becomes an operational burden.

### Cost of switching

The design deliberately confines the choice to the credential setup and the clone/push target —
two steps per workflow. Switching in either direction means:

1. Re-point the clone and push target; add or remove the SSH-agent step.
2. Flip `permissions: contents:` between `read` and `write` in the three workflows.
3. Enable Pages on the other repository; bootstrap `gh-pages` or create `SPLed-reports`.
4. Add or revoke the deploy key and the `PAGES_DEPLOY_KEY` secret.
5. Update the site URL in the README, AGENTS.md and the sticky PR comment template.

`tools/pages_publish.py`, its tests, the site layout and the retention logic are untouched. The
change is roughly an hour's work plus the one-time repository administration.

This is a documentation-level alternative, not a runtime switch: the workflows commit to one
target rather than selecting between them by configuration. Making it configurable would add a
seam that is exercised by exactly one code path in practice, which is not worth the complexity.
