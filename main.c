/// Project 3: A Dynamically Evil Word Guessing Game
/// Author: Jonathan Kang
/// Date: October 5, 2023
/// Course: CS 211, Fall 2023, UIC
/// System: Advanced Zylab
/// Description: The project is an evil "hangman" game where if given the opportunity, the program would make it as difficult as possible to give the answer from a variety of words. Through comparing multiple words from a dictionary and identifying what has the most varied choices, the program attempts to show as little progress as the participant can possibly do.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

typedef struct Pattern_struct {
    char* pat;  //string pattern - exs: "hello", "-ell-", "-----"
    int count;  //frequency of pattern in the current word list
    int changes;//number of occurences of new letter in this pattern
} Pattern;

//-------------------------------------------------------------------
//      Write addWord() function, which adds a new word to
//      a dynamic heap-allocated array of C-strings; heap-space must  
//      be allocated for the new word inside this function, then copy  
//      the chars from newWord to the newly allocated heap-space; if
//      adding the new word pushes numWords above maxWords, then 
//      realloate the words array with double the capacity
//      parameters: 
//          char*** words - array of C-strings passed-by-pointer
//              note: *words is an array of pointers
//                    **words is an array of chars
//                    ***words is a char  
//                    (*words)[0] is the 1st C-string in the array
//                    (*words)[1] is the 2nd C-string in the array
//                    (*words)[0][0] is 1st char of 1st C-string
//                    (*words)[1][2] is 3rd char of 2nd C-string
//                    etc.
//          int* numWords - current # of words in *words array
//               note: numWords will increase by 1 here, 
//                     so it is passed-by-pointer
//          int* maxWords - current capacity of *words array
//               note: maxWords may increase by x2 here, 
//                     so it is passed-by-pointer
//          char* newWord - C-string word to be added to words
//               note: newWord is a C-string, automatically a pointer
//-------------------------------------------------------------------
void addWord(char*** words, int* numWords, int* maxWords, char* newWord) {
    if(*numWords >= *maxWords){
        //create heap-allocated array with double the memory
        char **doubled = (char**)malloc(2 * (*maxWords) * sizeof(char*));
        //copies all the words in "words" into the doubled array
        for(int i = 0; i < *numWords; i++){
            doubled[i] = (char*)malloc((strlen((*words)[i]) + 1) * sizeof(char));
            strcpy(doubled[i], (*words)[i]);
        }
        //make the *maxWords also double by 2
        (*maxWords) = (*maxWords) * 2;
        //add one more memory slot for a new word
        doubled[*numWords] = (char*)malloc((strlen(newWord) + 1) * sizeof(char));
        //copy newWord into doubled[*numWords]
        strcpy(doubled[*numWords], newWord);
        //free memory
        for(int i = 0; i < *numWords; i++){
            free((*words)[i]);
        }
        free(*words);
        //"reallocate"
        (*words) = doubled;
    } 
    //*numWords < *maxWords
    else{
        (*words)[*numWords] = (char*)malloc((strlen(newWord) + 1) * sizeof(char));
        strcpy((*words)[*numWords], newWord);
    }
    *numWords += 1;
}

//Essentially addWord but for patterns
void addPattern(Pattern** patterns, int* numPatterns, int* maxPatterns, char* newPattern){
    if(*numPatterns >= *maxPatterns){
        //create heap-allocated array with double the memory
        Pattern *doubled = (Pattern*)malloc(2 * (*maxPatterns) * sizeof(Pattern));
        //copies all the patterns in "patterns" into the doubled array
        for(int i = 0; i < *numPatterns; i++){
            doubled[i].pat = (char*)malloc((strlen((*patterns)[i].pat) + 1) * sizeof(char));
            doubled[i].count = (*patterns)[i].count;
            doubled[i].changes = (*patterns)[i].changes;
            strcpy(doubled[i].pat, (*patterns)[i].pat);
        }
        //make the *maxPatterns also double by 2
        (*maxPatterns) = (*maxPatterns) * 2;
        //add one more memory slot for a new pattern
        doubled[*numPatterns].pat = (char*)malloc((strlen(newPattern) + 1) * sizeof(char));
        //copy newWord into doubled[*numWords]
        strcpy(doubled[*numPatterns].pat, newPattern);
        //free memory
        for(int i = 0; i < *numPatterns; i++){
            free((*patterns)[i].pat);
        }
        free(*patterns);
        //"reallocate"
        (*patterns) = doubled;
    }
    //*numPatterns < *maxPatterns
    else{
        (*patterns)[*numPatterns].pat = (char*)malloc((strlen(newPattern) + 1) * sizeof(char));
        strcpy((*patterns)[*numPatterns].pat, newPattern);
    }
    *numPatterns += 1;
}

