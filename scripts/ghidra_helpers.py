import os
from pathlib import Path
import re
import shlex
import subprocess


SCRIPT_PATH = Path(os.path.realpath(__file__)).parent


def findAnalyzeHeadless():
    ghidra_home = None

    if os.getenv("GHIDRA_HOME") is not None:
        ghidra_home = Path(os.getenv("GHIDRA_HOME"))

    if ghidra_home is not None:
        if os.name == "nt":
            analyze_headless = ghidra_home / "support" / "analyzeHeadless.bat"
        else:
            analyze_headless = ghidra_home / "support" / "analyzeHeadless"
        if analyze_headless.exists():
            return str(analyze_headless)

    raise Exception(
        "Could not find Ghidra installation. Please install ghidra from https://ghidra-sre.org/ and set your GHIDRA_HOME environment variable to the installation directory"
    )


def runAnalyze(
    ghidra_repo_name,
    project_name,
    process=None,
    import_file=None,
    analysis=False,
    username=None,
    pre_scripts=None,
    post_scripts=None,
    read_only=True,
):
    """Invoke Ghidra's `analyzeHeadless` against a project.

    `read_only` defaults to True because the export-side scripts
    (ExportDelinker, GenerateMapping) only need to read symbols. Set to
    False for one-shot rename / namespace-mutation passes that need to
    persist back to the .gpr (e.g. `PromoteVftableMembers.java`)."""
    if pre_scripts is None:
        pre_scripts = []
    if post_scripts is None:
        post_scripts = []
    commonAnalyzeHeadlessArgs = [findAnalyzeHeadless(), ghidra_repo_name]

    if not re.match("^ghidra://", ghidra_repo_name):
        commonAnalyzeHeadlessArgs += [project_name]

    if process and import_file:
        raise Exception("Cannot provide both import and process")
    elif process:
        commonAnalyzeHeadlessArgs += ["-process", process]
    elif import_file:
        commonAnalyzeHeadlessArgs += ["-import", import_file]

    if read_only:
        commonAnalyzeHeadlessArgs += ["-readOnly"]
    commonAnalyzeHeadlessArgs += [
        "-scriptPath",
        str(SCRIPT_PATH / "ghidra"),
    ]

    if not analysis:
        commonAnalyzeHeadlessArgs += ["-noanalysis"]

    for pre_script in pre_scripts:
        if isinstance(pre_script, list):
            commonAnalyzeHeadlessArgs += ["-prescript"] + pre_script
        elif isinstance(pre_script, str):
            commonAnalyzeHeadlessArgs += ["-prescript", pre_script]

    for post_script in post_scripts:
        if isinstance(post_script, list):
            commonAnalyzeHeadlessArgs += ["-postscript"] + post_script
        elif isinstance(post_script, str):
            commonAnalyzeHeadlessArgs += ["-postscript", post_script]

    commonAnalyzeHeadlessEnv = os.environ.copy()
    if username is not None:
        commonAnalyzeHeadlessEnv["_JAVA_OPTIONS"] = (
            f"-Duser.name={username} " + os.environ.get("_JAVA_OPTIONS", "")
        )

    print("Running " + " ".join(shlex.quote(x) for x in commonAnalyzeHeadlessArgs))
    return subprocess.run(
        commonAnalyzeHeadlessArgs, env=commonAnalyzeHeadlessEnv, check=True
    )
