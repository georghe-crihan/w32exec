int fhandle;

main() {
char ch;

    printf("Hello!\n");
    fhandle = open("hello.c", 0);
    lseek(fhandle, 0L, 0);
    close(fhandle);
    ch = getch();
    printf("%c\n", ch);
    return 0;
}