//-------------------------------------------------------------------
//              Write the strNumMods() function, which
//      returns count of character differences between two strings;
//      includes extra characters in longer string if different lengths
// Exs: str1 = magic, str2 = magic, returns 0
//      str1 = wands, str2 = wants, returns 1
//      str1 = magic, str2 = wands, returns 4
//      str1 = magic, str2 = mag, returns 2
//      str1 = magic, str2 = magicwand, returns 4
//      str1 = magic, str2 = darkmagic, returns 8
//-------------------------------------------------------------------
int strNumMods(char* str1, char* str2) {
    int length = strlen(str1);
    int difference = 0;
    //adds difference from size
    if(strlen(str1) > strlen(str2)){
        length = strlen(str2);
        difference = strlen(str1) - strlen(str2);
    }
    //adds difference from size
    else if(strlen(str1) < strlen(str2)){
        length = strlen(str1);
        difference = strlen(str2) - strlen(str1);
    }
    //accumulates difference integer from same size
    for(int i = 0; i < length; i++){
        if(str1[i] != str2[i]){
            difference++;
        }
    }
    return difference;
}

//-------------------------------------------------------------------
//              Write the strDiffInd() function, which
//      returns index of the character where two strings first differ, &
//      returns strlen(str1) = strlen(str2) if no differences
// Exs: str1 = magic, str2 = magic, returns 5
//      str1 = wands, str2 = wants, returns 3
//      str1 = magic, str2 = wands, returns 0
//      str1 = magic, str2 = mag, returns 3
//      str1 = magic, str2 = magicwand, returns 5
//      str1 = magic, str2 = darkmagic, returns 0
//-------------------------------------------------------------------
int strDiffInd(char* str1, char* str2) {
    int length = strlen(str1);
    int difference = 0;
    //gets the shorter length of two
    if(strlen(str1) > strlen(str2)){
        length = strlen(str2);
        difference = strlen(str2);
    }
    //gets the shorter length of one
    else if(strlen(str1) < strlen(str2)){
        length = strlen(str1);
        difference = strlen(str1);
    }
    //for loops through the shorter length to find the difference index
    for(int i = 0; i < length; i++){
        if(str1[i] != str2[i]){
            return i;
        }
    }
    //returns the length if no differences
    if(strcmp(str1, str2) == 0){
        return strlen(str1);
    }
    return difference;
}

//frees wordList
void freeWordList(int* wordCount, char** wordList){
    for(int i = 0; i < *wordCount; i++) {
        free(wordList[i]);
    }
    free(wordList);
}

//frees patterns
void freePatterns(int* countPattern, Pattern* patterns){
    for(int i = 0; i < *countPattern; i++){
        free(patterns[i].pat);
    }
    free(patterns);
}

//occurs when lettermode is true
void letterModeTrue(bool* usedChar){
    printf("Previously guessed letters: ");
    for(int i = 0; i < 26; i++){
        if(usedChar[i] == true){
            printf("%c ", i + 'a');
        }
    }
    printf("\n");
}

//occurs when wordMode is true
void wordModeTrue(int* wordCount, char** wordList){
    printf("Possible words are now:\n");
    for(int i = 0; i < *wordCount; i++){
        printf("  %s\n", wordList[i]);
    }
}

