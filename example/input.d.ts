// Type definitions for autoprefixer 9.1
// Project: https://github.com/postcss/autoprefixer
// Definitions by:  Armando Meziat <https://github.com/odnamrataizem>, murt <https://github.com/murt>
// Definitions: https://github.com/DefinitelyTyped/DefinitelyTyped
// TypeScript Version: 2.2

declare module 'numbers' {
    export var x: number


    export function add(a: number, b: number): number;
    module array_math {
        export function sum(nums: number[]): number
        export function sum2(nums: [number, number]): number
    }
}

declare namespace autoprefixer {
    interface Options {
        browsers?: string[] | string;
        env?: string;
        cascade?: boolean;
        add?: boolean;
        remove?: boolean;
        supports?: boolean;
        flexbox?: boolean | 'no-2009';
        grid?: boolean;
        stats?: any;
        ignoreUnknownVersions?: boolean;

        myMethod(a: string, b: boolean): string;
    }

    type Autoprefixer = any;
}

declare const autoprefixer: autoprefixer.Autoprefixer;
export = autoprefixer;
