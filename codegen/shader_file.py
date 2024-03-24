from typing import cast, Any, TypeVar, Iterable
from pathlib import Path
import textwrap, json, itertools
import jsonschema.validators


SCRIPT_POS = Path(__file__).parent

SHADER_HPP_TMPL = '''
// This file is generated, do not modify.

#ifndef __STATIC_SHADERS_HPP__
#define __STATIC_SHADERS_HPP__

#include "ogl_deps.hpp"

namespace shaders {{

namespace utils {{

static inline gl::ShaderSource make_source(const std::string& name, const std::string& text) {{
    gl::ShaderSource source;
    source.set_source(text);
    source.set_source_file(name);
    return source;
}}

template <typename... Shaders>
static inline gl::Program make_program(Shaders&&... shaders) {{
    gl::Program prog;
    (..., prog.attachShader(shaders));
    prog.link();
    return prog;
}}

template <typename... OpTs> requires (... && std::is_invocable_v<OpTs, gl::Program&>)
static inline gl::Program&& init_program(gl::Program&& prog, OpTs... ops) {{
    (..., ops(prog));
    return std::move(prog);
}}

}} // namespace utils

namespace files {{
{file_functions}
}} // namespace files

namespace programs {{
{program_functions}
}} // namespace programs

}} // namespace shaders

#endif // __STATIC_SHADERS_HPP__
'''

SHADER_CPP_TMPL = '''
// This file is generated, do not modify.

#include "static_shaders.hpp"
#include "static_shader_inits.hpp"

using namespace std::string_literals;

{impls}
'''

SHADER_FILE_TMPL = '''
gl::ShaderSource& shaders::files::{func_name}() {{
    static gl::ShaderSource source = utils::make_source({name_literal}, {text_literal});
    return source;
}}
'''

SHADER_FILE_DECL_TMPL = '''gl::ShaderSource& {func_name}();'''

SHADER_PROGRAM_TMPL = '''
gl::Program& shaders::programs::{prog_name}() {{
    static gl::Program prog = utils::init_program(utils::make_program({shader_entries}){shader_inits});
    return prog;
}}
'''

SHADER_PROGRAM_DECL_TMPL = '''gl::Program& {prog_name}();'''

SHADER_ENTRY_TMPL = 'gl::Shader({type_enum}, files::{func_name}())'

SHADER_FILE_SUFFIX = '.shader.json'

TYPE_ENUM_MAP = {
    'frag': 'gl::kFragmentShader',
    'vert': 'gl::kVertexShader',
    'geom': 'gl::kGeometryShader',
}

SHADER_INFO_VALIDATOR = jsonschema.Draft7Validator(
    json.loads((SCRIPT_POS / 'shader.schema.json').read_text(encoding='utf-8'))
)

T1 = TypeVar('T1'); T2 = TypeVar('T2')
def unzip2(it: Iterable[tuple[T1, T2]]) -> tuple[list[T1], list[T2]]:
    l1 = list[T1](); l2 = list[T2]()
    for e1, e2 in it:
        l1.append(e1); l2.append(e2)
    return l1, l2

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
        func_name = file_path.name.replace('.', '_')
        file_func_decl = SHADER_FILE_DECL_TMPL.format(func_name = func_name)
        func_ns = '::'.join(file_relpath.parent.parts)
        if func_ns:
            file_func_decl = '\n' + wrap_namespace(func_ns, file_func_decl) + '\n'
            func_name = func_ns + '::' + func_name
        file_func = SHADER_FILE_TMPL.format(
            func_name = func_name,
            name_literal = make_stdstr_literal('/'.join(file_relpath.parts)),
            text_literal = make_stdstr_literal(file_path.read_text(encoding='utf-8')),
        )
        return file_func, file_func_decl
    
    def dump_program_code(self, program_path: Path):
        program_info: dict[str, Any] = json.loads(program_path.read_text(encoding='utf-8'))
        validate_shader_info(program_info)
        prog_name = program_info['name']
        
        shader_entries = list[str]()
        for file_relpath_s in cast(list[str], program_info["files"]):
            file_relpath = Path(file_relpath_s)
            func_name = file_relpath.name.replace('.', '_')
            func_ns = '::'.join(file_relpath.parent.parts)
            if func_ns: func_name = func_ns + '::' + func_name
            shader_entries.append(SHADER_ENTRY_TMPL.format(
                type_enum = TYPE_ENUM_MAP[file_relpath.suffix[1:]],
                func_name = func_name,
            ))
            self.files.add((program_path.parent / file_relpath).absolute())
        
        init_names = program_info.get("inits", [])
        if init_names == 'default':
            init_names = [f'{prog_name}_init']
        init_names = cast(list[str], init_names)

        return SHADER_PROGRAM_TMPL.format(
            prog_name = prog_name,
            shader_entries = ', '.join(shader_entries),
            shader_inits = ''.join(f', ::shaders::details::{init_name}'
                                   for init_name in init_names)
        ), SHADER_PROGRAM_DECL_TMPL.format(prog_name = prog_name)

    def find_programs(self):
        for child in self.root_dir.iterdir():
            if self.is_prog_file(child): yield child

    def run(self):
        prog_codes, prog_decls = unzip2(map(self.dump_program_code, self.find_programs()))
        glsl_codes, glsl_decls = unzip2(map(self.dump_file_code, self.files))
        return SHADER_HPP_TMPL.format(
            file_functions = '\n'.join(glsl_decls),
            program_functions = '\n'.join(prog_decls),
        ), SHADER_CPP_TMPL.format(
            impls = '\n'.join(itertools.chain(prog_codes, glsl_codes))
        ) 


def run_codegen(src: Path, dst: Path):
    assert src.is_dir() and dst.is_dir()
    ss_hpp, ss_cpp = ShaderPool(src).run()
    (dst / 'static_shaders.hpp').write_text(ss_hpp, encoding='utf-8')
    (dst / 'static_shaders.cpp').write_text(ss_cpp, encoding='utf-8')
    (dst / 'static_shader_inits.hpp').touch()

if __name__ == '__main__':
    import click
    
    @click.command()
    @click.argument('src')
    @click.argument('dst')
    def main(src: str, dst: str):
        run_codegen(Path(src), Path(dst))
    
    main()
