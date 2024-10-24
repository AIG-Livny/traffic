#!/usr/bin/python3

import copy

def tool_config() -> "mapyr.ToolConfig":
    tc = mapyr.ToolConfig()
    tc.MINIMUM_REQUIRED_VERSION = '0.4.5'
    tc.PRINT_SIZE = True
    return tc

def config() -> list["mapyr.ProjectConfig"]:

    result = []

    default = mapyr.ProjectConfig()
    default.OUT_FILE  = "libnuklear.a"
    default.COMPILER  = "clang"
    default.SRC_DIRS = ['.']
    default.INCLUDE_DIRS = [
        '/usr/include/c++/12',
        '/usr/include/x86_64-linux-gnu/c++/12',
        '.',
    ]
    default.EXPORT_DEFINES = [
        'NK_INCLUDE_FIXED_TYPES',
        'NK_INCLUDE_STANDARD_IO',
        'NK_INCLUDE_DEFAULT_ALLOCATOR',
        'NK_INCLUDE_VERTEX_BUFFER_OUTPUT',
        'NK_INCLUDE_FONT_BAKING',
        'NK_INCLUDE_DEFAULT_FONT',
    ]
    default.GROUPS = ['NUKLEAR']

    # Debug
    debug = copy.deepcopy(default)
    debug.CFLAGS = ["-g","-O0"]
    debug.GROUPS += ['DEBUG']
    debug.DEFINES += ['DEBUG']

    result.append(debug)

    # Profiler
    profiler = copy.deepcopy(default)
    profiler.CFLAGS = ["-gdwarf-4","-O0"]
    profiler.GROUPS += ['PROFILER']
    profiler.DEFINES += ['DEBUG']
    result.append(profiler)

    # Release
    release = copy.deepcopy(default)
    release.GROUPS += ['RELEASE']
    release.CFLAGS = ["-Ofast","-flto"]
    release.LINK_EXE_FLAGS = ["-flto"]

    result.append(release)

    return result

#-----------FOOTER-----------
# https://github.com/AIG-Livny/mapyr.git
try:
    import mapyr
except:
    import requests, os
    os.makedirs(f'{os.path.dirname(__file__)}/mapyr',exist_ok=True)
    with open(f'{os.path.dirname(__file__)}/mapyr/__init__.py','+w') as f:
        f.write(requests.get('https://raw.githubusercontent.com/AIG-Livny/mapyr/master/__init__.py').text)
    import mapyr

if __name__ == "__main__":
    mapyr.process(config, tool_config)