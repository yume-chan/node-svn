let done = false;

(function keep_alive() {
    if (done)
        return;

    setTimeout(keep_alive, 1000);
})();

try {
    const svn = require("..");
    const client = new svn.AsyncClient();

    client.remove_from_changelists(String.raw`D:\Svn\Working Copy\repo1\file-that-is-external.txt`)
        .then(() => { console.log("success"); }, (error) => { console.error(JSON.stringify(error)); });
}
catch (err) {
    console.error(err);
    // done = true;
}
