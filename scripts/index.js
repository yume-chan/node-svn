const list = [
    "../build/Debug/svn.node",
    "../build/Release/svn.node",
];

(function() {
    for (const item of list) {
        try {
            module.exports = require(item);
            return;
        } catch (err) {
            continue;
        }
    }

    throw new Error("");
})();
