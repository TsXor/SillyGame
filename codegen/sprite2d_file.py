from typing import cast, Any
from dataclasses import dataclass
from pathlib import Path
import json, jsonschema.validators
from jinja2 import Environment, FileSystemLoader, select_autoescape


SCRIPT_POS = Path(__file__).parent

JINJA_ENV = Environment(
    loader=FileSystemLoader(SCRIPT_POS / 'templates'),
    autoescape=select_autoescape(),
    trim_blocks=True, lstrip_blocks=True,
)


def make_cstr_literal(s: str):
    return json.dumps(s, ensure_ascii=False)


@dataclass
class Sprite2DEntry:
    name: str
    file_relpath: Path
    uv: tuple[int, int, int, int]
    
    @property
    def file_literal(self):
        return make_cstr_literal('/'.join(self.file_relpath.parts))
    
    @property
    def uv_literal(self):
        return '{' + ', '.join(str(i) for i in self.uv) + '}'


class Sprite2DFile:
    SUFFIX = '.sprite2d.json'
    VALIDATOR = jsonschema.Draft7Validator(
        json.loads((SCRIPT_POS / 'sprite2d.schema.json').read_text(encoding='utf-8'))
    )

    file_relpath: Path
    entries: list[Sprite2DEntry]

    def __init__(self, root_dir: Path, sprite_desc: Any) -> None:
        self.VALIDATOR.validate(sprite_desc)
        self.file_relpath = Path(sprite_desc['file'])
        assert (root_dir / self.file_relpath).is_file()
        self.entries = [Sprite2DEntry(entry['name'], self.file_relpath, entry['uv'])
                        for entry in sprite_desc['sprites']]

    @staticmethod
    def find_program_files(root_dir: Path):
        for child in root_dir.iterdir():
            if child.is_file() and child.name.endswith(Sprite2DFile.SUFFIX):
                yield child


def gather_template_args(root_dir: str | Path):
    root_dir = Path(root_dir).absolute()
    entries = list[Sprite2DEntry]()

    for sprite_desc_file in Sprite2DFile.find_program_files(root_dir):
        sprite_desc = json.loads(sprite_desc_file.read_text(encoding='utf-8'))
        sf = Sprite2DFile(root_dir, sprite_desc)
        entries.extend(sf.entries)
    
    return {
        'sprites': entries,
    }


def run_codegen(src: Path, dst: Path):
    assert src.is_dir() and dst.is_dir()
    tmpl_args = gather_template_args(src)
    hpp = JINJA_ENV.get_template("static_sprites.hpp.jinja").render(**tmpl_args)
    cpp = JINJA_ENV.get_template("static_sprites.cpp.jinja").render(**tmpl_args)
    (dst / 'static_sprites.hpp').write_text(hpp, encoding='utf-8')
    (dst / 'static_sprites.cpp').write_text(cpp, encoding='utf-8')


if __name__ == '__main__':
    import click
    
    @click.command()
    @click.argument('src')
    @click.argument('dst')
    def main(src: str, dst: str):
        run_codegen(Path(src), Path(dst))
    
    main()
