from .basics import *


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
    SUFFIX = '.shader'
    VALIDATOR = get_json_validator('shader')

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


@wrap_jinja_formatter
def generate_shader(root_dir: Path, glad_include: str) -> dict[str, dict[str, Any]]:
    root_dir = Path(root_dir).absolute()
    files = set[ShaderFile]()
    programs = list[ShaderProgram]()

    for _, shader_desc in load_second_suffix(root_dir, ShaderProgram.SUFFIX):
        sp = ShaderProgram(root_dir, shader_desc)
        for f in sp.files: files.add(f)
        programs.append(sp)

    args = {
        'files': files,
        'programs': programs,
        'glad_include': glad_include,
    }

    return {
        'static_shaders.hpp': args,
        'static_shaders.cpp': args,
    }
