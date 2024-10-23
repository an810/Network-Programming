void hazchem_decoder(char *str)
{   
    if (strlen(str) != 2 && strlen(str) != 3)
    {
        printf("Invalid HAZCHEM code\n");
        return;
    }
    char material[10], react[50], protection[100], containment[100], evacuation[50], color[5];
    switch (str[0])
    {
        case '1':
            strcpy(material, "Jets");
            break;
        case '2':
            strcpy(material, "Fog");
            break;
        case '3':
            strcpy(material, "Foam");
            break;
        case '4':
            strcpy(material, "Dry agent");
            break;
        default:
            strcpy(material, "Unknown");
            break;
    }
    switch (str[1])
    {
        case 'P': case 'R': case 'S': case 'T':
            strcpy(containment,"the dangerous goods may be washed down to a drain with a large quantity of water");
            break;
        case 'W': case 'X': case 'Y': case 'Z':
            strcpy(containment,"a need to avoid spillages from entering drains of water courses.");
            break;
        default:
            break;
    }
    switch (str[1]){
        case 'P': case 'S': case 'W': case 'Y': case 'Z':
            strcpy(react, "can be violently reactive");
            break;
        default:
            break;
    }
    switch (str[1])
    {
        case 'P': case 'R': case 'W': case 'X':
            strcpy(protection,"full protective clothing must be worn");
            break;
        case 'S': case 'T': case 'Y': case 'Z':
            printf("Is the character reversed coloured? ");
            scanf("%s",color);
            if(strcmp(color, "yes") == 0){
                strcpy(protection, "breathing apparatus, protective gloves for fire only.");
            }
            else{
                strcpy(protection, "breathing apparatus.");
            }
            break;
        default:
            break;
    }
    if (str[2] == 'E')
    {
        strcpy(evacuation, "Consider evacuation");
    }
    printf("***Emergency action advice***\n");
    printf("Material: %s\n", material);
    printf("Containment: %s\n", containment);
    printf("Reactivity: %s\n", react);
    printf("Protection: %s\n", protection);
    printf("Evacuation: %s\n", evacuation);
    printf("*****************************\n");    
}
