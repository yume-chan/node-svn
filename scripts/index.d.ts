import "node";

declare module svn {
    interface CommitInfo {

    }

    interface NodeInfo {

    }

    interface NodeStatus {

    }

    class Client {
        constructor();

        add_to_changelist(path: string | string[], changelist: string): void;
        get_changelists(path: string, callback: (path: string, changelist: string) => void): void;
        remove_from_changelists(path: string | string[]): void;

        add(path: string): void;
        cat(path: string): Buffer;
        checkout(url: string, path: string): void;
        commit(path: string | string[], message: string, callback: (info: CommitInfo) => void): void;
        info(path: string, callback: (path: string, info: NodeInfo) => void): void;
        remove(path: string | string[], callback: (info: CommitInfo) => void): void;
        revert(path: string | string[]): void;
        status(path: string, callback: (path: string, info: NodeStatus) => void): void;
        update(path: string): number;

        get_working_copy_root(path: string): string;
    }
}
