try {
    const svn = require("..");
    console.log(process.pid);

    process.stdin.resume();
    process.stdin.on("data", () => {
        const client = new svn.Client();
        const revision = client.update("c:/Users/Simon/Desktop/www/webpc");
        console.log(revision);
    });
} catch (err) {
    console.log(err.stack);
}
