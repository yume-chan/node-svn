const path = require("path");

const root = path.resolve(__dirname, "../../");

function normalize(value) {
    return path.relative(root, value);
}

normalize.root = root;

module.exports = normalize;
