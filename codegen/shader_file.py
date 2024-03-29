from typing import cast, Any
from pathlib import Path
import json, jsonschema.validators
from jinja2 import Environment, FileSystemLoader, select_autoescape


SCRIPT_POS = Path(__file__).parent

JINJA_ENV = Environment(
    loader=FileSystemLoader(SCRIPT_POS / 'templates'),
    autoescape=select_autoescape(),
    trim_blocks=True, lstrip_blocks=True,
)


def make_stdstr_literal(s: str):
    return json.dumps(s, ensure_ascii=False) + 's'


class ShaderFile:
    TYPE_ENUM_MAP = {
        'frag': 'gl::kFragmentShader',
        'vert': 'gl::kVertexShader',
        'geom': 'gl::kGeometryShader',
    }

    _relpath: Path
    _content: str
    
    def __init__(self, root_dir: Path, relpath: Path) -> None:
        self._relpath = relpath
        self._content = (root_dir / relpath).read_text(encoding='utf-8')
    
    def __hash__(self):
        return hash(self._relpath)

    def __eq__(self, other: Any):
        if not isinstance(other, self.__class__):
            return False
        return other._relpath == self._relpath
    
    @property
    def namespaces(self):
        return self._relpath.parts[:-1]
    
    @property
    def name(self):
        return self._relpath.stem
    
    @property
    def name_with_spaces(self):
        return '::'.join(self._relpath.with_suffix('').parts)
    
    @property
    def name_literal(self):
        return make_stdstr_literal(self._relpath.name)
    
    @property
    def text_literal(self):
        return make_stdstr_literal(self._content)
    
    @property
    def type_enum(self):
        return self.TYPE_ENUM_MAP[self._relpath.suffix[1:]]


class ShaderProgram:
    SUFFIX = '.shader.json'
    VALIDATOR = jsonschema.Draft7Validator(
        json.loads((SCRIPT_POS / 'shader.schema.json').read_text(encoding='utf-8'))
    )

    name: str
    inits: list[str]
    files: list[ShaderFile]

    def __init__(self, root_dir: Path, shader_desc: Any) -> None:
        self.VALIDATOR.validate(shader_desc)
        self.name = shader_desc["name"]
        self.files = [ShaderFile(root_dir, Path(file_path_str))
                      for file_path_str in shader_desc["files"]]
        init_names = shader_desc.get("inits", [])
        if init_names == 'default':
            init_names = [f'{self.name}_init']
        self.inits = init_names

    @staticmethod
    def find_program_files(root_dir: Path):
        for child in root_dir.iterdir():
            if child.is_file() and child.name.endswith(ShaderProgram.SUFFIX):
                yield child


def gather_template_args(root_dir: str | Path):
    root_dir = Path(root_dir).absolute()
    files = set[ShaderFile]()
    programs = list[ShaderProgram]()

    for shader_desc_file in ShaderProgram.find_program_files(root_dir):
        shader_desc = json.loads(shader_desc_file.read_text(encoding='utf-8'))
        sp = ShaderProgram(root_dir, shader_desc)
        for f in sp.files: files.add(f)
        programs.append(sp)
    
    return {
        'files': files,
        'programs': programs,
    }


def run_codegen(src: Path, dst: Path):
    assert src.is_dir() and dst.is_dir()
    tmpl_args = gather_template_args(src)
    hpp = JINJA_ENV.get_template("static_shaders.hpp.jinja").render(**tmpl_args)
    cpp = JINJA_ENV.get_template("static_shaders.cpp.jinja").render(**tmpl_args)
    (dst / 'static_shaders.hpp').write_text(hpp, encoding='utf-8')
    (dst / 'static_shaders.cpp').write_text(cpp, encoding='utf-8')
    (dst / 'static_shader_inits.hpp').touch()


if __name__ == '__main__':
    import click
    
    @click.command()
    @click.argument('src')
    @click.argument('dst')
    def main(src: str, dst: str):
        run_codegen(Path(src), Path(dst))
    
    main()
