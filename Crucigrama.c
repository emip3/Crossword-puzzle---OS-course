#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define yChange 8
#define xChange 0
#define changeCrosswordTime 180

char *answer[6][5] = {
    {"hormiga",     "paraguas",     "caracol",  "cerradura",    "tarjeta"}, 
    {"rana",        "roca",         "reloj",    "rueda",        "rapido"},  
    {"paloma",      "zanahoria",    "ropa",     "pelota",       "polilla"}, 
    {"oceano",      "orden",        "oficina",  "oregano",      "ofrenda"},
    {"nutritivo",   "nata",         "noticia",  "nota",         "natural"},
    {"tiempo",      "taza",         "tortuga",  "toro",         "tamal"}
};

char *answerDescriptor[6][5] = {
    {   "Insecto pequeño que puede cargar entre 10 y 50 veces su peso",
        "Herramienta usada para taparse de la lluvia o el sol",
        "Animal que se mueve muuuuuuuuuuuuuuy lento. Usualmente tiene concha",
        "Objeto metálico que asegura puertas, ventanas u otros objetos",
        "La mayoría de los alumnos de la UDLAP la utilizan para pagar su comida"},
    
    {   "Anfibio (comúnmente verde) que puede saltar y prefiere lugares húmedos",
        "Material formado por minerales que se encuentra en el suelo",
        "Herramienta fabricada finamente para leer la hora",
        "Objeto circular que permite a objetos moverse",
        "Adjetivo que describe algo que se mueve a grandes velocidades"},
    
    {   "Animal volador utilizado en la antigüedad para enviar mensajes",
        "Alimento de color naranja que muchas personas detestan porque es una verdura",
        "Objeto cotidiano para ocultar las partes sensibles del cuerpo humano",
        "Objeto redondo utilizado en diferentes deportes",
        "Insecto asociado a la mala suerte que es atraído por la luz y suelta una especie de pelusa"},
    
    {   "Existen siete de estos al rededor del mundo y son hechos de agua",
        "La Ley y el...",
        "Dwight Schrute siempre quiso su propia...",
        "Hierba aromática que normalmente se añade al pozole",
        "Conjunto de elementos (como alimentos) que se ofrecen como señal de respeto o gratitud"},
    
    {   "Adjetivo -> que nutre",
        "Al calentar la leche, esta forma una película en su superficie",
        "Divulgación o publicación de un hecho",
        "Escrito breve que recuerda o avisa algo",
        "Adjetivo que describe algo de la naturaleza, relacionado o producida por ella"},
    
    {   "Hay que darle ... al ... (tiene que ver con relojes)",
        "Normalmente los vecinos reparten azúcar en este objeto",
        "Animal que puede ser marino o de tierra que tiene un caparazón duro",
        "Animal con cuernos que supuestamente le molesta el rojo",
        "Alimento típico mexicano a base de maíz que viene en diferentes presentaciones"}
};

char crossword[17][17];
char pCrossword[17][17];

int correct[6][5] = {
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}
};

int current = 0;
int updateFlag = 0;

sem_t semaphore;

int lastInChar(char *str, char letter);
int equals(char *str1, char *str2);

