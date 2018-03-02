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

function async_simple_auth_provider(realm, username, may_save) {
    return Promise.resolve({
        username: "ad_cxm",
        password: "cxm123",
        may_save: false
    });
}

async function testAsync(svn) {
    try {
        const client = new svn.AsyncClient();
        await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
        await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
        client.add_simple_auth_provider(async_simple_auth_provider);
        client.remove_simple_auth_provider(async_simple_auth_provider);
        const revision = await client.update("c:/Users/Simon/Desktop/dev/webchat");
        // const revision = await Promise.resolve(1);
        console.log(revision);
    } catch (e) {
        console.error(e);
    }
}

try {
    const svn = require("..");
    console.log(process.pid);

    // process.stdin.resume();
    // process.stdin.on("data", () => {
    testAsync(svn);
    // });
} catch (err) {
    console.log(err.stack);
}
