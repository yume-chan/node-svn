const svn = require("../bin/x64/Debug/node-svn.node");
const client = new svn.AsyncClient();
client.info("C:\\Users\\Simon\\Desktop\\www\\webchat", (info) => {
    console.log(JSON.stringify(info));
}, { revision: svn.RevisionKind.working });

console.log("exit");

setInterval(() => {}, 1000);