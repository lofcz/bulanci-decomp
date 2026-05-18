from enum import Enum

from pathlib import Path

class FunctionType(Enum):
    STATIC = 1,
    THIS = 2,
    GLOBAL = 3,
    CONSTRUCTOR = 4,
    UNKNOWN = 5

class Function:
    def __init__(self):
        self.type = FunctionType.UNKNOWN
        self.mangledName = ''
        self.namespace = ''
        self.name = ''
        self.start = 0
        self.end = 0
        self.size = 0
        self.hasVarArgs = False
        self.args = []
        self.returnType = 'void'

    def __repr__(self):
        return f"Function {self.name} in {self.namespace} from 0x{self.start:x} to 0x{self.end:x} [0x{self.size:x}] of {self.type} type. Args: {self.args}"

    @staticmethod
    def create(csv_record: str):
        func = Function()
        csv_values = csv_record.split(';')
        mangled_name = csv_values[0]
        func.mangledName = mangled_name
        full_name = csv_values[1]
        name_parts = full_name.split("::")
        func.namespace = "::".join(name_parts[0: len(name_parts)-1])
        func.name = name_parts[-1]
        func.start = int(csv_values[2], 16)
        func.size = int(csv_values[3], 16)
        func.end = func.start + func.size

        convention = csv_values[4]
        if csv_values[5] != '':
            func.hasVarArgs = True
        func.returnType = csv_values[6]

        for arg_index in range(7, len(csv_values)):
            func.args.append(csv_values[arg_index])

        if convention == '__thiscall':
            if func.name == func.namespace or func.name == f"~{func.namespace}":
                func.type = FunctionType.CONSTRUCTOR
            else:
                func.type = FunctionType.THIS
        elif convention == '__cdecl':
            if func.hasVarArgs:
                func.type = FunctionType.THIS
            else:
                func.type = FunctionType.STATIC
        elif convention == '_stdcall' or convention == '__stdcall':
            func.type = FunctionType.GLOBAL

        if (func.type is FunctionType.THIS or func.type is FunctionType.CONSTRUCTOR) and len(func.args) > 0:
            func.args.pop(0)

        return func

class DecompUnit:
    def __init__(self, progress_category: str, directory_name: str, import_filename: str):
        self.progress_category = progress_category
        self.directory_name = directory_name
        self.target_file_name = import_filename

        self.configPath = Path("config") / self.directory_name
        self.buildOrig = Path("build") / "orig" / self.directory_name
        self.buildSrc = Path("build") / "Src" / self.directory_name
        self.includePath = Path("include") / self.directory_name
        self.srcPath = Path("src") / self.directory_name
        self.filePath = Path("orig") / self.target_file_name
        from helpers import load_mappings, load_units, load_completed
        units_file = self.configPath / "units_listing.csv"
        mapping_file = self.configPath / "mapping.csv"
        completed_file = self.configPath / "completed.csv"
        self.units = load_units(str(units_file)) if units_file.exists() else {}
        self.mappings = load_mappings(str(mapping_file)) if mapping_file.exists() else {}
        self.completed = load_completed(str(completed_file)) if completed_file.exists() else set()
