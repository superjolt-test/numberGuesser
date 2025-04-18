#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <palette.h>  // A color palette lib I made (https://github.com/superjolt-test/palette.h)

#define MAX_ENTRIES 1000

#ifdef _WIN32
    #define CLEAR_SCREEN "cls"
    #include <windows.h>
    #define getpid GetCurrentProcessId
#else
    #include <unistd.h>
    #define CLEAR_SCREEN "clear"
#endif

#define UP "\033[F"

void menu();
void setup();
void difficultyMenu();
void instructions();
void debug();
void game();
void leaderboard();
void showLeaderboard(int currentGameID);
int compareScores(const void *a, const void *b);

int playerID;
int secretNumber;
int difficulty;
int maxNumber;
int guess = 0;
int moves = 0;

double multiplier = 0;
int timeTaken;
float score;

char name[25];
char input;

typedef struct {
    int id;
    char name[25];
    int moves;
    long time;
    int range;
    float score;
} Entry;

int main()
{
    playerID = getpid();

    do {
        menu();
        setup();
        instructions();
        game();
        leaderboard();

        printf(ROSE "\nWanna play again? (Y/N) " RESET);
        scanf(" %c", &input);
    } while (input == 'Y' || input == 'y');

    return 0;
}

void menu()
{
    system(CLEAR_SCREEN);

    printf(DEFAULT "\n");
    printf(BOLD FG_BRIGHT_CYAN);

    printf("\n    ██████   █████                         █████                       \n");
    printf("   ░░██████ ░░███                         ░░███                         \n");
    printf("    ░███░███ ░███ █████ █████████████████  ░███████   ██████  ████████ \n");
    printf("    ░███░░███░███░░███ ░███░░███░░███░░███ ░███░░███ ███░░███░░███░░███\n");
    printf("    ░███ ░░██████ ░███ ░███ ░███ ░███ ░███ ░███ ░███░███████  ░███ ░░░ \n");
    printf("    ░███  ░░█████ ░███ ░███ ░███ ░███ ░███ ░███ ░███░███░░░   ░███     \n");
    printf("    █████  ░░█████░░█████████████░███ █████████████ ░░██████  █████    \n");
    printf("   ░░░░░    ░░░░░  ░░░░░░░░░░░░░ ░░░ ░░░░░░░░░░░░░   ░░░░░░  ░░░░░     \n");
    printf("\n\n\n");
    printf("    █████████                                                         \n");
    printf("   ███░░░░░███                                                        \n");
    printf("  ███     ░░░ █████ ████  ██████  █████  █████   ██████  ████████    \n");
    printf(" ░███        ░░███ ░███  ███░░██████░░  ███░░   ███░░███░░███░░███   \n");
    printf(" ░███    █████░███ ░███ ░███████░░█████░░█████ ░███████  ░███ ░░░    \n");
    printf(" ░░███  ░░███ ░███ ░███ ░███░░░  ░░░░███░░░░███░███░░░   ░███        \n");
    printf("  ░░█████████ ░░████████░░██████ ██████ ██████ ░░██████  █████       \n");
    printf("   ░░░░░░░░░   ░░░░░░░░  ░░░░░░ ░░░░░░ ░░░░░░   ░░░░░░  ░░░░░         \n");
    
    printf(RESET "\n\n");

    printf(BOLD "What's your name? (No spaces) " RESET);
    scanf("%24s", name);

    difficultyMenu();
}


void difficultyMenu()
{
    printf("\n" COOL_ICE "Select your difficulty:\n\n" RESET);

    printf(FG_GREEN BOLD " [1] Easy     " RESET FG_WHITE "- Range 1 to 50\n");
    printf(FG_YELLOW BOLD " [2] Medium   " RESET FG_WHITE "- Range 1 to 100\n");
    printf(FG_RED BOLD " [3] Hard     " RESET FG_WHITE "- Range 1 to 500\n");
    printf(FG_MAGENTA BOLD " [#] Custom   " RESET FG_WHITE "- Enter any upper limit\n\n");

    printf(SHADOW "Your choice: " RESET);
    scanf("%d", &difficulty);

    if (difficulty == 1) maxNumber = 50;
    else if (difficulty == 2) maxNumber = 100;
    else if (difficulty == 3) maxNumber = 500;
    else maxNumber = difficulty;

    multiplier = maxNumber / 100.0;

    printf("\n" FG_GREEN "Great! I'll think of a number from 1 to %d...\n\n" RESET, maxNumber);
}

