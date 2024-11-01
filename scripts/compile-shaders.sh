# Compile shaders (Linux)
# Compile shaders (using glslc) from Input into Output folder
# made by vertoker

# Variables (relative to the project folder)
INPUT="src/shaders"
OUTPUT="resources/shaders"

# Go to the directory of launching this script
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
# And go up, to the project folder
cd "$SCRIPT_DIR/.."

# Create Output folder
mkdir -p -v "$OUTPUT"

# Compile all shaders (TODO add auto detection)
glslc "$INPUT/simple.vert" -o "$OUTPUT/simple.vert.spv"
glslc "$INPUT/simple.frag" -o "$OUTPUT/simple.frag.spv"
