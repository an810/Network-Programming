#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct student{
    char std_id[15];
    char surname[15];
    char name[15];
    float progress, final;
    char grade;
    float avg;
    struct student *next;
}student;

typedef struct{
    char subject_id[15];
    char subject_name[15];
    int percentage;
    char semester[20];
    int count;
    student *head;
}subject;

student* makeStudent(char std_id[15], char surname[15], char name[15], float progress, float final){
    student *s = (student*)malloc(sizeof(student));
    strcpy(s->std_id, std_id);
    strcpy(s->surname, surname);
    strcpy(s->name, name);
    s->progress = progress;
    s->final = final;
    s->next = NULL;
    return s;
}

void addScoreBoard(subject s[], int n, int m){
    for(int i = n; i < n+m; i++){
        printf("Enter the subject ID, subject and number of student : ");
        scanf("%s %s %d",s[i].subject_id,s[i].subject_name, &s[i].count);//Chú ý dấu &
        fseek(stdin,0,SEEK_END);
        s[i].subject_id[strlen(s[i].subject_id)-1] = '\0';
        printf("Enter the semester: ");
        scanf("%s",s[i].semester);
        s[i].semester[strlen(s[i].semester)-1] = '\0';
        s[i].head = NULL;
        }
}

void addStudent(subject s[], int n){
    char subject_id[20], semester[20];
    char std_id[15], surname[15], name[15];
    float progress, final;
    printf("Enter the subject id and semester: \n");
    scanf("%s %s",subject_id, semester);
    subject_id[strlen(subject_id)-1] = '\0';
    semester[strlen(semester)-1] = '\0';
    for(int i = 0; i < n; i++){
        if(strcmp(subject_id, s[i].subject_id) == 0 && strcmp(semester, s[i].semester) == 0){
            student *tmp = s[i].head;
            while(tmp != NULL){
                tmp = tmp->next;
            }
            for(int i = 0; i < s[i].count; i++){
                printf("Enter the student id: ");
                scanf("%s", std_id);
                printf("Enter the name of the student: ");
                scanf("%s %s",surname, name);
                printf("Enter the progress point and final point: ");
                scanf("%f %f",&progress, &final);
                tmp = makeStudent(std_id, surname, name, progress, final);
                tmp->avg = progress * (s[i].percentage/100) + final * (1 - s[i].percentage/100);
                if(tmp->avg >= 8.5 && tmp->avg <=10){
                    tmp->grade = 'A';
                }
                else if (tmp->avg >= 7.0 && tmp->avg <=8.4)
                {
                    tmp->grade = 'B';
                }
                else if(tmp->avg >= 5.5 && tmp->avg < 7.0){
                    tmp->grade = 'C';
                }
                else if(tmp->avg >= 4.0 && tmp->avg <5.5){
                    tmp->grade = 'D';
                }
                else{
                    tmp->grade = 'F';
                }
                tmp = tmp->next;
            }
        }
    }
}

void searchStudent(subject s[], int n){
    char subject_id[15], semester[15], surname[20], name[20];
    printf("Enter the subject id and the semester id: ");
    scanf("%s %s",subject_id, semester);
    for(int  i = 0; i < n ; i++){
        if(strcmp(s[i].subject_id , subject_id) == 0 && strcmp(s[i].semester, semester) == 0){
            printf("Enter the name of the student: ");
            scanf("%s %s", surname, name);
            student *tmp = s[i].head;
            while(tmp != NULL){
                if(strcmp(tmp->surname, surname) != 0 || strcmp(tmp->name, name) != 0){
                    printf("S|%-10s |%-15s| %-10s|%-5f|%-5f|%-5c|\n", tmp->std_id, tmp->surname, tmp->name, tmp->progress, tmp->final, tmp->grade);
                    break;
                }
            }
        }

    }
}