void setup()
{
    srand(time(NULL));
    secretNumber = (rand() % maxNumber) + 1;
}

void game()
{
    time_t start = time(NULL);
    guess = 0;
    moves = 0;

    printf(OCEAN "Start guessing!\n\n" RESET);

    while (guess != secretNumber)
    {
        printf(FG_CYAN "Guess: " RESET);

        scanf("%d", &guess);
        if (scanf("%d", &guess) != 1) {
            printf(ALERT "Invalid input! Please enter a number.\n" RESET);
            while (getchar() != '\n'); // Clear the buffer
            continue;
        }
        
        if (guess < secretNumber)
            printf(FOREST "📉 Too small!\n" RESET);
        else if (guess > secretNumber)
            printf(FIRE "📈 Too big!\n" RESET);

        moves++;
    }

    time_t end = time(NULL);
    timeTaken = (int)(end - start);

    printf("\n" ALERT "🎉 You guessed it in %d moves, taking %d seconds!\n" RESET, moves, timeTaken);
}

void leaderboard()
{
    FILE *fp = fopen("leaderboard.txt", "a");

    if (timeTaken == 0) timeTaken = 1; // If this was not there, in the bottom if timeTaken = 0, then you're dividing by 0, which = infinity
    score = ((float)moves / (float)timeTaken) * multiplier;

    fprintf(fp, "%-6d %-25s %-6d %-8ld %-7d %.3f\n", playerID, name, moves, timeTaken, maxNumber, score);
    fclose(fp);

    showLeaderboard(playerID);
}

int compareScores(const void *a, const void *b) {
    const Entry *entryA = (const Entry *)a;
    const Entry *entryB = (const Entry *)b;

    if (entryA->score < entryB->score) return 1;  // descending
    if (entryA->score > entryB->score) return -1;
    return 0;
}

void showLeaderboard(int currentGameID)
{
    FILE *fp = fopen("leaderboard.txt", "r");
    if (fp == NULL)
    {
        printf(ALERT "No leaderboard yet!\n" RESET);
        return;
    }

    Entry entries[MAX_ENTRIES];
    int count = 0;

    while (fscanf(fp, "%d %s %d %ld %d %f",
                  &entries[count].id,
                  entries[count].name,
                  &entries[count].moves,
                  &entries[count].time,
                  &entries[count].range,
                  &entries[count].score) == 6)
    {
        count++;
        if (count >= MAX_ENTRIES) break;
    }

    fclose(fp);

    qsort(entries, count, sizeof(Entry), compareScores); // Sort

    printf(BOLD "\n🏆 LEADERBOARD 🏆\n" RESET);
    printf(UNDERLINE "ID    Name                     Moves  Time     Range  Score\n" RESET);
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < count; i++)
    {
        if (entries[i].id == currentGameID)
            printf(ROSE); // highlight current player

        printf("%-6d %-25s %-6d %-8ld %-7d %.3f\n",
               entries[i].id,
               entries[i].name,
               entries[i].moves,
               entries[i].time,
               entries[i].range,
               entries[i].score);

        if (entries[i].id == currentGameID)
            printf(RESET);
    }
}

void instructions() {

    printf(NEON_NIGHT "\n🧠 INSTRUCTIONS:\n" RESET);
    printf(FG_WHITE "I have a secret number between 1 and %d.\n", maxNumber);
    printf("You need to guess it!\n");
    printf("I'll tell you if your guess is too high or too low.\n");
    printf("Try to beat the leaderboard with your speed and accuracy!\n\n" RESET);
}

void debug()
{
    printf(COOL_ICE "Debug: secretNumber = %d\n", secretNumber);
    printf("Debug: time(NULL) = %ld\n", time(NULL));
    printf("Debug: moves = %d, time = %d, multiplier = %.2lf\n\n" RESET, moves, timeTaken, multiplier);
}
