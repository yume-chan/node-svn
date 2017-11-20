const svn = require("../bin/x64/Debug/node-svn.node");
const client = new svn.AsyncClient();

client.info("C:\\Users\\Simon\\Desktop\\www\\webchat", (file, info) => {
    console.log(file);
    console.log(JSON.stringify(info));
}, { revision: svn.RevisionKind.working });

//client.status("C:\\Users\\Simon\\Desktop\\www\\webchat", (file, info) => {
//    console.log(file);
//    console.log(JSON.stringify(info));
//});

console.log("exit");

setInterval(() => {}, 1000);