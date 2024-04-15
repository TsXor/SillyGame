from pathlib import Path

import click

@click.group()
def cli(): pass

@cli.command()
@click.argument('src')
@click.argument('dst')
@click.argument('glad_include')
def shader(src: str, dst: str, glad_include: str):
    from silly_resgen import generate_shader
    generate_shader(Path(src), Path(dst), glad_include)

@cli.command()
@click.argument('src')
@click.argument('dst')
def sprite2d(src: str, dst: str):
    from silly_resgen import generate_sprite2d
    generate_sprite2d(Path(src), Path(dst))

@cli.command()
@click.argument('src')
@click.argument('dst')
def map2d(src: str, dst: str):
    from silly_resgen import generate_map2d
    generate_map2d(Path(src), Path(dst))

cli()
