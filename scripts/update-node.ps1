$version = node -v
$version = $version.substring(1);

Write-Output "Downloading SDK for Node $version...";

node-gyp.cmd install $version --devdir node-gyp --disturl=https://npm.taobao.org/mirrors/node *> $null

Write-Output "Extracting...";

Remove-Item -Path include/node -Recurse *> $null
Copy-Item -Path node-gyp/$version/include/node -Destination include/node -Recurse *> $null

Remove-Item -Path lib/node/* -Recurse *> $null

New-Item -Path lib/node/windows/x64 -ItemType Directory *> $null
Copy-Item -Path node-gyp/$version/x64/* -Destination lib/node/windows/x64 -Recurse *> $null

New-Item -Path lib/node/windows/x86 -ItemType Directory *> $null
Copy-Item -Path node-gyp/$version/ia32/* -Destination lib/node/windows/x86 -Recurse *> $null

Write-Output "Done";
