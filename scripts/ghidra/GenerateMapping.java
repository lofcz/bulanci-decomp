// Generate mapping.csv
//@category bulanci

import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.DataType;
import ghidra.program.model.data.Pointer;
import ghidra.program.model.data.TypeDef;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolType;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Parameter;
import java.io.File;
import java.nio.file.Files;

import java.util.regex.Pattern;

public class GenerateMapping extends GhidraScript
{
    private static final Pattern MSVC_MANGLING = Pattern.compile("\\?.*");

    private String transformType(DataType ty)
    {
        if (ty == null) {
           return "void";
        }

        switch (ty.toString())
        {
        case "bool":
        case "char":
        case "uchar":
        case "short":
        case "ushort":
        case "int":
        case "uint":
        case "long":
        case "ulong":
        case "float":
        case "double":
            return ty.toString();

        case "i8":
            return "char";
        case "undefined":
        case "undefined1":
        case "byte":
        case "u8":
            return "uchar";

        case "i16":
            return "short";
        case "undefined2":
        case "u16":
            return "ushort";

        case "i32":
            return "int";
        case "undefined4":
        case "u32":
            return "uint";

        case "f32":
            return "float";
        case "f64":
            return "double";
        }
        if (ty instanceof Pointer)
        {
            Pointer ptr = (Pointer)ty;
            DataType pointee = ptr.getDataType();
            return transformType(pointee) + "*";
        }
        if (ty instanceof TypeDef)
        {
            // Typedef handling rules - chosen so the resulting CSV
            // entries are both informative *and* something
            // generate_sources.py can emit as a self-contained C++
            // declaration with at most a forward declaration:
            //
            //  - Typedefs whose base is a Pointer (`LPDIRECTDRAW`,
            //    `LPCSTR`, `HWND`, `HANDLE`, ...) flatten to the
            //    underlying pointer-to-struct / pointer-to-primitive.
            //    Otherwise the stub generator would have to emit
            //    `struct LPDIRECTDRAW;` which is invalid because
            //    `LPDIRECTDRAW` is a typedef of a pointer, not a
            //    struct tag.
            //
            //  - Typedefs whose base is a primitive or a struct
            //    (`HRESULT`, `DWORD`, `BOOL`, `WAVEFORMATEX`,
            //    `GUID`, ...) keep their typedef name so the
            //    stubs read naturally. Any new typedef name that
            //    pops up gets either forward-declared as
            //    `struct Name;` (for unknown structs, harmless) or
            //    needs to be added to `include/globals.h` as a
            //    one-line `typedef ...`.
            //
            //  - Anonymous / synthetic typedefs (no name, or names
            //    that match the base type's toString, or names that
            //    start with `__` like compiler-internal aliases)
            //    fall through to the base type so we don't leak
            //    Ghidra-internal identifiers into the CSV.
            TypeDef typedef = (TypeDef)ty;
            DataType base = typedef.getBaseDataType();
            if (base instanceof Pointer) {
                return transformType(base);
            }
            String name = typedef.getName();
            if (name == null || name.isEmpty()
                    || name.equals(base.toString())
                    || name.startsWith("__")) {
                return transformType(base);
            }
            return name;
        }
        return ty.getName();
    }

    public String generateCsv()
    {
        StringBuilder builder = new StringBuilder();

        FunctionIterator funcIter = currentProgram.getListing().getFunctions(true);
        while (funcIter.hasNext())
        {
            Function func = funcIter.next();

            if (func.isThunk())
            {
                continue;
            }


            Symbol mangledSymbol = getMangledNameSymbol(func);
            if (mangledSymbol != null) {
                builder.append(mangledSymbol.getName());
            }
            builder.append(";");
            builder.append(func.getName(true));
            builder.append(";0x");
            builder.append(Long.toHexString(func.getEntryPoint().getOffset()));
            builder.append(";0x");
            builder.append(Long.toHexString(func.getBody().getNumAddresses()));
            builder.append(";");
            builder.append(func.getCallingConventionName());
            builder.append(";");
            builder.append(func.hasVarArgs() ? "varargs" : "");
            builder.append(";");
            builder.append(transformType(func.getReturnType()));
            for (Parameter p : func.getParameters())
            {
                builder.append(";");
                builder.append(transformType(p.getDataType()));
            }
            builder.append("\n");
        }

        return builder.toString();
    }

    private Symbol getMangledNameSymbol(Function function) {
       Symbol[] functionStartSymbols = currentProgram.getSymbolTable().getSymbols(function.getEntryPoint());
       for (Symbol symbol : functionStartSymbols) {
            if (MSVC_MANGLING.matcher(symbol.getName(true)).matches()) {
                return symbol;
            }
       }
       return null;
    }

    @Override public void run() throws Exception
    {
        String mappingData = generateCsv();
        File outputMapping;
        String[] args = getScriptArgs();
        if (args != null && args.length > 0) {
            outputMapping = new File(args[0]);
        } else {
            outputMapping = askFile("mapping.csv", "Save");
        }
        Files.write(outputMapping.toPath(), mappingData.getBytes());
    }
}
