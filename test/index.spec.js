const { expect } = require("chai");

describe("svn.node", () => {
    it("should load", () => {
        expect(require("..")).to.be.an("object");
    });

    it("should have Client", () => {
        expect(require("..")).to.have.property("Client");
    });

    it("should have AsyncClient", () => {
        expect(require("..")).to.have.property("AsyncClient");
    });
});
