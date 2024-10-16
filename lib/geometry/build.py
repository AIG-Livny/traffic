#!/usr/bin/python3
REQUIRED_VERSION = '0.4.2'

def config() -> list["mapyr.ProjectConfig"]:
    
    result = []
    p = mapyr.ProjectConfig()

    p.OUT_FILE  = "libgeometry.a"
    p.COMPILER  = "clang"
    p.CFLAGS    = ["-g","-O0"]
    p.SRC_DIRS  = ["."]
    #p.CFLAGS    = ["-Ofast","-flto"]
    #p.LINK_EXE_FLAGS = ["-flto"]

    p.INCLUDE_DIRS = [
        '.',
        '../c-vector',
    ]

    p.DEFINES = [
        'CVECTOR_LOGARITHMIC_GROWTH',
    ]

    result.append(p)
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
    mapyr.process(config(), REQUIRED_VERSION)