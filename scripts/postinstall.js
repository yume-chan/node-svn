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

find("lib").forEach(x => fs.copyFileSync(x, path.resolve("build/Release", path.relative("lib", x))));
