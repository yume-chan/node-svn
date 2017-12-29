try {
    const svn = require("..");
    console.log(process.pid);

    process.stdin.resume();
    process.stdin.on("data", () => {
        const client = new svn.Client();
        const revision = client.checkout("http://svn.apache.org/repos/asf/subversion/trunk", "c:/Users/Simon/Desktop/subversion");
        console.log(revision);
    });
} catch (err) {
    console.log(err.stack);
}
