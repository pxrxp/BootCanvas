int isdigit(int c) {
    return (unsigned int)c - '0' < 10;
}

int isalpha(int c) {
    return ((unsigned int)c | 32) - 'a' < 26;
}

int isalnum(int c) {
    return isdigit(c) || isalpha(c);
}

int isspace(int c) {
    return c == ' ' || (unsigned int)c - '\t' < 5;
}

int isxdigit(int c) {
    return isdigit(c) || (((unsigned int)c | 32) - 'a' < 6);
}

int toupper(int c) {
    if ((unsigned int)c - 'a' < 26) {
        return c - 32;
    }
    return c;
}

int tolower(int c) {
    if ((unsigned int)c - 'A' < 26) {
        return c + 32;
    }
    return c;
}