void printWholeCrossword(void)
{
    printf("\n");
    for(int i = 0; i < 17; i++)
    {
        for(int j = 0; j < 17; j++)
        {
            printf(" %c ", crossword[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printCrossword(void)
{
    printf("\n");
    for(int i = 0; i < 17; i++)
    {
        for(int j = 0; j < 17; j++)
        {
            printf(" %c ", pCrossword[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void printfQuesNAns(void)
{
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            printf("Pregunta:   %s\n", answerDescriptor[i][j]);
            printf("Respuesta:  %s\n", answer[i][j]);
        }
        printf("\n####MODIFICACIÓN REALIZADA#####\n");
    }
}

void printfLastInChar(int index, char letter)
{
    for(int i = 0; i < 5; i++)
    {
        printf("Indice de '%c' en la respuesta %d: %d\n", letter, index, lastInChar(answer[index][i], letter));
    }
    printf("\n");
}

void printlastInCharAll(void)
{
    printfLastInChar(0, 'r');
    printfLastInChar(0, 'a');
    printfLastInChar(2, 'o');
    printfLastInChar(3, 'n');
    printfLastInChar(4, 't');
}

void printQuestion(void)
{
    for(int i = 0; i < 6; i++)
    {
        printf("%d:  %s     || ", i + 1, answerDescriptor[i][current]);
        if (correct[i][current]) printf("%s\n", answer[i][current]);
        else printf("?????\n");
    }
}

void initCrossword(void)
{
    for(int i = 0; i < 17; i++)
    {
        for(int j = 0; j < 17; j++)
        {
            crossword[i][j] = '-';
            pCrossword[i][j] = '-';
        }
    }
}

int lastInChar(char *str, char letter)
{
    int lastIndex = -1;
    int i = 0;

    while (str[i] != '\0')
    {
        if (str[i] == letter)
        {
            lastIndex = i;
        }
        i++;
    }
    return lastIndex;
}

void updateCrossword(int fixed)
{
    initCrossword();
    srand(time(NULL));
    int rnd = rand() % 5;
    if (fixed < 5 && fixed > -1)
    {
        rnd = fixed;
    }

    int x = 0;
    int y = 0;
    int temp = 0;

    while (answer[0][rnd][temp] != '\0')
    {
        crossword[yChange][xChange + temp] = answer[0][rnd][temp];
        if (correct[0][rnd]) pCrossword[yChange][xChange + temp] = answer[0][rnd][temp];
        else pCrossword[yChange][xChange + temp] = '1';
        temp++;
    }

    temp = 0;
    x = lastInChar(answer[0][rnd], 'r');
    while (answer[1][rnd][temp] != '\0')
    {
        crossword[yChange + temp][xChange + x] = answer[1][rnd][temp];
        if (correct[1][rnd]) pCrossword[yChange + temp][xChange + x] = answer[1][rnd][temp];
        else pCrossword[yChange + temp][xChange + x] = '2';
        temp++;
    }

    temp = 0;
    x = lastInChar(answer[0][rnd], 'a');
    while (answer[2][rnd][temp] != '\0')
    {
        temp++;
    }

    for(int len = 0; len < temp; len++)
    {
        crossword[yChange - temp + len + 1][xChange + x] = answer[2][rnd][len];
        if (correct[2][rnd])
        {
            pCrossword[yChange - temp + len + 1][xChange + x] = answer[2][rnd][len];
        }
        else
        {
            pCrossword[yChange - temp + len + 1][xChange + x] = '3';
        }
    }

    y = temp - lastInChar(answer[2][rnd], 'o') - 1;

    temp = 0;
    while (answer[3][rnd][temp] != '\0')
    {
        crossword[yChange - y][xChange + x + temp] = answer[3][rnd][temp];
        if (correct[3][rnd])
        {
            pCrossword[yChange - y][xChange + x + temp] = answer[3][rnd][temp];
        }
        else
        {
            pCrossword[yChange - y][xChange + x + temp] = '4';
        }
        temp++;
    }

    temp = 0;
    x += lastInChar(answer[3][rnd], 'n');
    while (answer[4][rnd][temp] != '\0')
    {
        temp++;
    }

    for(int len = 0; len < temp; len++)
    {
        crossword[yChange - y + len][xChange + x] = answer[4][rnd][len];
        if (correct[4][rnd])
        {
            pCrossword[yChange - y + len][xChange + x] = answer[4][rnd][len];
        }
        else
        {
            pCrossword[yChange - y + len][xChange + x] = '5';
        }
    }

    temp = 0;
    y = lastInChar(answer[4][rnd], 't') - y;

    while (answer[5][rnd][temp] != '\0')
    {
        crossword[yChange + y][xChange + x + temp] = answer[5][rnd][temp];
        if (correct[5][rnd])
        {
            pCrossword[yChange + y][xChange + x + temp] = answer[5][rnd][temp];
        }
        else
        {
            pCrossword[yChange + y][xChange + x + temp] = '6';
        }
        temp++;
    }
    current = rnd;
}

int reviewAnswer(char *input, int res)
{
    correct[res][current] = equals(input, answer[res][current]);
    if (correct[res][current])
    {
        updateCrossword(current);
    }
    return correct[res][current];
}

int equals(char *str1, char *str2)
{
    while (*str1 != '\0' || *str2 != '\0')
    {
        if (*str1 != *str2)
        {
            return 0;
        }
        str1++;
        str2++;
    }
    return 1;
}

void clearScreen(void)
{
    for(int i = 0; i < 100; i++)
    {
        printf("\n");
    }
}

int checkIfWin(void)
{
    int out = 0;
    for (int i = 0; i < 5; i++)
    {
        out = 1;
        for (int j = 0; j < 6; j++)
        {
            if (!correct[j][i])
            {
                out = 0;
                break;
            }
        }
        if (out) return out;
    }
    return out;
}

void* timerFunction(void* argh)
{
    while(1)
    {
        for(int count = 0; count < changeCrosswordTime; count++)
        {
            sleep(1);
        }
        sem_wait(&semaphore);
        updateFlag = 1;
        sem_post(&semaphore);
    }
}

void update(int sig)
{
    updateCrossword(-1);
    printf("\n EL CRUCIGRAMA HA CAMBIADO \n");
}


void play(void)
{
    int option;
    char inputAnswer[20];

    clearScreen();

    if (updateFlag)
    {
        alarm(1);
        pause();
    }
    updateFlag = 0;

    printCrossword();
    printQuestion();

    for (;;)
    {
        printf("\nSeleccione una pregunta: ");
        scanf("%d", &option);

        if (option < 7 && option > 0)
        {
            option--;
            if (correct[option][current])
            {
                printf("\n  Ya ingresó la respuesta.");
                printf("\n  Elige otra pregunta, por favor.\n");
                continue;
            }

            printf("\nEscribe tu respuesta: ");
            scanf("%s", inputAnswer);

            if (reviewAnswer(inputAnswer, option))
            {
                printf("\n !!! Correcto !!! \n\n");
            }
            else
            {
                printf("\n !!! Incorrecto !!! \n\n");
            }
            printf("Presione cualquier tecla para continuar: ");
            scanf("%s", inputAnswer);
            break;
        }
        else
        {
            printf("\nNo existe la pregunta\n");
        }
    }
}

void updateCrosswordAndPlay(void)
{
    updateCrossword(-1);
    play();
}


int main(int argc, const char * argv[])
{
    sem_init(&semaphore, 0, 1);
    pid_t pid;
    int status;

    pid = fork();
    if(pid == 0)
    {
        pthread_t timer;

     pthread_create(&timer, NULL, &timerFunction, NULL);

        signal(SIGALRM, update);

        printf("Bienvenido/a a 'LA CASA DE LAS HOJAS'\n");
        printf("Intenta contestar las preguntas para completar el crucigrama.\n");
        printf("Tendrás 3 minutos para completarlo, si te pasas, EL CRUCIGRAMA CAMBIARÁ.\n");
        printf("Recuerda que siempre puedes pausar el juego usando CTRL+C\n");

        int playing = 0;
        printf("\nEscribe '1' para empezar el juego: ");
        scanf("%d", &playing);

        while (playing)
        {
            initCrossword();
            updateCrossword(-1);
            while(!checkIfWin()) play();
            clearScreen();
            printf(" !!! GANASTE !!!\n\n !!! Bien hecho !!!\n\n");
            printf("\nEscribe '1' para volver a jugar, o cualquier otro número para salir.: ");
            scanf("%d", &playing);
        }
        return 0;
    }

    waitpid(pid, &status, 0);

    sem_destroy(&semaphore);

    return 0;
}