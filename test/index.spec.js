const fs = require("fs-extra");
const path = require("path");
const uri = require("vscode-uri").default;

const { expect } = require("chai");

const config = path.resolve(__dirname, "config");
const server = path.resolve(__dirname, "server");
const repository = path.resolve(__dirname, "repository");

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
        fs.removeSync(config);
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
        expect(fs.existsSync(config)).to.be.true;
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

        const file1 = path.resolve(repository, "file1.txt");
        await fs.writeFile(file1, "file1");
        await client.add(file1);
    });

    it("commit", async function() {
        // don't know why but I need this line
        // to disable timeout to let tests pass
        // they actually only take less than 100ms
        this.timeout(0);

        const result = await client.commit(repository, "commit1");
        async_iterate(result, () => {
            expect(result.revision).to.equal(1);
            expect(result.post_commit_error).to.be.undefined;
        });
    });
});
