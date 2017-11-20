import "node";

export interface CommitInfo {

}

export interface NodeInfo {
    path: string;
}

export interface NodeStatus {
    path: string;
    kind: NodeKind;
    node_status: StatusKind;
    text_status: StatusKind;
    prop_status: StatusKind;
    versioned: boolean;
    changelist?: string;
}

export type Revision = RevisionKind.base |
    RevisionKind.committed |
    RevisionKind.head |
    RevisionKind.previous |
    RevisionKind.unspecified |
    RevisionKind.working |
    { number: number } |
    { date: number };

export interface GetChangelistsOptions {
    changelists: string | string[];
    depth: Depth;
}

export interface InfoOptions {
    peg_revision: Revision;
    revision: Revision;
    depth: Depth;
}

type CredentialProviderResult<T> =
    undefined |
    T |
    Promise<undefined> |
    Promise<T>

interface SimpleCredential {
    username: string;
    password: string;
    save: boolean;
}

interface SimpleCredentialProvider {
    provide_simple_provider(realm: string, username?: string): CredentialProviderResult<SimpleCredential>;
}

type InfoCallback = (info: NodeInfo) => void;

export class Client {
    constructor();

    add_to_changelist(path: string | string[], changelist: string): void;
    get_changelists(path: string, callback: (path: string, changelist: string) => void, options?: Partial<GetChangelistsOptions>): void;
    remove_from_changelists(path: string | string[]): void;

    add(path: string): void;
    cat(path: string): Buffer;
    checkout(url: string, path: string): void;
    commit(path: string | string[], message: string, callback: (info: CommitInfo) => void): void;

    info(path: string, callback: InfoCallback): void;
    info(path: string, options: Partial<InfoOptions> | undefined, callback: InfoCallback): void;

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
    get_changelists(path: string, callback: (path: string, changelist: string) => void, options?: Partial<GetChangelistsOptions>): Promise<void>;
    remove_from_changelists(path: string | string[]): Promise<void>;

    add(path: string): Promise<void>;
    cat(path: string): Promise<Buffer>;
    checkout(url: string, path: string): Promise<void>;
    commit(path: string | string[], message: string, callback: (info: CommitInfo) => void): Promise<void>;

    info(path: string, callback: InfoCallback): Promise<void>;
    info(path: string, options: Partial<InfoOptions> | undefined, callback: InfoCallback): Promise<void>;

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
    unspecified = 0,
    number = 1,
    date = 2,
    committed = 3,
    previous = 4,
    base = 5,
    working = 6,
    head = 7,
}

export enum StatusKind {
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
