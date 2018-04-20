$env:npm_config_disturl="https://atom.io/download/electron"
$env:npm_config_target="1.7.9"
$env:npm_config_runtime="electron"
# node-gyp rebuild
# cp build/Release/svn.node release/0.0.12_win32-x64_electron-54.node

$env:npm_config_target="2.0.0-beta.7"
node-gyp configure build
# Copy-Item "build/Release/svn.node" "release/0.0.12_win32-x64_electron-57.node"
