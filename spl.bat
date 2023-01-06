@echo off

set this_dir=%~dp0

pushd %this_dir%

set PYTHONPATH=%this_dir%;%PYTHONPATH%
pipenv run python src/creator/creator.py %*

popd
