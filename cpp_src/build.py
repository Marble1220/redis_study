import os

from sys import argv

DEBUG = 0

def scandir(path: str, chose: str = ''):
    res = []
    filter_suffix = [chose + '.c', chose + ".cpp"]
    for f in os.listdir(path):

        if (f.endswith(filter_suffix[0]) or f.endswith(filter_suffix[1])) and ("test" not in f):
            res.append(os.path.join(os.path.abspath(path), f))
    return res


def clean(dirpath: str='.'):
    for f in os.listdir(dirpath):
        if f.endswith(".o") or f == "test":
            os.remove(f)
            print("remove {}".format(f))


def compile_one(filename: str):
    compile_s = ["g++", "-fPIC", "-c", filename, "-o", filename.replace(".cpp", ".o")]
    if DEBUG:
        compile_s.insert(1, '-g')
    compile_s = " ".join(compile_s)
    print(compile_s)
    if os.path.exists(filename.replace(".cpp", ".o")) and \
    os.stat(filename).st_mtime < os.stat(filename.replace(".cpp", ".o")).st_mtime:
        pass
    else:
        if os.system(compile_s) != 0:
            raise Exception("compile error: {}".format(filename))
    return filename.replace(".cpp", ".o")



def compile(source_files):
    compile_s = ["g++"]
    if DEBUG:
        compile_s.append('-g')
    for f in source_files:
        compile_s.append(compile_one(f))

    compile_s.append("-o")
    compile_s.append("test")
    if os.system(" ".join(compile_s)) != 0:
        raise Exception("compile error {}".format(" ".join(compile_s)))
    print(" ".join(compile_s))




def build(cmd: list):
    file_lst = []

    for once in cmd:
    
        if once == 'all':
            all_lst = ['base', 'sds', 'intset', 'object', 'StringObject', 'dict', 'adlist', 'skiplist', 'ListObject', 'SetObject', 'HashObject', 'ZsetObject', 'db', 'pydis']
            for i in all_lst:
                file_lst += scandir(".", i)
            for i in file_lst:
                compile_one(i)
            dll_compile = ["g++", "-shared"]
            dll_compile += list(map(lambda o: o+".o", all_lst))
            dll_compile += ["-o", "pydis.so"]
            os.system(' '.join(dll_compile))
            return 
            
        elif once == 'clean':
            clean()
        else:
            file_lst += scandir(".", once)
    if file_lst:
        file_lst.append(os.path.join(os.path.split(file_lst[-1])[0], "test" + argv[-1] + ".cpp"))
        compile(file_lst)




if __name__ == "__main__":

    if len(argv) >= 2:
        if argv[1] != 'debug':
            build(argv[1:])
        else:
            DEBUG = 1
            build(argv[2:])
    else:
        raise Exception("paramter error")