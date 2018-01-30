import "node";

export interface CommitInfo {
    author: string;
    date: string;
    repos_root: string;
    revision: number;
    post_commit_error: string | undefined;
}

export interface NodeInfo {
    path: string;
    kind: NodeKind;
    last_changed_author: string;
    last_changed_date: number | string;
    last_changed_rev: number;
    repos_root_url: string;
    url: string;
}

export interface NodeStatus {
    path: string;
    changed_author: string;
    changed_date: number | string;
    changed_rev: number;
    conflicted: boolean;
    copied: boolean;
    depth: Depth;
    file_external: boolean;
    kind: NodeKind;
    node_status: StatusKind;
    prop_status: StatusKind;
    revision: number;
    text_status: StatusKind;
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

export interface DepthOption {
    /** The depth of the operation. */
    depth: Depth;
}

export interface ChangelistsOption {
    changelists: string | string[];
}

export type AddToChangelistOptions = DepthOption & ChangelistsOption;

export type GetChangelistsOptions = DepthOption & ChangelistsOption;

export type RemoveFromChangelistsOptions = DepthOption & ChangelistsOption;

export type AddOptions = DepthOption;

export interface RevisionOption {
    /** The operative revision. */
    revision: Revision;
}

export interface PegRevisionOpitons extends RevisionOption {
    /** The peg revision. */
    peg_revision: Revision;
}

export type CatOptions = PegRevisionOpitons;

export interface CatResult {
    content: Buffer;
    properties: { [key: string]: string };
}

export type CheckoutOptions = DepthOption & PegRevisionOpitons;

export type InfoOptions = DepthOption & PegRevisionOpitons;

export type StatusOptions = DepthOption & RevisionOption & {
    /** If true, don't process externals definitions as part of this operation. */
    ignore_externals: boolean;
};

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

type GetChangelistCallback = (path: string, changelist: string) => void;
type InfoCallback = (info: Readonly<NodeInfo>) => void;
type StatusCallback = (status: Readonly<NodeStatus>) => void;
type CommitCallback = (info: Readonly<CommitInfo>) => void;

export class Client {
    constructor();

    add_to_changelist(path: string | string[], changelist: string, options?: Partial<AddToChangelistOptions>): void;

    get_changelists(path: string, callback: GetChangelistCallback): void;
    get_changelists(path: string, options: Partial<GetChangelistsOptions> | undefined, callback: GetChangelistCallback): void;

    remove_from_changelists(path: string | string[], options?: Partial<RemoveFromChangelistsOptions>): void;

    add(path: string, options?: Partial<AddOptions>): void;
    cat(path: string, options?: Partial<CatOptions>): CatResult;
    /**
     * Check out a working copy from a repository.
     *
     * @param url The repository URL of the checkout source.
     * @param path The root of the new working copy.
     * @param options The options of the checkout.
     *
     * @returns The value of the revision checked out from the repository.
     */
    checkout(url: string, path: string, options?: Partial<CheckoutOptions>): number;
    commit(path: string | string[], message: string, callback: CommitCallback): void;

    info(path: string, callback: InfoCallback): void;
    info(path: string, options: Partial<InfoOptions> | undefined, callback: InfoCallback): void;

    remove(path: string | string[], callback: CommitCallback): void;
    resolve(path: string): void;
    revert(path: string | string[]): void;

    status(path: string, callback: StatusCallback): void;
    status(path: string, options: Partial<StatusOptions> | undefined, callback: StatusCallback): void;

    update(path: string): number;
    update(path: string[]): number[];

    get_working_copy_root(path: string): string;
}

export class AsyncClient {
    constructor();

    add_to_changelist(path: string | string[], changelist: string, options?: Partial<AddToChangelistOptions>): Promise<void>;

    get_changelists(path: string, callback: GetChangelistCallback): Promise<void>;
    get_changelists(path: string, options: Partial<GetChangelistsOptions> | undefined, callback: GetChangelistCallback): Promise<void>;

    remove_from_changelists(path: string | string[], options?: Partial<RemoveFromChangelistsOptions>): Promise<void>;

    add(path: string, options?: Partial<AddOptions>): Promise<void>;
    cat(path: string, options?: Partial<CatOptions>): Promise<CatResult>;
    /**
     * Check out a working copy from a repository.
     *
     * @param url The repository URL of the checkout source.
     * @param path The root of the new working copy.
     * @param options The options of the checkout.
     *
     * @returns The value of the revision checked out from the repository.
     */
    checkout(url: string, path: string, options?: Partial<CheckoutOptions>): Promise<number>;
    commit(path: string | string[], message: string, callback: CommitCallback): Promise<void>;

    info(path: string, callback: InfoCallback): Promise<void>;
    info(path: string, options: Partial<InfoOptions> | undefined, callback: InfoCallback): Promise<void>;

    remove(path: string | string[], callback: CommitCallback): Promise<void>;
    resolve(path: string): Promise<void>;
    revert(path: string | string[]): Promise<void>;

    status(path: string, callback: StatusCallback): Promise<void>;
    status(path: string, options: Partial<StatusOptions> | undefined, callback: StatusCallback): Promise<void>;

    update(path: string): Promise<number>;
    update(path: string[]): Promise<number[]>;

    get_working_copy_root(path: string): Promise<string>;
}

export enum ConflictChoose {
    postpone,
    base,
    theirs_full,
    mine_full,
    theirs_conflict,
    mine_conflict,
    merged,
    unspecified,
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
    /** No revision information given */
    unspecified = 0,
    /** revision given as number */
    number = 1,
    /** revision given as date */
    date = 2,
    /** rev of most recent change */
    committed = 3,
    /** (rev of most recent change) - 1 */
    previous = 4,
    /** .svn/entries current revision */
    base = 5,
    /** current, plus local mods */
    working = 6,
    /** repository youngest */
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
