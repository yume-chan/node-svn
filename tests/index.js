let done = false;

(function keep_alive() {
    if (done)
        return;

    setTimeout(keep_alive, 1000);
})();

try {
    const svn = require("..");
    const client = new svn.AsyncClient();

    // client.info("C:\\Users\\Simon\\Desktop\\www\\webchat", (file, info) => {
    //     console.log(file);
    //     console.log(JSON.stringify(info));
    // }, { revision: svn.RevisionKind.working });

    // client.status("C:\\Users\\Simon\\Desktop\\www\\webchat", (file, info) => {
    //     console.log(file);
    //     console.log(JSON.stringify(info));
    // });
    // client.status("C:\\Users\\Simon\\Desktop\\www\\webchat", (file, info) => {
    //     console.log(file);
    //     console.log(JSON.stringify(info));
    // });

    client.cat("C:\\Users\\Simon\\Desktop\\www\\webchat\\index.html").then(result => {
        console.log(JSON.stringify(result));
        done = true;
    }, err => {
        console.error(err);
        done = true;
    });

}
catch (err) {
    console.error(err);
    done = true;
}
