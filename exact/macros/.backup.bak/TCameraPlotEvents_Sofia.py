import os
import subprocess
import sys

# Set environment variables
os.environ['EXACT_DIR'] = '/storage/hive/project/phy-otte/shared/analysistools/exact'
os.environ['LD_LIBRARY_PATH'] = f"{os.environ['EXACT_DIR']}/dict:{os.environ['EXACT_DIR']}/dict/include:/storage/hive/project/phy-otte/shared/analysistools/VERITAS/lib:{os.environ.get('LD_LIBRARY_PATH', '')}"

# Check if the command-line argument is provided
if len(sys.argv) != 2:
    print("Usage: python script.py <folder_name>")
    sys.exit(1)

# Get the folder name from the command-line argument
folder_name = sys.argv[1]

# Construct the full path to the ROOT directory
root_dir = f"/storage/hive/project/phy-otte/shared/Trinity/Data/{folder_name}/RawDataMerged"

# Directory containing the .root files
root_script = "/storage/hive/project/phy-otte/shared/analysistools/exact/macros/TCameraPlotEvents_Trinity_saveimg.cpp"

# List all .root files in the directory and sort them
root_files = [f for f in os.listdir(root_dir) if f.endswith(".root")]
root_files.sort()

# Get just the last ten files
last_ten_files = root_files[-10:]

# Iterate over files in the directory
for filename in last_ten_files:
    if filename.endswith(".root"):
        # Construct the full path to the .root file
        root_file = os.path.join(root_dir, filename)

        # Run the ROOT script with the file as an argument
        cmd = f"root -b -q '{root_script}(\"{root_file}\", \"Test\")'"
        subprocess.call(cmd, shell=True)
