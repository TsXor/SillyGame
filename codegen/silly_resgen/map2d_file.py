from .basics import *
from dataclasses import dataclass


@dataclass
class Map2DBlock:
    uv: tuple[int, int, int, int]
    at: tuple[int, int]

    @property
    def literal(self):
        return make_initializer_list((self.uv, self.at))


class Map2DFile:
    SUFFIX = '.map2d'
    VALIDATOR = get_json_validator('map2d')

    file_relpath: Path
    name: str
    blocks: list[Map2DBlock]

    def __init__(self, root_dir: Path, map_desc: Any) -> None:
        self.VALIDATOR.validate(map_desc)
        self.file_relpath = Path(map_desc['file'])
        assert (root_dir / self.file_relpath).is_file()
        self.name = map_desc['name']
        self.blocks = [Map2DBlock(entry['uv'], entry['at'])
                       for entry in map_desc['blocks']]

    @property
    def file_literal(self):
        return make_cstr_literal('/'.join(self.file_relpath.parts))


@wrap_jinja_formatter
def generate_map2d(root_dir: Path) -> dict[str, dict[str, Any]]:
    root_dir = Path(root_dir).absolute()
    entries = list[Map2DFile]()

    for _, map_desc in load_second_suffix(root_dir, Map2DFile.SUFFIX):
        entries.append(Map2DFile(root_dir, map_desc))
    
    args = {
        'maps': entries,
    }

    return {
        'static_maps.hpp': args,
        'static_maps.cpp': args,
    }
