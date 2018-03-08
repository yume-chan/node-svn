function update_with_simple_auth_provider(svn) {
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

const simple_auth = {
    username: "ad_cxm",
    password: "cxm123",
    may_save: false
};

function simple_auth_provider(realm, username, may_save) {
    return { ...simple_auth };
}

function async_resolved_simple_auth_provider(realm, username, may_save) {
    return Promise.resolve({ ...simple_auth });
}

function async_timeout_simple_auth_provider(realm, username, may_save) {
    return new Promise(resolve => {
        setTimeout(() => {
            resolve({ ...simple_auth })
        }, 2000);
    });
}

async function async_update_with_async_simple_auth_provider(svn) {
    const client = new svn.AsyncClient();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    client.add_simple_auth_provider(async_resolved_simple_auth_provider);
    const revision = await client.update("c:/Users/Simon/Desktop/dev/webchat");
    console.log(revision);
}

async function async_status(svn) {
    const client = new svn.AsyncClient();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    await client.status("c:/Users/Simon/Desktop/dev/webchat", status => {
        console.log(status);
    });
    console.log("pass");
}

try {
    const svn = require("..");
    console.log(process.pid);

    // async_update_with_async_simple_auth_provider(svn);
    async_status(svn);
} catch (err) {
    console.log(err.stack);
}
