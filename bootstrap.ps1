$ErrorActionPreference = "Stop"

# TODO: remove this Chicken Egg workaround

New-Item -ItemType Directory -Path ".venv" -Force
pipenv install
pipenv run west update
