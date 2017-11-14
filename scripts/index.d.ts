import "node";

export interface CommitInfo {

}

export interface NodeInfo {

}

export interface NodeStatus {

}

export class Client {
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
    update(path: string[]): number[];

    get_working_copy_root(path: string): string;
}

export class AsyncClient {
    constructor();

    add_to_changelist(path: string | string[], changelist: string): Promise<void>;
    get_changelists(path: string, callback: (path: string, changelist: string) => void): Promise<void>;
    remove_from_changelists(path: string | string[]): Promise<void>;

    add(path: string): Promise<void>;
    cat(path: string): Promise<Buffer>;
    checkout(url: string, path: string): Promise<void>;
    commit(path: string | string[], message: string, callback: (info: CommitInfo) => void): Promise<void>;
    info(path: string, callback: (path: string, info: NodeInfo) => void): Promise<void>;
    remove(path: string | string[], callback: (info: CommitInfo) => void): Promise<void>;
    revert(path: string | string[]): Promise<void>;
    status(path: string, callback: (path: string, info: NodeStatus) => void): Promise<void>;
    update(path: string): Promise<number>;
    update(path: string[]): Promise<number[]>;

    get_working_copy_root(path: string): Promise<string>;
}

export enum Depth {
    unknown,
    empty,
    files,
    immediates,
    infinity,
}

export enum NodeKind {
    none,
    file,
    dir,
    unknown,
}

export enum RevisionKind {
    unspecified,
    number,
    date,
    committed,
    previous,
    base,
    working,
    head,
}

export enum WorkingCopyStatus {
    none,
    unversioned,
    normal,
    added,
    missing,
    deleted,
    replaced,
    modified,
    conflicted,
    ignored,
    obstructed,
    external,
    incomplete,
}
