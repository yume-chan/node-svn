const child_process = require("child_process");
const path = require("path");
const fs = require("fs");
const request = require("request-promise-native");

const base = path.resolve(__dirname, "..");

function compile(target) {
    return new Promise((resolve, reject) => {
        const command = "node";

        const args = [
            path.resolve(require.resolve("node-gyp"), "..", "..", "bin", "node-gyp.js"),
            "rebuild"
        ];

        /** @type {child_process.SpawnOptions} */
        const options = {
            cwd: base,
            env: {
                "npm_config_runtime": "electron",
                "npm_config_disturl": "https://atom.io/download/electron",
                "npm_config_target": target
            },
            shell: false
        };

        const gyp = child_process.spawn(command, args, options);

        gyp.stdout.pipe(process.stdout);
        gyp.stderr.pipe(process.stderr);

        gyp.on("error", (err) => {
            reject(err);
        });
        gyp.on("exit", (code) => {
            if (code !== 0) {
                reject(`node-gyp return: ${code}`);
            } else {
                resolve();
            }
        });
    });
}

async function upload(api) {
    console.log("uploading to server");

    await request.post({
        url: "https://chensi.moe/upload-svn",
        headers: {
            "x-auth": process.env.upload_svn_auth,
            "x-filename": `latest_${process.platform}-${process.arch}_electron-${api}.node`
        },
        body: fs.createReadStream(path.resolve(base, "build", "Release", "svn.node"))
    });

    console.log("done");
}

(async () => {
    try {
        // API 54
        await compile("1.7.9");
        await upload(54);

        // API 59
        await compile("2.0.0-beta.7");
        await upload(59);
    } catch (err) {
        console.error(err);
    }
})();

