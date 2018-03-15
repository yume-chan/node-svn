const svn = require("..");

const simple_auth = {
    username: "ad_cxm",
    password: "cxm123",
    may_save: false
};

function simple_auth_provider(realm, username, may_save) {
    return { ...simple_auth };
}

function resolved_simple_auth_provider(realm, username, may_save) {
    return Promise.resolve({ ...simple_auth });
}

function timeout_simple_auth_provider(realm, username, may_save) {
    return new Promise(resolve => {
        setTimeout(() => {
            resolve({ ...simple_auth })
        }, 2000);
    });
}

async function test_changelist() {
    try {
        const client = new svn.Client();

        await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");

        await client.remove_from_changelists("c:/Users/Simon/Desktop/dev/webchat/index.html")
        await client.add_to_changelist("c:/Users/Simon/Desktop/dev/webchat/index.html", Date.now().toString());

        const iterable = client.get_changelists("c:/Users/Simon/Desktop/dev/webchat/index.html");
        const iterator = iterable[Symbol["asyncIterator"]]();
        let result;
        do {
            result = await iterator.next();
            console.log(result.value);
        } while (result.done === false);
    } catch (err) {
        console.error(err);
    }
}

async function test_info() {
    try {
        const client = new svn.Client();

        const iterable = client.info("c:/Users/Simon/Desktop/dev/webchat");
        const iterator = iterable[Symbol["asyncIterator"]]();
        let result;
        do {
            result = await iterator.next();
            console.log(result.value);
        } while (result.done === false);
    } catch (err) {
        console.error(err);
    }
}

async function update_with_simple_auth_provider() {
    const client = new svn.Client();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    client.add_simple_auth_provider(resolved_simple_auth_provider);
    const revision = await client.update("c:/Users/Simon/Desktop/dev/webchat");
    console.log(revision);
}

async function status() {
    console.log("start");

    const client = new svn.Client();

    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    console.log("cleanup complete");

    const iterable = client.status("c:/Users/Simon/Desktop/dev/webchat");
    await async_iterate(iterable, value => {
        console.log(value);
    });
    console.log("pass");
}

async function status_memory_leak() {
    const client = new svn.Client();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    const before = process.memoryUsage().rss;
    console.log(before);
    for (let i = 0; i < 10000; i++) {
        const iterable = client.status("c:/Users/Simon/Desktop/dev/webchat");
        await async_iterate(iterable, value => {
            console.log(value);
        });
    }
    const after = process.memoryUsage().rss;
    console.log(after);
    console.log("pass");
}

async function async_iterate(value, callback) {
    const iterator = value[Symbol["asyncIterator"]]();
    let result = await iterator.next();
    while (result.done === false) {
        callback(result.value);
        result = await iterator.next();
    }
}

async function info_memory_leak() {
    const client = new svn.Client();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    const before = process.memoryUsage().rss;
    console.log(before);
    for (let i = 0; i < 100000; i++) {
        const iterable = client.info("c:/Users/Simon/Desktop/dev/webchat");
        await async_iterate(iterable, value => {
            console.log(value);
        });
    }
    const after = process.memoryUsage().rss;
    console.log(after);
    console.log("pass");
}

async function test_blame() {
    const client = new svn.Client();
    await client.cleanup("c:/Users/Simon/Desktop/dev/webchat");
    const iterable = client.blame("c:/Users/Simon/Desktop/dev/webchat/index.html");
    await async_iterate(iterable, value => {
        console.log(value);
    });
}

async function test_iterator() {
    console.log("start");

    // const iterator = svn.test();
    // let value;
    // do {
    //     const promise = iterator.next();
    //     console.log(promise);

    //     value = await promise;
    //     console.log(value);
    // } while (!value.done);

    // const result = await svn.test();
    // console.log(result);

    console.log("end");
}

try {
    console.log("pid: " + process.pid);

    // test_changelist();
    // test_info()
    // update_with_simple_auth_provider();
    // status();
    // status_memory_leak();
    // info_memory_leak();
    // test_iterator();
    test_blame();
} catch (err) {
    console.log(err);
}
