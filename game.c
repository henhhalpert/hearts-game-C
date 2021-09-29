/* possible NULL pointers */
#include <stdio.h>
/* malloc for a new cards */
#include <stdlib.h>
/* status signals */
#define GAME_MAGIC_NUM 1266784
#define NO_WINNER_YET -1
#define STILL_RUNNING 0
#define GAME_OVER 100
#define RUNNING_GAME 1

#include "round.h"
#include "player.h"
#include "game.h"
#include "deck.h"
#include "card.h"

struct Game
{
	struct Team *m_team;
	int m_bots;
	int m_humans;
	int m_gameStatus; 
	int *m_scores; 
	int m_whosTheWinner;
	int m_status;
	int m_magic;
	
};

void RefillDeck(struct Game *_game)
{
	struct Deck *newDeck;
	struct Team *newTeam;
	int nCards, nCardsInHand;
	
	nCards = GetAmntOfCardsHearts();
	newDeck = GetDeckForHearts(nCards);
	
	nCardsInHand = nCards / (_game->m_bots + _game->m_humans);
	DestroyTeam(_game->m_team); 
	newTeam = CreatePlayers(_game->m_bots, _game->m_humans, nCardsInHand, newDeck->m_cards);
	_game->m_team = newTeam;
	
}

struct Game * CreateGame(int _nBots, int _nHumans) /* called in main.c */
{
	struct Game *newGame;
	struct Team *newTeam;
	struct Deck *newDeck;
	
	int *scores;
	int card;
	int player, nCards, nCardsInHand;
	if (_nBots + _nHumans <= 0)
	{
		return NULL;
	}
	/* new game */
	newGame = (struct Game*)malloc(sizeof(struct Game));
	if(newGame == NULL)
	{
		return NULL;
	}
	scores = (int*)malloc(sizeof(int) * (_nBots + _nHumans));
	if(scores == NULL)
	{
		return NULL;
	}
	/* create a deck according to policy */
	nCards = GetAmntOfCardsHearts();
	newDeck = GetDeckForHearts(nCards);
	
	/*create players */
	nCardsInHand = nCards / (_nBots + _nHumans);
	newTeam = CreatePlayers(_nBots, _nHumans, nCardsInHand, newDeck->m_cards); 
	
	newGame->m_team = newTeam;
	newGame->m_whosTheWinner = NO_WINNER_YET;
	newGame->m_status = STILL_RUNNING;
	newGame->m_scores = scores;
	newGame->m_bots = _nBots;
	newGame->m_humans = _nHumans;
	newGame->m_magic = GAME_MAGIC_NUM;
	for(player=0; player < (_nBots + _nHumans); player++ )
	{
		newGame->m_scores[player] = 0; /*initial score of each player */
	}
	return newGame;
}

void SortHands(struct Game *_game)
{
	SortCards(_game->m_team);
}

void SortHandsByRank(struct Game *_game)
{
	SortCardsByRank(_game->m_team);
}

void PrintGameCards(struct Game *_game)
{
	PrintCards(_game->m_team);
}

void PrintHand(struct Game *_game, int _playerId)
{
	PrintCardsHand(_game->m_team, _playerId);
}

void GetCardFromPlayer(struct Game *_game, int _playerId, int *card, int(*PolicyGetCard)(int), int _cardIdx)
{
	PlayerGiveCard(_game->m_team, _playerId, card, PolicyGetCard, _cardIdx);
}

void SeeCardGame(struct Game *_game, int _playerId, int *card, int _idx)
{
	PlayerSeeCard(_game->m_team, _playerId, card, _idx);
}

void GiveCardsToGame(struct Game *_game, int _playerId, int rank, int suit)
{
	GiveCardsToPlayer(_game->m_team, _playerId, rank, suit);
}

void FindPlayerGame(struct Game *_game, int _rank, int _suit, int *playerID)
{
	FindPlayer(_game->m_team, _rank, _suit, playerID);
}

void FindIdxGame(struct Game *_game, int _rank, int _suit, int *idx, int _player)
{
	FindIdx(_game->m_team, _rank, _suit, idx, _player);
}

void FindBestCardGame(struct Game *_game, int _playerID, int _leadSuit, int _leadRank, int *idx, int _status)
{
	FindBestCardIdx(_game->m_team, _playerID, _leadSuit, _leadRank, idx, _status);
}

int CheckSuitGame(struct Game *_game, int _playerID, int _leadSuit)
{
	return CheckSuitInHand(_game->m_team, _playerID, _leadSuit);
}

void PrintBoard(struct Game *_game)
{
	int nPlayers, score;
	nPlayers = _game->m_bots + _game->m_humans;
	for(score=0; score<nPlayers; score++)
	{
		printf("------- PLAYER: %d, SCORE: %d\n", score, _game->m_scores[score]);
	}
}

int GetWinner(struct Game *_game)
{
	int tmp;
	int minIdx=0, score, nPlayers;
	nPlayers = _game->m_bots + _game->m_humans;
	
	tmp = _game->m_scores[0];
	for(score=0; score<nPlayers; score++)
	{
		if(_game->m_scores[score] < tmp)
		{
			tmp = _game->m_scores[score];
			minIdx = score;
		}
	}
	return minIdx;
}

int UpdateScores(struct Game *_game, int *_updatedScores, int(*GameStatus)(int *))
{
	int nPlayers, score, minIdx;
	nPlayers = _game->m_bots + _game->m_humans;
	printf("------- SCORE BOARD ------ \n");
/*	PrintBoard(_game);*/
	for(score=0; score<nPlayers; score++)
	{
		_game->m_scores[score] += _updatedScores[score];
		if(GameStatus(_game->m_scores) == GAME_OVER)
		{
			printf("GAME FINAL RESULTS:\n");
			PrintBoard(_game);
			minIdx = GetWinner(_game);
			printf("\nThe winner is Player #%d\n", minIdx);
			DestroyGame(_game);
			return GAME_OVER;
		}
	}
	return RUNNING_GAME;
}

void DestroyGame(struct Game *_game)
{
	int i;
	if(_game == NULL || _game->m_magic != GAME_MAGIC_NUM)
	{
		return;
	}
	_game->m_magic = 0;
	DestroyTeam(_game->m_team);
	free(_game);
}










