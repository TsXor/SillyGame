from typing import Callable, Iterable, Any
from abc import abstractmethod
from pathlib import Path
import os
import json, yaml, jsonschema.validators
from jinja2 import Environment, FileSystemLoader, select_autoescape

MODULE_POS = Path(__file__).parent

JINJA_ENV = Environment(
    loader=FileSystemLoader(MODULE_POS / 'templates'),
    autoescape=select_autoescape(),
    trim_blocks=True, lstrip_blocks=True,
)

def make_cstr_literal(s: str):
    return json.dumps(s, ensure_ascii=False)

def make_stdstr_literal(s: str):
    return make_stdstr_literal(s) + 's'

def make_initializer_list(obj: Any):
    if isinstance(obj, str):
        return obj
    elif isinstance(obj, Iterable):
        return '{' + ', '.join(make_initializer_list(e) for e in obj) + '}'
    else:
        return str(obj)

def get_json_validator(name: str):
    schema_text = (MODULE_POS / 'schemas' / f'{name}.schema.json').read_text(encoding='utf-8')
    return jsonschema.Draft7Validator(json.loads(schema_text))

def read_config(cfg_path: Path) -> Any:
    if cfg_path.suffix == '.json':
        cfg_text = cfg_path.read_text(encoding='utf-8')
        return json.loads(cfg_text)
    elif cfg_path.suffix in {'.yaml', '.yml'}:
        cfg_text = cfg_path.read_text(encoding='utf-8')
        return yaml.load(cfg_text, Loader=yaml.Loader)

def load_second_suffix(root_dir: Path, suffix: str) -> Iterable[tuple[Path, Any]]:
    for root, dirs, files in os.walk(root_dir):
        for child in map(lambda file: Path(root) / file, files):
            if child.stem.endswith(suffix):
                yield child, read_config(child)

def wrap_jinja_formatter(gather_template_args_fn: Callable[[Path], dict[str, dict[str, Any]]]):
    def jinja_format_wrapper(src: Path, dst: Path):
        assert src.is_dir() and dst.is_dir()
        for file_name, tmpl_args in gather_template_args_fn(src).items():
            result = JINJA_ENV.get_template(f"{file_name}.jinja").render(**tmpl_args)
            (dst / file_name).write_text(result, encoding='utf-8')
    return jinja_format_wrapper
