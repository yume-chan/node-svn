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

async function async_status_memory_leak(svn) {
    const client = new svn.AsyncClient();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    const before = process.memoryUsage().rss;
    console.log(before);
    for (let i = 0; i < 10000; i++) {
        await client.status("c:/Users/Simon/Desktop/dev/webchat", status => {
            // console.log(status);
        });
    }
    const after = process.memoryUsage().rss;
    console.log(after);
    console.log("pass");
}

async function async_info_memory_leak(svn) {
    const client = new svn.AsyncClient();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    const before = process.memoryUsage().rss;
    console.log(before);
    for (let i = 0; i < 100000; i++) {
        await client.info("c:/Users/Simon/Desktop/dev/webchat", info => {
            // console.log(info);
        });
    }
    const after = process.memoryUsage().rss;
    console.log(after);
    console.log("pass");
}

async function async_blame(svn) {
    const client = new svn.AsyncClient();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    client.blame()
}

function test_async_iterator(svn) {
    const result = svn.test();
    console.log(typeof Symbol.asyncIterator);
    console.log(typeof result[Symbol.asyncIterator]);
    const iterator = result[Symbol.asyncIterator]();
    console.log(typeof iterator.next);
    const promise = iterator.next();
    console.log(promise);
    promise.then(args => {
        console.log(args);
    });
}

try {
    console.log(process.pid);

    process.stdin.resume();
    process.stdin.on("data", () => {
        const svn = require("..");
        // async_update_with_async_simple_auth_provider(svn);
        // async_status(svn);
        // async_status_memory_leak(svn);
        // async_info_memory_leak(svn);
        test_async_iterator(svn);
    });
} catch (err) {
    console.log(err.stack);
}
