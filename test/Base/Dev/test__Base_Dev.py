from utils import ArtifactsCollection, ComponentReportsCollection, SplBuild


def test_reports():
    assert 0 == SplBuild(
        variant="Base/Dev",
        build_kit="test",
        expected_artifacts=ComponentReportsCollection(
            modules=[
                "examples/component_a"
            ]
        ),
    ).execute(target="all", strict=True, archive=True)
