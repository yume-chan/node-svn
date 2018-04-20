const fs = require("fs-extra");
const path = require("path");
const uri = require("vscode-uri").default;

const { expect } = require("chai");

const global_config = false;
const config = global_config ? undefined : path.resolve(__dirname, "config");
const server = path.resolve(__dirname, "server");
const local = path.resolve(__dirname, "local");

const file1 = path.resolve(local, "file1.txt").replace(/\\/g, "/");

async function async_iterate(value, callback) {
    const iterator = value[Symbol["asyncIterator"]]();

    do {
        let result = await iterator.next();

        if (result.done === true) {
            break;
        }

        callback(result.value);
    } while (true);
}

describe("svn.node", () => {
    before(function() {
        if (!global_config) {
            fs.removeSync(config);
        }

        fs.removeSync(server);
        fs.removeSync(local);
    });

    let svn;
    let client;

    it("load", () => {
        svn = require("..");
    });

    it("delay", async function() {
        await new Promise(resolve => {
            setTimeout(resolve, 500);
        });
    });

    it("version", () => {
        const version = svn.version;

        expect(version).to.be.an("object");
        expect(version.major).to.equal(1);
        expect(version.minor).to.equal(11);
        expect(version.patch).to.equal(0);
    });

    it("create repository", () => {
        svn.create_repos(server);

        expect(fs.existsSync(server)).to.be.true;
    });

    it("new Client", () => {
        client = new svn.Client(config);

        if (!global_config) {
            expect(fs.existsSync(config)).to.be.true;
        }
    });

    it("dispose", () => {
        const start = process.memoryUsage().rss;

        for (let i = 0; i < 1000; i++) {
            const client = new svn.Client(config);
            client.dispose();
        }

        const end = process.memoryUsage().rss;

        expect(end - start).to.lessThan(2 * 1000 * 1000);
    });

    it("checkout", async function() {
        this.timeout(0);

        const url = uri.file(server).toString(true);
        const revision = await client.checkout(url, local);
        expect(revision, "revision").to.equal(0);

        expect(fs.existsSync(local)).to.be.true;
    });

    it("status after creating file", async function() {
        this.timeout(0);

        await fs.writeFile(file1, file1);

        let count = 0;
        const result = client.status(local);
        await async_iterate(result, (item) => {
            count++;
            expect(item.path, "item.path").to.equal(file1);
            expect(item.kind, "item.kind").to.equal(svn.NodeKind.unknown);
            expect(svn.StatusKind[item.node_status], "item.node_status").to.equal(svn.StatusKind[svn.StatusKind.unversioned]);
            expect(svn.StatusKind[item.text_status], "item.text_status").to.equal(svn.StatusKind[svn.StatusKind.none]);
        });
        expect(count, "count").to.equal(1);

        // WARNING: **NEVER** DO THIS IN PRODUCTION
        // `status()` runs asynchronously
        // here it's testing threading safety
        const tasks = [];
        for (let i = 0; i < 20; i++) {
            const client = new svn.Client(config);
            tasks.push(async_iterate(client.status(local), () => { }));
        }
        await Promise.all(tasks);
    });

    it("add file", async function() {
        this.timeout(0);

        await client.add(file1);
    });

    it("status after adding file", async function() {
        this.timeout(0);

        await fs.writeFile(file1, file1);

        let count = 0;
        const result = client.status(local);
        await async_iterate(result, (item) => {
            count++;
            expect(item.path, "item.path").to.equal(file1);
            expect(item.kind, "item.kind").to.equal(svn.NodeKind.file);
            expect(svn.StatusKind[item.node_status], "item.node_status").to.equal(svn.StatusKind[svn.StatusKind.added]);
            expect(svn.StatusKind[item.text_status], "item.text_status").to.equal(svn.StatusKind[svn.StatusKind.modified]);
        });
        expect(count, "count").to.equal(1);
    });

    it("commit", async function() {
        this.timeout(0);

        let count = 0;
        const result = client.commit(local, "commit1");
        await async_iterate(result, (item) => {
            switch (count) {
                case 0:
                    expect(item.action, "action").to.equal(16);
                    expect(item.path, "path").to.equal(file1);
                    break;
                case 1:
                    expect(item.action, "action").to.equal(19);
                    expect(item.path, "path").to.equal(file1);
                    break;
                case 2:
                    expect(item.action, "action").to.equal(79);
                    expect(item.path, "path").to.equal(".");
                    expect(item.revision, "item.revision").to.equal(1);
                    expect(item.post_commit_error, "item.post_commit_error").to.be.undefined;
                    break;
                default:
                    throw new Error();
            }

            count++;
        });
    });

    it("checkout again", async function() {
        this.skip();
        this.timeout(0);

        client.dispose();
        client = new svn.Client(config);

        fs.removeSync(local);

        const url = uri.file(server).toString(true);
        const revision = await client.checkout(url, local);
        expect(revision, "revision").to.equal(1);

        expect(fs.existsSync(local)).to.be.true;
    });

    it("update", async function() {
        this.skip();
        this.timeout(0);

        const result = client.update(local);
        await async_iterate(result, (item) => {
            console.log(item);
        });
    });

    it("status after modifying file", async function() {
        this.timeout(0);

        await fs.writeFile(file1, file1 + file1);

        let count = 0;
        const result = client.status(local);
        await async_iterate(result, (item) => {
            count++;
            expect(item.path, "item.path").to.equal(file1);
            expect(item.kind, "item.kind").to.equal(svn.NodeKind.file);
            expect(svn.StatusKind[item.node_status], "item.node_status").to.equal(svn.StatusKind[svn.StatusKind.modified]);
            expect(svn.StatusKind[item.text_status], "item.text_status").to.equal(svn.StatusKind[svn.StatusKind.modified]);
        });
        expect(count).to.equal(1);
    });

    it("cat", async function() {
        this.timeout(0);

        let result = await client.cat(file1);
        expect(result.content.toString("utf-8")).to.equal(file1);

        await fs.writeFile(file1, file1 + file1);

        result = await client.cat(file1);
        expect(result.content.toString("utf-8")).to.equal(file1);

        result = await client.cat(file1, { revision: svn.RevisionKind.working });
        expect(result.content.toString("utf-8")).to.equal(file1 + file1);
    });

    describe("changelist", () => {
        const changelist = Date.now().toString();

        it("add_to_changelist", async function() {
            this.timeout(0);

            await client.add_to_changelist(file1, changelist);
        });

        it("get_changelists", async function() {
            this.timeout(0);

            let count = 0;
            const result = client.get_changelists(file1);
            await async_iterate(result, (item) => {
                count++;
                expect(item.path).to.equal(file1);
            });
            expect(count).to.equal(1);
        });

        it("remove_from_changelists", async function() {
            this.timeout(0);

            await client.remove_from_changelists(file1);

            const result = client.get_changelists(file1);
            await async_iterate(result, (item) => {
                throw new Error(JSON.stringify(item));
            });
        });
    });

    it("blame", async function() {
        if (!global_config) {
            this.skip();
        }

        this.timeout(0);

        let max = {
            revision: 0,
            author: 0,
            message: 0
        };

        let iterable = client.blame("c:/Users/Simon/Desktop/dev/webchat/index.html");
        const revisions = new Map();
        const lines = [];
        await async_iterate(iterable, (value) => {
            let info;
            if (revisions.has(value.revision)) {
                info = revisions.get(value.revision);
            } else {
                info = {};
                revisions.set(value.revision, info);

                const length = value.revision.toString().length;
                if (length > max.revision) {
                    max.revision = length;
                }
            }

            lines.push({
                revision: value.revision,
                content: value.line,
                info: info
            });
        });

        const ranges = [];
        for (const [item] of revisions) {
            const revision = { number: item };
            ranges.push({
                start: revision,
                end: revision
            });
        }
        iterable = client.log("c:/Users/Simon/Desktop/dev/webchat/index.html", {
            revision_ranges: ranges
        });
        await async_iterate(iterable, (value) => {
            const info = revisions.get(value.revision);
            info.message = value.message;
            info.author = value.author;

            if (value.author.length > max.author) {
                max.author = value.author.length;
            }
            if (value.message.length > max.message) {
                max.message = value.message.length;
            }
        });

        function count(value) {
            return value.split("").reduce((a, c) => a += 1);
        }

        let text = "";
        for (const line of lines) {
            text += `${line.revision.toString().padEnd(max.revision)} ${line.info.author.padEnd(max.author)} ${line.info.message.padEnd(max.message)} ${line.content.replace(/\r\n?/g, "")}\r\n`;
        }
        await fs.writeFile("blame.txt", text);
    });
});
