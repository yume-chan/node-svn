function test(svn) {
    const client = new svn.Client();
    client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    client.add_simple_auth_provider((realm, username, may_save) => {
        return Promise.resolve({
            username: "ad_cxm",
            password: "cxm123",
            may_save: false
        });
    })
    const revision = client.update("c:/Users/Simon/Desktop/dev/webchat");
    console.log(revision);
}

async function testAsync(svn) {
    const asyncClient = new svn.AsyncClient();
    asyncClient.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    asyncClient.add_simple_auth_provider((realm, username, may_save) => {
        return Promise.resolve({
            username: "ad_cxm",
            password: "cxm123",
            may_save: false
        });
    });
    const revision = await asyncClient.update("c:/Users/Simon/Desktop/dev/webchat");
    await asyncClient.update("c:/Users/Simon/Desktop/dev/webchat");
    console.log(revision);
}

try {
    const svn = require("..");
    console.log(process.pid);

    process.stdin.resume();
    process.stdin.on("data", async () => {
        testAsync(svn);
    });
} catch (err) {
    console.log(err.stack);
}
