const fs = require("fs-extra");
const path = require("path");
const uri = require("vscode-uri").default;

const { expect } = require("chai");

const global_config = false;
const config = global_config ? undefined : path.resolve(__dirname, "config");
const server = path.resolve(__dirname, "server");
const repository = path.resolve(__dirname, "repository");

const file1 = path.resolve(repository, "file1.txt").replace(/\\/g, "/");

async function async_iterate(value, callback) {
    const iterator = value[Symbol["asyncIterator"]]();
    let result = await iterator.next();
    while (result.done === false) {
        callback(result.value);
        result = await iterator.next();
    }
}

describe("svn.node", () => {
    before(function() {
        if (!global_config) {
            fs.removeSync(config);
        }

        fs.removeSync(server);
        fs.removeSync(repository);
    });

    let svn;
    let client;

    it("load", () => {
        svn = require("..");
    });

    it("create repository", () => {
        svn.create_repos(server);

        expect(fs.existsSync(server)).to.be.true;
    });

    it("create Client", async () => {
        client = new svn.Client(config);

        if (!global_config) {
            expect(fs.existsSync(config)).to.be.true;
        }
    });

    it("checkout", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        const url = uri.file(server).toString(true);
        const revision = await client.checkout(url, repository);
        expect(revision).to.be.a("number");

        expect(fs.existsSync(repository)).to.be.true;
    });

    it("add file", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        await fs.writeFile(file1, file1);
        await client.add(file1);
    });

    it("status", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        const result = client.status(repository);
        await async_iterate(result, (value) => {
            expect(value.path).to.equal(file1);
            expect(value.kind).to.equal(svn.NodeKind.file);
        });

        // WARNING: **NEVER** DO THIS IN PRODUCTION
        // `status()` runs asynchronously
        // here it's testing threading safety
        const tasks = [];
        for (let i = 0; i < 20; i++) {
            const client = new svn.Client(config);
            tasks.push(async_iterate(client.status(repository), () => { }));
        }
        await Promise.all(tasks);
    });

    it("memory leak", async function() {
        this.skip();
        this.timeout(0);

        while (true) {
            new svn.Client(config);
        }
    });

    it("commit", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        const result = await client.commit(repository, "commit1");
        await async_iterate(result, (value) => {
            expect(value.revision).to.equal(1);
            expect(value.post_commit_error).to.be.undefined;
        });
    });

    it("cat", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        let result = await client.cat(file1);
        expect(result.content.toString("utf-8")).to.equal(file1);

        await fs.writeFile(file1, file1 + file1);

        result = await client.cat(file1, { revision: svn.RevisionKind.base });
        expect(result.content.toString("utf-8")).to.equal(file1);
    });

    it("blame", async function() {
        if (!global_config) {
            this.skip();
        }

        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
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
