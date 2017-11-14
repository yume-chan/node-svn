const list = [
    "../build/Debug/svn.node",
    "../build/Release/svn.node",
];

for (const item of list) {
    try {
        module.exports = require(item);
        break;
    } catch (err) { }
}
