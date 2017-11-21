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

const platform = process.argv[2];
const from = `lib/svn/windows/${platform}`;

const configuration = process.argv[3] === "debug" ? "Debug" : "Release";
const target = `build/${configuration}`;

console.log(`Copying dependencies to ${target}...`);

find(from).forEach(x => fs.copyFileSync(x, path.resolve(target, path.relative(from, x))));
