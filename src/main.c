#include <stdio.h>

#include "info.h"

void main() {
    char* hostname = get_hostname();
    char* username = get_username();

    printf("%s@%s\n", username, hostname);
}