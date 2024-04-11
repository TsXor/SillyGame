from .basics import *
from dataclasses import dataclass


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
        return make_initializer_list(self.uv)


class Sprite2DFile:
    SUFFIX = '.sprite2d'
    VALIDATOR = get_json_validator('sprite2d')

    file_relpath: Path
    entries: list[Sprite2DEntry]

    def __init__(self, root_dir: Path, sprite_desc: Any) -> None:
        self.VALIDATOR.validate(sprite_desc)
        self.file_relpath = Path(sprite_desc['file'])
        assert (root_dir / self.file_relpath).is_file()
        self.entries = [Sprite2DEntry(entry['name'], self.file_relpath, entry['uv'])
                        for entry in sprite_desc['sprites']]


@wrap_jinja_formatter
def generate_sprite2d(root_dir: Path) -> dict[str, dict[str, Any]]:
    root_dir = Path(root_dir).absolute()
    entries = list[Sprite2DEntry]()

    for _, sprite_desc in load_second_suffix(root_dir, Sprite2DFile.SUFFIX):
        sf = Sprite2DFile(root_dir, sprite_desc)
        entries.extend(sf.entries)
    
    args = {
        'sprites': entries,
    }

    return {
        'static_sprites.hpp': args,
        'static_sprites.cpp': args,
    }
