from pathlib import Path
import json

try:
    import squarify
    from matplotlib import pyplot as plt
    from matplotlib import colormaps as cm
except ImportError as exc:
    raise SystemExit(
        "visualize_progress.py needs matplotlib + squarify. Install with: pip install matplotlib squarify"
    ) from exc

cmap = cm['RdYlGn']


def print_progress(file: Path, sizes, labels, matches):
    fig, axs = plt.subplots(1, 1, figsize=(40, 40))
    plt.rcParams.update({'font.size': 10})
    squarify.plot(sizes=sizes, norm_x=1000, norm_y=1000, label=labels, color=cmap(matches), pad=True, ax=axs)
    plt.axis("off")
    plt.savefig(str(file))
    plt.close()


def visualize_overall(report, output_file: Path):
    units = report["units"]
    sizes_arr = []
    matches = []
    labels_arr = []
    for unit in units:
        meas = unit["measures"]
        matched = meas.get("fuzzy_match_percent", 0)
        matches.append(matched / 100)
        sizes_arr.append(int(meas.get("total_code", 0)))
        labels_arr.append(f"{unit['name']}\n{matched:.1f}%")

    print_progress(output_file, sizes_arr, labels_arr, matches)


def visualize_unit(unit, output_file: Path):
    functions = unit.get("functions", [])
    sizes_arr = []
    matches = []
    labels_arr = []
    for function in functions:
        matched = function.get("fuzzy_match_percent", 0)
        matches.append(matched / 100)
        sizes_arr.append(int(function.get("size", 0)))
        labels_arr.append(f"{function['name']}\n{matched:.1f}%")

    print_progress(output_file, sizes_arr, labels_arr, matches)


def visualize(report_filepath: Path, output_folder: Path):
    with report_filepath.open("r") as report_file:
        report = json.load(report_file)
        visualize_overall(report, output_folder / "progress.png")

        progress_dir = output_folder / "progress"
        progress_dir.mkdir(exist_ok=True)
        for unit in report["units"]:
            unit_dir = progress_dir / unit["name"]
            unit_dir.parent.mkdir(parents=True, exist_ok=True)
            visualize_unit(unit, unit_dir)


if __name__ == "__main__":
    workspacePath = Path(__file__).parent.parent
    visualize(workspacePath / "report.json", workspacePath)
