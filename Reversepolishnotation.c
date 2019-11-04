#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TAILLE 11 
#define BASE 10 

int error = 0;

typedef struct stack
{
    int value;
    struct stack *before;
}stack_v;

stack_v* under_create(int value, stack_v* before)
{
    stack_v*  under = (stack_v*)malloc(sizeof(stack_v));
    if(under == NULL)
    {
        printf("Problème de mémoire en ajout dans la stack");
        exit(1);
    }
    under->value = value;
    under->before = before;
    return under;
}

void stack_free(stack_v* under) 
{
    if (under != NULL)
    {
        stack_free(under->before);
        free(under);
    }
}


void stack_aff(stack_v* under, int i) 
{
    if ( under != NULL )
    {
        stack_aff(under->before, i+1);
        printf("%d", under->value); 
        if ( i != 0 )
            printf(" ");
    }
}

stack_v* under_add(stack_v* stack, stack_v* under) 
{
    if ( stack == NULL ) 
    {
        under->before = NULL;
        return under;
    }
    under->before = stack; 
    return under;
}

int entier (char instruction[TAILLE])
{
    int i;
    for ( i = 0; i < TAILLE && instruction[i] != '\0'; i++ )
        if ( instruction[0] != '-' && !isdigit(instruction[i]) )
            return 0;
    return 1;
}

int conv_entier (char instruction[TAILLE])
{
    char *ptr;
    return strtol(instruction, &ptr, BASE);
}

int stack_top ( stack_v * back, stack_v* stack, int i )
{
    if ( back == NULL || stack == NULL ) 
    {
        error = 1;
        return 0;
    }
    if ( i == 1 )
    {
        stack_v* before = stack->before;
        int value = stack->value;
        free(stack);
        back->before = before;
        return value;
    }
    else
         return stack_top ( stack, stack->before, i-1 );
}

stack_v* stack_echange ( stack_v* stack )
{
    if ( stack == NULL ) 
    {
        return NULL;
    }
    if ( stack->before != NULL )
    {
        int ech = stack->value;
        stack->value = stack->before->value;
        stack->before->value = ech;
    }
    return stack;
}

stack_v* stack_dupe ( stack_v* under ) 
{
    if ( under == NULL )
    {
        return NULL;
    }
    return under_add(under, under_create(under->value, under));
}

stack_v* stack_suppr ( stack_v* under )
{
    stack_v* before = NULL;
    if ( under != NULL )
    {
        before = under->before;
        free(under);
    }
    return before;
}

stack_v* stack_add ( stack_v* under ) 
{
    if ( under == NULL )
        return NULL;
    if ( under->before == NULL ) 
    {
        error = 1;
        under = stack_suppr(under);
        return under;
    }
    int left = under->before->value, right = under->value;
    stack_v* before = under->before->before;
    free(under->before);
    free(under);
    return under_add(before, under_create(left+right, before));
}

stack_v* stack_soust ( stack_v* under ) 
{
    if ( under == NULL ) 
        return NULL;
    if ( under->before == NULL ) 
    {
        error = 1;
        under = stack_suppr(under);
        return under;
    }
    int left = under->before->value, right = under->value;
    stack_v* before = under->before->before;
    free(under->before);
    free(under);
    return under_add(before, under_create(left-right, before));
}

stack_v* stack_prod ( stack_v* under )
{
    if ( under == NULL )
        return NULL;
    if ( under->before == NULL )
    {
        error = 1;
        under = stack_suppr(under);
        return under;
    }
    int left = under->before->value, right = under->value;
    stack_v* before = under->before->before;
    free(under->before);
    free(under);
    return under_add(before, under_create(left*right, before));
}

stack_v* stack_div ( stack_v* under ) 
{
    if ( under == NULL )  
        return NULL;
    if ( under->before == NULL )
    {
        error = 1;
        under = stack_suppr(under);
        under = stack_suppr(under);
        return under;
    }
    int left = under->before->value, right = under->value;
    if ( right == 0 )
    {
        error = 1;
        under = stack_suppr(under);
        under = stack_suppr(under);
        return under;
    }
    stack_v* before = under->before->before;
    free(under->before);
    free(under);
    return under_add(before, under_create(left/right, before));
}

stack_v* stack_modulo ( stack_v* under ) 
{
    if ( under == NULL ) 
        return NULL;
    if ( under->before == NULL )
    {
        error = 1;
        free(under);
        return under;
    }
    int left = under->before->value, right = under->value;
    stack_v* before = under->before->before;
    free(under->before);
    free(under);
    return under_add(before, under_create(left%right, before));
}

stack_v* calculs ( stack_v* stack, char instruction[TAILLE] )
{
    if ( !strcmp("ROL", instruction) )
    {
        int n = stack->value;
        stack = stack_suppr(stack);
        n = stack_top(stack, stack->before, n-1);
        stack = under_add(stack, under_create(n, stack));
    }
    else if ( !strcmp("SWP", instruction) )
        stack = stack_echange(stack);
    else if ( !strcmp("DUP", instruction) )
        stack = stack_dupe(stack);
    else if ( !strcmp("POP", instruction) )
        stack = stack_suppr(stack);
    else if ( !strcmp("MOD", instruction) )
        stack = stack_modulo(stack);
    else if ( !strcmp("DIV", instruction) )
        stack = stack_div(stack);
    else if ( !strcmp("MUL", instruction) )
        stack = stack_prod(stack);
    else if ( !strcmp("SUB", instruction) )
        stack = stack_soust(stack);
    else if ( !strcmp("ADD", instruction) )
        stack = stack_add(stack);
    else
        error = 1;
    return stack;
}
int main()
{
    int N, i;
    stack_v* stack = NULL;
    scanf("%d", &N);
    for ( i = 0; i < N && error != 1; i++ )
    {
        char instruction[TAILLE];
        scanf("%s", instruction);
        if ( entier(instruction) )
            stack = under_add(stack, under_create(conv_entier(instruction), stack));
        else
            stack = calculs(stack, instruction);
    }
    stack_aff(stack, 0);
    if ( error )
    {
        if ( stack != NULL )
           { 
               printf(" ");
           }       
        printf("ERROR");
    }
    printf("\n");
    stack_free(stack);
    return 0;
}