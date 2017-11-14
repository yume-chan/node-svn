const svn = require("../bin/x64/Debug/node-svn.node");
const client = new svn.AsyncClient();
const promise = client.get_working_copy_root("C:\\Users\\Simon\\Desktop\\www\\webchat\\static");
promise.then(value => console.log(value));

setInterval(()=>{}, 1000);