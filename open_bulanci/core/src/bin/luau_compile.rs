//! Compile a Luau source file to **stripped bytecode** for distribution
//! paks. Invoked by `open_bulanci/asset_pipeline/build_paks.py` so the
//! pak compiler can ship scenes/behaviors as bytecode instead of source.
//!
//! Debug level 0 drops local/upvalue names and line info, so the emitted
//! bytecode carries no identifiers — the same no-name-leak invariant the
//! asset pipeline enforces for slugs/paths. Optimization level 2 is the
//! Luau "release" tier.
//!
//!   luau_compile <in.luau> <out.luac>
//!   luau_compile --keep-debug <in.luau> <out.luac>   (dev: keep names)

use std::process::ExitCode;

fn main() -> ExitCode {
    let mut args: Vec<String> = std::env::args().skip(1).collect();
    let mut debug_level: u8 = 0;
    args.retain(|a| {
        if a == "--keep-debug" {
            debug_level = 1;
            false
        } else {
            true
        }
    });
    let (input, output) = match (args.first(), args.get(1)) {
        (Some(i), Some(o)) => (i.clone(), o.clone()),
        _ => {
            eprintln!("usage: luau_compile [--keep-debug] <in.luau> <out.luac>");
            return ExitCode::from(2);
        }
    };

    let src = match std::fs::read(&input) {
        Ok(s) => s,
        Err(e) => {
            eprintln!("luau_compile: read {input}: {e}");
            return ExitCode::from(1);
        }
    };

    let bytecode = match mlua::Compiler::new()
        .set_optimization_level(2)
        .set_debug_level(debug_level)
        .compile(&src)
    {
        Ok(b) => b,
        Err(e) => {
            eprintln!("luau_compile: compile {input}: {e}");
            return ExitCode::from(1);
        }
    };

    if let Err(e) = std::fs::write(&output, &bytecode) {
        eprintln!("luau_compile: write {output}: {e}");
        return ExitCode::from(1);
    }
    eprintln!(
        "luau_compile: {} -> {} ({} bytes, debug_level={debug_level})",
        input,
        output,
        bytecode.len(),
    );
    ExitCode::SUCCESS
}
