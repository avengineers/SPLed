from utils import ArtifactsCollection, SplBuild


def test_reports():
    assert 0 == SplBuild(
        variant="Base/Dev",
        build_kit="test",
        expected_artifacts=ArtifactsCollection(artifacts=["reports"]),
    ).execute(target="reports", strict=True, archive=True)
