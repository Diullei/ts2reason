declare module "myModule" {
    interface Person {
        firstName: string;
        lastName: string;
        age: number;
        giveMeSomeNumber(): number;
    }
}

declare interface Setting {
    value1: string;
    value2: number;
}
