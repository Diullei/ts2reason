declare interface Company {
    owner: Person;
    address: any;
    getOwnerFullName(): string;
}

declare interface Person {
    firstName: string;
    lastName: string;
    age: number;
}
