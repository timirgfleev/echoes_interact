#include <cassert>
#include "../receiver/launcher.h"

void listen_for(unsigned short a, unsigned long b)
{
    assert(a == 1234);
    assert(b == 3);
}


void test_launcher()
{
    const char *argv1[] = {"program", "-r"};
    launcher(2, argv1);

    const char *argv2[] = {"program", "-p", "1234"};
    launcher(3, argv2);

    const char *argv3[] = {"program", "-e", "3"};
    launcher(3, argv3);

    const char *argv4[] = {"program", "-p", "1234", "-e", "3"};
    launcher(5, argv4);

    const char *argv5[] = {"program", "-e", "3", "-p", "1234"};
    launcher(5, argv5);
}

int main()
{
    test_launcher();
    return 0;
}