const path = require("path");
const fs = require("fs");
const { exec } = require("child_process");

function find(folder, ext) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full, ext));
        } else if (path.extname(full) === ext) {
            result.push(full);
        }
    }
    return result;
}

const platform = process.argv[2] || process.platform;
const arch = process.argv[3] || process.arch;
const base = `lib/svn/${platform}/${arch}`;

const configuration = process.argv[4] === "debug" ? "Debug" : "Release";
const target = `build/${configuration}`;

console.log(`Copying dependencies to ${target}...`);

switch (platform) {
    case "win32":
        find(base, ".dll").forEach(x => fs.copyFileSync(x, path.resolve(target, path.relative(base, x))));
        break;
    case "linux":
        find(base, ".so").forEach(x => fs.copyFileSync(x, path.resolve(target, path.relative(base, x + ".0"))));

        console.log("Patching svn.node");
        exec(`patchelf --set-rpath '$ORIGIN/' ${target}/svn.node`);
        break;
}
