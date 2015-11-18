#include <string>

int func(int a)
{
    int c;
    int * d;
    int e[1];
    int f = 1;
    int g = 0, h = 0;
    int i, j = 8;
    int *k, l = 98;
    int *m = malloc(sizeof(int)), n;
    char* o = "asdf";
    char p = 'q';
    bool q,s = false;
    char* r = 'asdf';

    // Estrutura DO
    do {
        i = 10*(3+2);
    } while (!q);

    // Estrutura WHILE
    while (q && !(s) || q != 0)
    {
        l = 0;
        j = 0;
    }

    // Estrutura Switch e IF
    switch (g)
    {
        case 1:
        case 2:
            r = "qwer";
            if (45 == 45)
            {
                p = 'y';
            }
        case 3:
            r = "tyui";
            if (46 == 46)
            {
                p = 'w';
            }
            else if (50 > 23)
            {
                p = 'n';
            }
            else
            {
                p = 'o';
            }
        default:
            r = "default";
    }

    // Estrutura FOR
    for (int u = 0; u < 10; u++)
    {
        glob += h+1;
    }
    for (;;)
    {
        glob2 += h+1;
    }

    return c;
}


void fx()
{
    int a,b,c,d,e,f,r;
    char g,h,i,j,k,l,m,n,o,p,q;

    g = '1';
    glob = 'q';

    func(r);
}