void deleteStudent(subject s[], int n){
    char subject_id[15], semester[15], surname[20], name[20];
    printf("Enter the subject id and the semester id: ");
    scanf("%s %s",subject_id, semester);
    for(int  i = 0; i < n ; i++){
        if(strcmp(s[i].subject_id , subject_id) == 0 && strcmp(s[i].semester, semester) == 0){
            printf("Enter the name of the student: ");
            scanf("%s %s", surname, name);
            student *tmp = s[i].head;
            if(strcmp(tmp->surname, surname)==0 && strcmp(tmp->name, name) == 0){
                s[i].head = s[i].head->next;
                free(tmp);
            }
            else{
                while(strcmp(tmp->next->surname, surname) != 0 || strcmp(tmp->next->name, name) != 0){
                    tmp = tmp->next;
                }
                student *p = tmp->next;
                tmp->next = p->next;
                free(p);
            }
        }
    }
}

void show(subject s[], int n){
    for(int  i = 0; i < n; i++){
        int a,b,c,d,f;
        a = b = c = d = f = 0;
        int total = 0;
        printf("\n\n-----SCORE BROAD-----\n");
        printf("Subject ID| %s\n",s[i].subject_id);
        printf("Subject| %s\n", s[i].subject_name);
        printf("F| %.0d|%.0d\n", s[i].percentage, 1-s[i].percentage);
        printf("Semester| %s\n",s[i].semester);
        printf("Student Count| %d\n",s[i].count);
        student *tmp = s[i].head;
        student *max = tmp;
        student *min = tmp;
        while(tmp != NULL){
            printf("S|%-10s |%-15s| %-10s|%-5f|%-5f|%-5c|\n", tmp->std_id, tmp->surname, tmp->name, tmp->progress, tmp->final, tmp->grade);
            total += tmp->avg;
            if(tmp->avg > max->avg){
                max = tmp;
            }
            if(tmp->avg < min->avg){
                min = tmp;
            }
            switch(tmp->grade){
                case 'A':
                   a += 1;
                   break;
                case 'B':
                   b += 1;
                   break;
                case 'C':
                   c += 1;
                   break;
                case 'D':
                   d += 1;
                   break;
                case 'F':
                   f += 1;
                   break;
                default:
                   break;
            }
            tmp = tmp->next;
        }
        printf("\n\n-----SCORE REPORT-----\n");
        printf("%s %s",s[i].subject_id, s[i].semester);
        printf("The student with the highest mark is: %s\n", max->name);
        printf("The student with the lowest mark is: %s\n", min->name);
        printf("A histogram of %s is: \n", s[i].subject_id);
        printf("A: ");
        for(int k = 0; k < a; k++){
            printf("*");
        }
        printf("\nB: ");
        for(int k = 0; k < b; k++){
            printf("*");
        }
        printf("\nC: ");
        for(int k = 0; k < c; k++){
            printf("*");
        }
        printf("\nD: ");
        for(int  k = 0; k < d; k++){
            printf("*");
        }
        printf("\nF: ");
        for(int  k = 0; k < f; k++){
            printf("*");
        }
    }

}

int main(){
    int n = 0;
    int m;
    subject s[100];
    int choice;
    do{
        printf("LEARNING MANAGEMENT SYSTEM\n");
        printf("----------------------------------\n");
        printf("1. Add a new score band\n");
        printf("2. Add score\n");
        printf("3. Remove score\n");
        printf("4. Search score\n");
        printf("5. Display score board and score report\n");
        printf("6. Quit\n");
        printf("Your choice (1-5, other to quit): \n");
        scanf("%d",&choice);
        switch (choice)
        {
        case 1:
            printf("Enter the number of subject: ");
            scanf("%d",&m);
            addScoreBoard(s,n,m);
            n = n + m;
            break;
        case 2:
            addStudent(s,n);
            break;
        case 3:
            deleteStudent(s,n);
            break;
        case 4:
            searchStudent(s,n);
            break;
        case 5:
            show(s,n);
            break;
        default:
            break;
        }
    }while(choice != 6);
    return 0;
}