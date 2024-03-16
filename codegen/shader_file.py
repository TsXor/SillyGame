from typing import cast, Any, TypedDict, NotRequired
from pathlib import Path
import textwrap, json, jsonschema
import jsonschema.validators


SHADER_FRAMEWORK_TMPL = '''
// This file is generated, do not modify.

#ifndef __GENERATED_SHADERS_DEF__
#define __GENERATED_SHADERS_DEF__

namespace shaders {{

using namespace std::string_literals;

namespace utils {{

static inline gl::ShaderSource builtin_source(const std::string& name, const std::string& text) {{
    gl::ShaderSource source;
    source.set_source(text);
    source.set_source_file(name);
    return source;
}}

template <typename... Shaders>
static inline gl::Program builtin_program(Shaders&&... shaders) {{
    gl::Program prog;
    (..., prog.attachShader(shaders));
    prog.link();
    return prog;
}}

}} // namespace utils

namespace files {{
{file_functions}
}} // namespace files

namespace programs {{
{program_functions}
}} // namespace programs

}} // namespace shaders

#endif // __GENERATED_SHADERS_DEF__
'''

SHADER_FILE_TMPL = '''
static inline gl::ShaderSource& {func_name}() {{
    static gl::ShaderSource source = utils::builtin_source({name_literal}, {text_literal});
    return source;
}}
'''

SHADER_PROGRAM_TMPL = '''
static inline gl::Program& {prog_name}() {{
    static gl::Program prog = utils::builtin_program({shader_entries});
    return prog;
}}
'''

SHADER_ENTRY_TMPL = 'gl::Shader({type_enum}, files::{func_name}())'

SHADER_FILE_SUFFIX = '.shader.json'

TYPE_ENUM_MAP = {
    'frag': 'gl::kFragmentShader',
    'vert': 'gl::kVertexShader',
    'geom': 'gl::kGeometryShader',
}

SHADER_INFO_VALIDATOR = jsonschema.Draft7Validator({
    'type': 'object',
    'properties': {
        'name': {'type': 'string'},
        'frag': {'type': 'string'},
        'vert': {'type': 'string'},
        'geom': {'type': 'string'}
    },
    'required': ['name']
})

class ShaderInfo(TypedDict):
    name: str
    vert: NotRequired[str]
    frag: NotRequired[str]
    geom: NotRequired[str]


def wrap_namespace(name: str, code: str):
    TMPL = textwrap.dedent('''
        namespace {name} {{
        {code}
        }} // namespace {name}
    ''')[1:-1]
    return TMPL.format(name=name, code=code)

def make_stdstr_literal(s: str):
    return json.dumps(s, ensure_ascii=False) + 's'

def validate_shader_info(info: object):
    SHADER_INFO_VALIDATOR.validate(info)


class ShaderPool:
    root_dir: Path
    files: set[Path]

    def __init__(self, root_dir: str | Path) -> None:
        self.root_dir = Path(root_dir).absolute()
        self.files = set()

    @staticmethod
    def is_prog_file(p: Path):
        return p.is_file() and p.name.endswith(SHADER_FILE_SUFFIX)
    
    def dump_file_code(self, file_path: Path):
        file_relpath = file_path.relative_to(self.root_dir)
        file_func = SHADER_FILE_TMPL.format(
            func_name = file_path.name.replace('.', '_'),
            name_literal = make_stdstr_literal('/'.join(file_relpath.parts)),
            text_literal = make_stdstr_literal(file_path.read_text(encoding='utf-8')),
        )
        func_ns = '::'.join(file_relpath.parent.parts)
        if func_ns: file_func = '\n' + wrap_namespace(func_ns, file_func) + '\n'
        return file_func
    
    def dump_program_code(self, program_path: Path):
        program_info = json.loads(program_path.read_text(encoding='utf-8'))
        validate_shader_info(program_info)
        program_info = cast(ShaderInfo, program_info)
        shader_entries = list[str]()
        for typ in ('frag', 'vert', 'geom'):
            if typ in program_info:
                file_relpath = Path(program_info[typ]) # type: ignore
                func_name = file_relpath.name.replace('.', '_')
                func_ns = '::'.join(file_relpath.parent.parts)
                if func_ns: func_name = func_ns + '::' + func_name
                shader_entries.append(SHADER_ENTRY_TMPL.format(
                    type_enum = TYPE_ENUM_MAP[typ],
                    func_name = func_name,
                ))
                self.files.add((program_path.parent / file_relpath).absolute())
        return SHADER_PROGRAM_TMPL.format(
            prog_name = program_info['name'],
            shader_entries = ', '.join(shader_entries),
        )

    def find_programs(self):
        for child in self.root_dir.iterdir():
            if self.is_prog_file(child): yield child

    def run(self):
        prog_codes = list[str]()
        for prog_file in self.find_programs():
            prog_codes.append(self.dump_program_code(prog_file))
        glsl_codes = list[str]()
        for glsl_file in self.files:
            glsl_codes.append(self.dump_file_code(glsl_file))
        return SHADER_FRAMEWORK_TMPL.format(
            file_functions = '\n'.join(glsl_codes),
            program_functions = '\n'.join(prog_codes),
        )


def run_codegen(src: Path, dst: Path):
    assert src.is_dir() and dst.is_dir()
    (dst / 'static_shaders.hpp').write_text(ShaderPool(src).run(), encoding='utf-8')


if __name__ == '__main__':
    import click
    
    @click.command()
    @click.argument('src')
    @click.argument('dst')
    def main(src: str, dst: str):
        run_codegen(Path(src), Path(dst))
    
    main()
