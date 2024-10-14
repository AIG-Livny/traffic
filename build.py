#!/usr/bin/python3

def tool_config() -> "mapyr.ToolConfig":
    tc = mapyr.ToolConfig()
    tc.MINIMUM_REQUIRED_VERSION = '0.4.4'
    tc.VERBOSITY = 'INFO'
    return tc

def config() -> list["mapyr.ProjectConfig"]:

    result = []

    # Debug
    debug = mapyr.ProjectConfig()
    debug.OUT_FILE  = "bin/traffic"
    debug.COMPILER  = "clang"
    #debug.CFLAGS    = ["-g","-O0"]
    debug.CFLAGS    = ["-gdwarf-4","-O0"]

    debug.INCLUDE_DIRS = ['include']

    debug.SUBPROJECTS = [
        'lib/shaderutils',
        'lib/mathc',
        'lib/geometry',
    ]

    debug.INCLUDE_DIRS = [
        'lib/c-vector',
    ]

    debug.DEFINES = [
        'CVECTOR_LOGARITHMIC_GROWTH',
    ]

    debug.PKG_SEARCH = [
        'glfw3',
        'glew',
    ]

    debug.LIBS = [
        'm'
    ]

    debug.VSCODE_CPPTOOLS_CONFIG = True
    debug.OVERRIDE_CFLAGS = True
    debug.GROUPS = ['DEBUG']

    result.append(debug)

    # Release
    release = debug.copy()
    release.GROUPS = ['RELEASE']
    release.CFLAGS    = ["-Ofast","-flto"]
    release.LINK_EXE_FLAGS = ["-flto"]

    result.append(release)

    return result

#-----------FOOTER-----------
# https://github.com/AIG-Livny/mapyr.git
if __name__ == "__main__":
    try:
        import mapyr
    except:
        import requests, os
        os.makedirs(f'{os.path.dirname(__file__)}/mapyr',exist_ok=True)
        with open(f'{os.path.dirname(__file__)}/mapyr/__init__.py','+w') as f:
            f.write(requests.get('https://raw.githubusercontent.com/AIG-Livny/mapyr/master/__init__.py').text)
        import mapyr
    mapyr.process()