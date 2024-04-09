import sys, runpy
from datetime import datetime
from pathlib import Path

MODULE_DIR = Path(__file__).parent
LAUNCHER_GLOBAL_NAME = '__launcher_config__'
MODULE_NAME = 'silly_resgen'

if __name__ == '__main__':
    laucher_config = {}
    runpy.run_path(
        str(MODULE_DIR / MODULE_NAME),
        {LAUNCHER_GLOBAL_NAME: laucher_config},
        '__main__',
    )
