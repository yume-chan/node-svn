const svn = require("../bin/x64/Debug/node-svn.node");
const client = new svn.Client();
client.addToChangelist(String.raw`C:\Users\Simon\Desktop\git\svn\version.txt`, "test");
client.getChangelist(String.raw`C:\Users\Simon\Desktop\git\svn`, function(path, changelist) {
    console.log(`path: ${path}, changelist: ${changelist}`);
});
client.removeFromChangelists(String.raw`C:\Users\Simon\Desktop\git\svn\version.txt`);
