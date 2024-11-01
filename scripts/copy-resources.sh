# Copy Resources (Linux)
# Copies Source folder and paste into Debug/Release folders
# made by vertoker

# Variables (relative to the project folder)
SOURCE="resources"
DEBUG="out/Debug/resources"
RELEASE="out/Release/resources"

# Go to the directory of launching this script
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# And go up, to the project folder
cd "$SCRIPT_DIR/.."

# Create folders Debug/Release
mkdir -p -v $DEBUG
mkdir -p -v $RELEASE
# Delete all files inside folders Debug/Release
rm -rf $DEBUG
rm -rf $RELEASE
# Copies all files from Source to new Debug/Release folders
cp -rf $SOURCE $DEBUG
cp -rf $SOURCE $RELEASE
