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
    default.OUT_FILE  = "bin/traffic"
    default.COMPILER  = "clang"
    default.INCLUDE_DIRS = [
        'include',
    ]

    default.SUBPROJECTS = [
        'lib/shaderutils',
        'lib/cgeom',
        'lib/nuklear',
    ]

    default.INCLUDE_DIRS = [
        'lib/c-vector',
    ]

    default.DEFINES = [
        'CVECTOR_LOGARITHMIC_GROWTH',
    ]

    default.PKG_SEARCH = [
        'glfw3',
        'glew',
    ]

    default.LIBS = [
        'm',
    ]

    default.VSCODE_CPPTOOLS_CONFIG = True
    default.OVERRIDE_CFLAGS = True

    # Debug
    debug = copy.deepcopy(default)
    debug.CFLAGS    = ["-g","-Ofast"]
    debug.GROUPS = ['DEBUG']
    debug.DEFINES += ['DEBUG']

    result.append(debug)

    # Profiler
    #profiler = copy.deepcopy(default)
    #profiler.CFLAGS    = ["-gdwarf-4","-O0"]
    #profiler.GROUPS = ['PROFILER']
    #profiler.DEFINES += ['DEBUG']
    #result.append(profiler)

    # Release
    release = copy.deepcopy(default)
    release.GROUPS = ['RELEASE']
    release.CFLAGS    = ["-Ofast","-flto"]
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