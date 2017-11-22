const path = require("path");
const fs = require("fs");

function find(folder) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full));
        } else if (path.extname(full) === ".dll") {
            result.push(full);
        }
    }
    return result;
}

const arch = process.argv[2] || process.arch;
const lib = `lib/svn/windows/${arch}`;

const configuration = process.argv[3] === "debug" ? "Debug" : "Release";
const target = `build/${configuration}`;

console.log(`Copying dependencies to ${target}...`);

find(lib).forEach(x => fs.copyFileSync(x, path.resolve(target, path.relative(lib, x))));
