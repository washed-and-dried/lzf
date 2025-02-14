#include <unistd.h>

int main() {
    write(1, "yes\0", 4);
    return 0;
}
