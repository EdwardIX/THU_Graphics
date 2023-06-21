#include<bits/stdc++.h>

int main() {

    char str[] = "f 12/312/312/3 123/3/23/2/31";
    int x;
    printf("%d\n",sscanf(str,"f %d/%d",&x, &x));
    printf("%d\n",x);
    printf("%s\n", str);

    return 0;
}