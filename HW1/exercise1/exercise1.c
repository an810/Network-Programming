#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "hazchem.h"

int main()
{
    char hazchem[10];
    printf("Enter HAZCHEM code: ");
    scanf("%s", hazchem);
    hazchem_decoder(hazchem);    
    return 0;
}