//occurs when the letter is already guessed
void alreadyGuessed(Pattern* patterns, int* countPattern){
    printf("Letter previously guessed...\n");
    for(int i = 0; i < *countPattern; i++){
        free(patterns[i].pat);
    }
    free(patterns);
}

//adds to current pattern
void currentPatternAdd(int* wordSize, char* letter, char* currentPattern, char** wordList, int* i){
    for(int j = 0; j < *wordSize; j++){
        if(wordList[*i][j] == *letter){
            currentPattern[j] = *letter;
        }
    }
}

//with a, goes up by one to see how many patterns there are, as well as using strnummods
void countPatternProcessing(int* a, int* countPattern, char* currentPattern, Pattern* patterns, bool* addGateKeep){
    while(*a < *countPattern){
        if(strNumMods(currentPattern, patterns[*a].pat) == 0){
            //used to access the next process after this function
            *addGateKeep = true;
            patterns[*a].count++;
            break;
        }
        (*a)++;
    }
}

//determines the firstDiff everytime a comparison happens within the while loop on line 643.
void constantOrganization(int* position, Pattern* patterns, int* baseCheck, int* firstDiff, char* base, int* collectionOrg, int* collectionSize, int* currentPatternChoice){
    int collectionBaseCheck[*position];
    for(int i = 0; i < *position; i++){
        //find index of earliest difference
        int realDiff = strDiffInd((patterns)[*baseCheck].pat + (*firstDiff) + 1, base + (*firstDiff) + 1);
        //change baseCheck position if less than before
        if(realDiff > strDiffInd((patterns)[collectionOrg[i]].pat + (*firstDiff) + 1, base + (*firstDiff) + 1)){
            *baseCheck = collectionOrg[i];
        }
    }
    *collectionSize = *position;
    *position = 0;
    //if needed, used to repeat the while loop
    for(int i = 0; i < *collectionSize; i++){
        if(strDiffInd((patterns)[*baseCheck].pat + (*firstDiff) + 1, base + (*firstDiff) + 1) == strDiffInd((patterns)[collectionOrg[i]].pat + (*firstDiff) + 1, base + (*firstDiff) + 1)){
            collectionBaseCheck[*position] = collectionOrg[i];
            (*position)++;
        }
    }
    //changes firstDiff for the next while loop for difference
    *firstDiff = strDiffInd((patterns)[*baseCheck].pat + (*firstDiff) + 1, base + (*firstDiff) + 1);
    *currentPatternChoice = collectionBaseCheck[0];
    //might decrease necessary patterns to compare
    for(int i = 0; i < *position; i++){
        collectionOrg[i] = collectionBaseCheck[i];
    }
}

