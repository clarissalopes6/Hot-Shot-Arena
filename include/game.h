#ifndef GAME_H
#define GAME_H

#define MAX_JOGADORES 10
#define MAX_NOME 32

typedef struct Jogador {
    char nome[MAX_NOME];
    int acertos;
    int eliminacoes;
    int emCampo; 
    struct Jogador *prox;
} Jogador;

typedef struct Fila {
    Jogador *inicio;
    Jogador *fim;
} Fila;

typedef struct Time {
    char nome[MAX_NOME];
    Jogador jogadores[MAX_JOGADORES];
    int qtdJogadores;
    Fila filaEspera;
} Time;

typedef struct Jogo {
    Time timeA;
    Time timeB;
    int pontuacaoA;
    int pontuacaoB;
    int tempo;
} Jogo;

void iniciarJogo(Jogo *jogo);
void jogarRodada(Jogo *jogo);
void eliminarJogador(Time *time, int idxJogador);
void resgatarJogador(Time *time);
void exibirRanking(Time *time);
void salvarPartida(Jogo *jogo);

#endif
