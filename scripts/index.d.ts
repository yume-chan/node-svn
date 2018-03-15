import "node";

export interface CommitItem {
    author: string;
    date: string;
    repos_root: string;
    revision: number;
    post_commit_error: string | undefined;
}

export interface InfoItem {
    path: string;
    kind: NodeKind;
    last_changed_author: string;
    last_changed_date: number | string;
    last_changed_rev: number;
    repos_root_url: string;
    url: string;
}

export interface StatusItem {
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

interface SimpleAuth {
    username: string;
    password: string;
    may_save: boolean;
}

interface GetChangelistsItem {
    path: string;
    changelist: string;
}

interface BlameOptions extends PegRevisionOpitons {
    start_revision: Revision;
    end_revision: Revision;
}

interface BlameItem {
    start_revision: number;
    end_revision: number;
    line_number: number;
    revision: number | undefined;
    merged_revision: number | undefined;
    merged_path: string | undefined;
    line: string;
    local_change: boolean;
}

interface RevisionRange {
    start: Revision;
    end: Revision;
}

interface LogOptions extends PegRevisionOpitons {
    revision_ranges: RevisionRange | RevisionRange[];
    limit: number;
}

interface LogItem {
    revision: number;
    non_inheritable: boolean;
    subtractive_merge: boolean;
    author: string | undefined;
    date: string | undefined;
    message: string | undefined;
}

type AuthProviderResult<T> = undefined | T | Promise<undefined | T>
type SimpleAuthProvider = (realm: string, username: string | undefined, may_save: boolean) => AuthProviderResult<SimpleAuth>;

export class Client {
    constructor(config_path?: string);

    add_simple_auth_provider(provider: SimpleAuthProvider): void;
    remove_simple_auth_provider(provider: SimpleAuthProvider): void;

    add_to_changelist(path: string | string[], changelist: string, options?: Partial<AddToChangelistOptions>): Promise<void>;
    get_changelists(path: string, options?: Partial<GetChangelistsOptions>): AsyncIterable<GetChangelistsItem>;
    remove_from_changelists(path: string | string[], options?: Partial<RemoveFromChangelistsOptions>): Promise<void>;

    /**
     * Schedule a working copy path for addition to the repository.
     */
    add(path: string, options?: Partial<AddOptions>): Promise<void>;
    blame(path: string, options?: Partial<BlameOptions>): AsyncIterable<BlameItem>;
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
    cleanup(path: string): Promise<void>;
    commit(path: string | string[], message: string): AsyncIterable<CommitItem>;

    info(path: string, options?: Partial<InfoOptions>): AsyncIterable<InfoItem>;
    log(path: string | string[], options?: Partial<LogOptions>): AsyncIterable<LogItem>;

    remove(path: string | string[]): AsyncIterable<CommitItem>;
    resolve(path: string): Promise<void>;
    revert(path: string | string[]): Promise<void>;

    status(path: string, options?: Partial<StatusOptions>): AsyncIterator<StatusItem>;

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

export function create_repos(path: string): void;
