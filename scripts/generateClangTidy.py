# Checks if the clang tidy files exist and are up to date.
# This does not check if they were modified so its possible that they are not correct

import yaml
from pathlib import Path

Folders = {"engine", "test"}
Template_File_Name = ".clang-tidy.template.yaml"
Root_Dir = Path(__file__).resolve().parent.parent
Template_Path = Root_Dir / Template_File_Name


def needs_update(output_file, template_time):
    if not output_file.exists():
        return True
    return output_file.stat().st_mtime < template_time


def generate_clang_tidy():
    if not Template_Path.exists():
        raise FileNotFoundError(
            f"Could not find {Template_File_Name} at {Template_Path}")

    with open(Template_Path, "r") as f:
        template = yaml.safe_load(f)
        template_time = Template_Path.stat().st_mtime
        for folder in Folders:
            abs_dir = Root_Dir / folder
            output_file = abs_dir / ".clang-tidy"

            if not needs_update(output_file, template_time):
                print({f"{output_file} up to date"})
                continue

            dir_name = abs_dir.name.upper()
            macro_prefix = f"SSS{dir_name}_"

            config = template.copy()
            options = list(config.get("CheckOptions", []))

            options = [
                opt for opt in options
                if opt["key"] != "readability-identifier-naming.MacroDefinitionPrefix"
            ]

            options.append({
                "key": "readability-identifier-naming.MacroDefinitionPrefix",
                "value": macro_prefix
            })

            config["CheckOptions"] = options

            with open(output_file, "w") as output:
                output.write("# AUTO GENERATED. DO NOT EDIT!\n")
                yaml.dump(config, output, default_flow_style=False,
                          sort_keys=False)


if __name__ == "__main__":
    generate_clang_tidy()