int main(int argc, char* argv[]) {
    printf("Welcome to the (Evil) Word Guessing Game!\n\n");
    bool solved = false;
    //-------------------------------------------------------------------
    //                  Handle command-line arguments
    // command-line arguments:
    //  [-n wordSize] = sets number of characters in word to be guessed;
    //                  if [wordSize] is not a valid input (cannot be  
    //                  less than 2 or greater than 29), then print:
    //                    "Invalid word size."
    //                    "Terminating program..."
    //                  if dictionary has no words of length wordSize,
    //                  then print:
    //                    "Dictionary has no words of length [wordSize]."
    //                    "Terminating program..."
    //                  default wordSize = 5
    // [-g numGuesses] = sets number of unsuccessful letter guesses;
    //                   numGuesses must be a positive integer,
    //                   otherwise print 
    //                     "Invalid number of guesses."
    //                     "Terminating program..."
    //                   default numGuesses = 26 
    //                   (note: 26 guesses guarantees player can win)
    // [-s] = stats mode, which includes printing of dictionary 
    //                    statistics AND number of possible words 
    //                    remaining during gameplay.
    //                    default is OFF
    // [-w] = word list mode, which includes a print out of the full 
    //                        list of possible words before every guess.
    //                        default is OFF
    // [-l] = letter list mode, which prints out a list of previously 
    //                          guessed letters before each new guess
    //                          default is OFF
    // [-p] = pattern list mode, which prints out a list of all patterns 
    //                           in the word list, together with the
    //                           frequency of each pattern and the 
    //                           number of changes in the pattern due to
    //                           the current guess
    //                           default is OFF
    // [-v] = verbose mode, which turns ON stats mode, word list mode, 
    //                      letter list mode, AND pattern list mode
    // [-x] = extension mode, which is an optional extra credit 
    //                        extension doing something interesting with
    //                        the evil word game, e.g. optimizing the 
    //                        algorithm to predict future guesses
    //-------------------------------------------------------------------
    int wordSize = 5;
    int numGuesses = 26;
    bool statsMode = false;
    bool wordMode = false;
    bool letterMode = false;
    bool patternMode = false;
    printf("Game Settings:\n");
    int inc = 1; //increments
    //goes through each command
    while(inc < argc){
        //to change the wordSize
        if(strcmp(argv[inc], "-n") == 0 && atoi(argv[inc + 1]) >= 2 && atoi(argv[inc + 1]) <= 29){
            wordSize = atoi(argv[inc + 1]);
            inc++;
        }
        //-n but invalid wordSize
        else if(strcmp(argv[inc], "-n") == 0 && (atoi(argv[inc + 1]) < 2 || atoi(argv[inc + 1]) > 29)){
            printf("Invalid word size.\n");
            printf("Terminating program...");
            return 0;
        }
        //to change the numGuesses
        else if(strcmp(argv[inc], "-g") == 0 && atoi(argv[inc + 1]) > 0){
            numGuesses = atoi(argv[inc + 1]);
            inc++;
        }
        //-g but invalid numGuesses
        else if(strcmp(argv[inc], "-g") == 0&& atoi(argv[inc + 1]) <= 0){
            printf("Invalid number of guesses.");
            printf("Terminating program...");
            return 0;
        }
        //change statsMode
        else if(strcmp(argv[inc], "-s") == 0){
            statsMode = true;
        }
        //change wordMode
        else if(strcmp(argv[inc], "-w") == 0){
            wordMode = true;
        }
        //change letterMode
        else if(strcmp(argv[inc], "-l") == 0){
            letterMode = true;
        }
        //change patternMode
        else if(strcmp(argv[inc], "-p") == 0){
            patternMode = true;
        }
        //change all four modes
        else if(strcmp(argv[inc], "-v") == 0){
            statsMode = true;
            wordMode = true;
            letterMode = true;
            patternMode = true;
        }
        //extra credit
        else if(strcmp(argv[inc], "-x") == 0){
            //empty
        }
        //an invalid command
        else{
            printf("Invalid command-line argument.\n");
            printf("Terminating program...");
            return 0;
        }
        inc++;
    }
    //off[] and on[] make sure to print out if the mode is active or not
    char off[] = "OFF";
    char on[] = "ON";
    printf("  Word Size = %d\n", wordSize);
    printf("  Number of Guesses = %d\n", numGuesses);
    if(statsMode == false){
        printf("  View Stats Mode = %s\n", off);
    }
    else{
        printf("  View Stats Mode = %s\n", on);
    }
    if(wordMode == false){
        printf("  View Word List Mode = %s\n", off);
    }
    else{
        printf("  View Word List Mode = %s\n", on);
    }
    if(letterMode == false){
        printf("  View Letter List Mode = %s\n", off);
    }
    else{
        printf("  View Letter List Mode = %s\n", on);
    }
    if(patternMode == false){
        printf("  View Pattern List Mode = %s\n", off);
    }
    else{
        printf("  View Pattern List Mode = %s\n", on);
    }

    //-------------------------------------------------------------------
    //                  File-read the word list from dictionary.txt, 
    //                  storing only words of length set by wordSize 
    //                  (a command-line argument) in a dynamically
    //                  growing heap-allocated array of C-strings:
    //                      - the word list should be a dynamic array of 
    //                        pointers, initially with maximum size 4, 
    //                        doubling in maximum size whenever more 
    //                        space is needed
    //                      - each element of the word list array should 
    //                        point to a heap-allocated C-string that 
    //                        can store a word containing exactly 
    //                        wordSize lower-case letters
    //-------------------------------------------------------------------
    FILE* file = fopen("dictionary.txt", "r");
    int capacity = 4;
    int wordCount = 0;
    int total = 0;
    char** wordList = (char**)malloc(capacity * sizeof(char*));
    char word[wordSize];
    char longest[29] = "";
    //gets each word from dictionary.txt and adds into wordList
    while(fscanf(file, "%30s", word) == 1){
        int length = strlen(word);
        //create end
        while(length > 0 && !isalpha(word[length - 1])){
            word[length - 1] = '\0';
            length--;
        }
        //get longest word in dictionary
        if(length > strlen(longest)){
            strcpy(longest, word);
        }
        //doubling the wordList
        if(wordCount >= capacity && length == wordSize){  
            capacity *= 2;
            char** temp = (char**)malloc(capacity * sizeof(char*));
            for(int i = 0; i < wordCount; i++){
                temp[i] = malloc((strlen(wordList[i]) + 1) * sizeof(char));
                strcpy(temp[i], wordList[i]);
                free(wordList[i]);
            }
            free(wordList);
            //wordList is now temp
            wordList = temp;
        }
        //word added
        if(length == wordSize){
            wordList[wordCount] = malloc((length + 1) * sizeof(char));
            strcpy(wordList[wordCount], word);
            wordCount++;
        }
        total++;
    }
    fclose(file);

    //-------------------------------------------------------------------
    //                 Display dictionary stats [-s] & word list [-w] 
    //                 if the proper command-line flags are turned ON;
    //                 see sample output for proper formatting 
    //-------------------------------------------------------------------
    if(statsMode == true){
        printf("The dictionary contains %d words total.\n", total);
        printf("The longest word %s has 29 chars.\n", longest);
        printf("The dictionary contains %d words of length %d.\n", wordCount, wordSize);
        printf("Max size of the dynamic words array is %d.\n", capacity);
    }
    if(wordMode == true){
        printf("Words of length %d:\n", wordSize);
        for(int i = 0; i < wordCount; i++) {
            printf("  %s\n", wordList[i]);
        }
        if(wordCount == 0){
            printf("Dictionary has no words of length %d.\n", wordSize);
            printf("Terminating program...");
            return 0;
        }
    }

    //-------------------------------------------------------------------
    //                  Play the game; repeatedly...
    //           - Input a character from user as their guess; 
    //             if guess is not a lower case letter OR has been 
    //             previously guessed, input another character.
    //           - Build a dynamic array of unique Pattern structs, 
    //             where each Pattern contains a heap-allocated C-string
    //             representing the current status of the final word, 
    //             with a '-' for each unsettled character; for each word
    //             in the current word list, produce an updated pattern  
    //             by replacing '-'s with current letter guess wherever 
    //             that letter appears in the word, and either appending 
    //             it as a new Pattern if it is unique in the array of 
    //             Patterns, OR incrementing the count subitem for the
    //             appropriate Pattern. The array of Patterns should 
    //             dynamically grow, beginning with maximum size 4, 
    //             doubling whenever more space is needed.
    //           - Find the most common pattern in the current word list 
    //             and update the final word pattern;
    //             i.e. find the Pattern in the array that has the 
    //             highest count subitem. Break ties as follows: 
    //                + for two Patterns with the same count, select the 
    //                  one with fewer occurences of the guessed letter;
    //                  i.e. smaller changes subitem
    //                + for two Patterns with the same count & changes,
    //                  select the one where the fist occurrence of the 
    //                  guessed letter is earlier in the word
    //           - Report to the user whether their guessed letter is in 
    //             the final word or not. End the game if the user has 
    //             used all of the allowed guesses or has solved the 
    //             final word. Otherwise, repeat the process by inputting 
    //             another letter from the user.
    //           - Throughout the game, produce print statements matching 
    //             the format of the sample outputs; only printing the 
    //             statistics [-s], word list [-w], letter list [-l], and 
    //             pattern list [-p] if the proper command-line flags are 
    //             turned ON;
    //-------------------------------------------------------------------
    char letter;
    int max = 4;
    int countPattern = 0;
    char base[wordSize + 1];
    bool usedChar[26];
    //this means any alphabets haven't been inputted yet
    for(int i = 0; i < 26; i++){
        usedChar[i] = false;
    }
    //creates base
    for(int i = 0; i < wordSize; i++){
        base[i] = '-';
    }
    base[wordSize] = '\0';
    printf("The word pattern is: %s\n\n", base);
    printf("Number of guesses remaining: %d\n", numGuesses);
    while(numGuesses > 0){
        countPattern = 0;
        max = 4;
        //creates memory space for patterns
        Pattern *patterns = malloc(max * sizeof(Pattern));
        //data presentation
        if(letterMode == true){
            letterModeTrue(usedChar);
        }
        printf("Guess a letter (# to end game): \n");
        //gets letter here
        scanf(" %c", &letter);
        //if lowercase
        if(islower(letter) != 0){
            //checks if the letter has already been inputted
            if(usedChar[letter - 'a']){
                alreadyGuessed(patterns, &countPattern);
                continue;
            }
            //otherwise adds it into the usedChar to check for the next possible input
            else{
                usedChar[letter - 'a'] = true;
            }
            int currentPatternChoice;
            //for loops through each word
            for(int i = 0; i < wordCount; i++){
                char currentPattern[wordSize + 1];
                strcpy(currentPattern, base);
                bool wordInclude = false;
                //for loops through each letter. if equal, add letter to currentpattern
                currentPatternAdd(&wordSize, &letter, currentPattern, wordList, &i);
                currentPattern[wordSize] = '\0';
                int a = 0;
                bool addGateKeep = false;
                countPatternProcessing(&a, &countPattern, currentPattern, patterns, &addGateKeep);
                //add the pattern if it doesn't exist
                if(addGateKeep == false){
                    addPattern(&patterns, &countPattern, &max, currentPattern);
                    patterns[countPattern - 1].count = 1;
                    patterns[countPattern - 1].changes = strNumMods(base, patterns[countPattern - 1].pat);
                }
            }
            //there's more than one pattern
            if(countPattern >= 1 && patternMode == true){
                printf("All patterns for letter %c: \n", letter);
                for(int i = 0; i < countPattern; i++){
                    printf("  %s    count = %d  changes = %d\n", patterns[i].pat, patterns[i].count, patterns[i].changes);
                }
            }
            //For the code below, until line 625 is sorting through counts and changes of valid patterns and determining which one to use
            //this includes the tiebreakers
            int baseCheck = 0;
            //cycling with countPattern, changes baseCheck to i if patterns[i]'s count is greater
            for(int i = 1; i < countPattern; i++){
                if((patterns)[baseCheck].count < (patterns)[i].count){
                    baseCheck = i;
                }
            }
            //a collection of patterns
            int collection[countPattern];
            int position = 0;
            //cycles with countPattern, increases position by 1 and collection[position] to i if pattern from baseCheck is same as another
            for(int i = 0; i < countPattern; i++){
                if((patterns)[baseCheck].count == (patterns)[i].count){
                    collection[position] = i;
                    position++;
                }
            }
            if(position == 1){
                currentPatternChoice = collection[0];
            }
            else{
                int collectionSize = position;
                int collectionOrg[collectionSize];
                //compares changes
                for(int i = 0; i < collectionSize; i++){
                    if((patterns)[collection[i]].changes < (patterns)[baseCheck].changes){
                        baseCheck = collection[i];
                    }
                }
                position = 0; //reset position
                //collect the patterns that have same changes, position++
                for(int i = 0; i < collectionSize; i++){
                    if((patterns)[baseCheck].changes == (patterns)[collection[i]].changes){
                        collectionOrg[position] = collection[i];
                        position++;
                    }
                }
                int firstDiff = -1;
                currentPatternChoice = collectionOrg[0];
                int* collectionOrgPointer = collectionOrg;
                //comparison through the first different index
                while(position > 1){
                    constantOrganization(&position, patterns, &baseCheck, &firstDiff, base, collectionOrg, &collectionSize, &currentPatternChoice);
                }
            }
            //correct
            if(strNumMods(patterns[currentPatternChoice].pat, base) > 0){
                printf("Good guess! The word has at least one %c.\n", letter);
            }
            //mistake, loss of guess
            else{
                printf("Oops, there are no %c's. You used a guess.\n", letter);
                numGuesses--;
            }
            char previous[wordSize];
            strcpy(previous, base);
            strcpy(base, patterns[currentPatternChoice].pat);
            //to create the newWordList that replaces the previous wordList for the next guess
            char** newWordList = (char**)malloc(patterns[currentPatternChoice].count * sizeof(char*));
            //the amount of dashes there are must equal the difference between the pattern and the word
            int newWordPosition = 0;
            for(int i = 0; i < wordCount; i++){
                char currentPattern[wordSize];
                strcpy(currentPattern, previous);
                bool wordInclude = false;
                for(int j = 0; j < wordSize; j++){
                    if(wordList[i][j] == letter){
                        currentPattern[j] = letter;
                    }
                }
                //sees if there are no differences
                if(strNumMods(currentPattern, base) == 0){
                    newWordList[newWordPosition] = (char*)malloc((wordSize + 1) * sizeof(char));
                    strcpy(newWordList[newWordPosition], wordList[i]);
                    newWordPosition++;
                }
            }
            for(int i = 0; i < wordCount; i++){
                free(wordList[i]);
            }
            wordCount = newWordPosition;
            free(wordList);
            wordList = newWordList;
            //if statsMode is true?
            if(statsMode == true){
                printf("Number of possible words remaining: %d\n", wordCount);
            }
            //wordMode is true?
            if(wordMode == true){
                wordModeTrue(&wordCount, wordList);
            }
            //show current pattern
            printf("The word pattern is: %s\n\n", base);
            int dashCheck = 0;
            bool perfection = true;
            for(int i = 0; i < wordSize; i++){
                if(base[i] == '-'){
                    perfection = false;
                }
            }
            //lose the game
            if(numGuesses == 0){
                printf("You ran out of guesses and did not solve the word.\n");
                printf("The word is: %s\n", wordList[0]);
                printf("Game over.\n");
                freePatterns(&countPattern, patterns);
                break;
            }
            //win the game
            if(perfection == true){
                printf("You solved the word!\n");
                printf("The word is: %s\n", base);
                printf("Game over.\n\n");
                freePatterns(&countPattern, patterns);
                break;
            }
            printf("Number of guesses remaining: %d\n", numGuesses);
        }
        //end the game early
        else if(letter == '#'){
            printf("Terminating game...");
            freePatterns(&countPattern, patterns);
            break;
        }
        //doesn't work
        else{
            printf("Invalid letter...\n");
        }
        freePatterns(&countPattern, patterns);
    }

    //-------------------------------------------------------------------
    //                  Free all heap-allocated memory to avoid potential 
    //                  memory leaks. Since the word size for each game 
    //                  play is a command-line argument, any array whose 
    //                  size depends on the word size should be 
    //                  dynamically heap-allocated, and thus must be 
    //                  tracked and freed
    //-------------------------------------------------------------------

    freeWordList(&wordCount, wordList);
    return 0;
}
