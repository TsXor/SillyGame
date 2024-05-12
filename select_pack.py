from typing import Callable
from pathlib import Path
import os, zipfile
import click

def filter_pack(folder: Path, filter: Callable[[Path], bool]):
    with zipfile.ZipFile(folder.with_suffix('.zip'), 'w') as zipf:
        for root, dirs, files in os.walk(folder):
            for file in files:
                file_path = Path(os.path.join(root, file))
                rel_path = file_path.relative_to(folder)
                if filter(rel_path):
                    zipf.write(file_path, rel_path)

def silly_rule(fpath: Path) -> bool:
    if fpath.stem.removeprefix('lib') == 'SillyFramework': return True
    if fpath.stem == 'SillyGame' and fpath.suffix != '.log': return True
    if fpath.parts[0] == 'res' and fpath.suffix in {'.jpg', '.png'}: return True
    return False


if __name__ == '__main__':
    @click.command()
    @click.argument('folder')
    def main(folder: str):
        filter_pack(Path(folder).absolute(), silly_rule)

    main()
