#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dict.h"
#include "wordle.h"

#define SIZEGET 7 // words of 5 characters + \n + terminator
#define SIZEWORD 5 // words of 5 characters
#define LETTERS 26 // nb of letters in the alphabet

struct Wordle_t
{
	Dict *guesses;
	char *answer;
};

static void terminate(char *m)
{
    printf("%s\n", m);
    exit(EXIT_FAILURE);
    // TO TEACHER : l'imitation est la plus sincère des flatteries
}

Wordle *wordleStart(char *answers_file, char *guesses_file, char *answer)
{
	/*
	create structure
	---------------------------------------------------------------------------
	*/
	Wordle *wordle = malloc(sizeof(Wordle));
	if(!wordle)
		terminate("wordleStart: malloc Wordle failed");

	/*
	create guesses dictionary
	---------------------------------------------------------------------------
	*/
	FILE *guessesFile = fopen(guesses_file,"r");
	if(!guessesFile)
		terminate("wordleStart: guesses_file cannot be opened");
	int kitsune = 0;

// /* --meta comment-- // put a "//"in the begin to enable/disable the fragment
fseek(guessesFile, 0, SEEK_END);//go to the end
int  size = ftell(guessesFile)+1;  // get the number of bytes
fseek(guessesFile, 0, SEEK_SET); // return at the start
kitsune=size/(SIZEWORD+1);
//*/
/* little hack
give the number of bytes in the file , dividing that with SIZEWORD+1 gives the number of word
idk what you want to chose
it depends on how you encode the characters
but since we only use characters from the Roman alphabet,
a character is represented by a byte in UTF-8, its like ascii
*/
/*	  // seems like a cute kitsune wants to help

	while(fgets(guess,SIZEGET,guessesFile))
		kitsune++;
*/	/* initialize dictionary */
	Dict *guesses = dictCreate(kitsune);
	char guess[SIZEGET];
//	rewind(guessesFile);
	for(kitsune = 0; fgets(guess,SIZEGET,guessesFile); kitsune++)
		{
			guess[SIZEWORD] = 0;
			dictInsert(guesses,guess,(double) kitsune);
		}

	wordle->guesses = guesses;
	fclose(guessesFile);

	/*
	choose answer
	---------------------------------------------------------------------------
	*/
	wordle->answer = malloc((SIZEWORD+1)*sizeof(char));
	if(!wordle->answer)
		terminate("wordleStart: malloc answer failed");

	if(answer)
	{
		strcpy(wordle->answer,answer);
	}
	else
	{
		FILE *answerFile = fopen(answers_file,"r");
		if(!answerFile)
			terminate("wordleStart: answers_file cannot be opened");

		/* count Sa */
		kitsune = 0;
		char randomAnswer[SIZEGET];
		while(fgets(randomAnswer,SIZEGET,answerFile))
			kitsune++;

		/* choose random word */
		rewind(answerFile);
		int random = rand() % kitsune;
		for(kitsune = 0; kitsune < random; kitsune++)
			{
				fgets(randomAnswer,SIZEGET,answerFile);
			}
			// faute de mieux je parcours tout le fichier jusqu'au bon mot
			//->fseek
		randomAnswer[SIZEWORD] = 0;
		strcpy(wordle->answer,randomAnswer);
		fclose(answerFile);
	}
	return wordle;
}

void wordleFree(Wordle *game)
{
	dictFree(game->guesses);
	free(game);
}

char *wordleGetTrueWord(Wordle *game)
{
	return game->answer;
}
char *wordleComputePattern(char *guess, char *answer)
{
	/*
	Count Repetitions of each letter in answer
	---------------------------------------------------------------------------
	*/
	/* initialize array for 26 letters */
	int answerLetters[LETTERS];
	for(int i=0; i < LETTERS ;i++)
		answerLetters[i] = 0;

	/* count each letter */
	for(int i=0; i < SIZEWORD; i++)
	{
		answerLetters[answer[i] - 'a']++;
	}

	/* Pattern Computation
	---------------------------------------------------------------------------
	*/
	/* initialize pattern */
	char *pattern = malloc( (SIZEWORD+1) * sizeof(char) );
	for(int i=0; i < SIZEWORD; i++)
		pattern[i] = '_';
	pattern[SIZEWORD] = 0;

	/* correct letters */
	for(int i=0; i < SIZEWORD; i++)
	{
		if(guess[i] == answer[i])
			{
				pattern[i] = 'o';
				answerLetters[answer[i] - 'a']--;
			}
	}

	/* remaining incorrect or misplaced letters */
	for(int i=0; i < SIZEWORD; i++)
	{
		if(pattern[i] != 'o')
		{
			if(answerLetters[guess[i] - 'a'] > 0)
				{
					pattern[i] = '*';
					answerLetters[guess[i] - 'a']--;
				}
		}
	}

	return pattern;
}

char *wordleCheckGuess(Wordle *game, char *guess)
{
	if(!dictContains(game->guesses, guess))
		return NULL;

	return wordleComputePattern(guess, game->answer);
}
