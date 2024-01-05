from utils import ArtifactsCollection, ComponentReportsCollection, SplBuild


def test_build():
    assert 0 == SplBuild(
        variant="CustA/Disco",
        build_kit="prod",
        expected_artifacts=ArtifactsCollection(
            artifacts=["spled.exe", "compile_commands.json"]
        ),
    ).execute(target="all", strict=True, archive=True)


def test_reports():
    assert 0 == SplBuild(
        variant="CustA/Disco",
        build_kit="test",
        expected_artifacts=ComponentReportsCollection(
            modules=[
                "keyboard_interface",
                "light_controller",
                "main_control_knob",
                "power_signal_processing",
            ]
        ),
    ).execute(target="reports", strict=True, archive=True)
