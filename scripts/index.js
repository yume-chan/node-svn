const list = [
    "../bin/x64/Debug/node-svn.node",
    "../bin/x64/Release/node-svn.node",
];

for (const item of list) {
    try {
        module.exports = require(item);
        break;
    } catch (err) { }
}
