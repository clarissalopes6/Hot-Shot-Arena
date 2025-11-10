#include "game.h"
#include <stdio.h>
#include <string.h>

void iniciarJogo(Jogo *jogo) {
    strcpy(jogo->timeA.nome, "Time A");
    strcpy(jogo->timeB.nome, "Time B");
    jogo->timeA.qtdJogadores = MAX_JOGADORES;
    jogo->timeB.qtdJogadores = MAX_JOGADORES;
    for (int i = 0; i < MAX_JOGADORES; i++) {
        sprintf(jogo->timeA.jogadores[i].nome, "A_Jogador%d", i+1);
        jogo->timeA.jogadores[i].acertos = 0;
        jogo->timeA.jogadores[i].eliminacoes = 0;
        jogo->timeA.jogadores[i].emCampo = 1;
        jogo->timeA.jogadores[i].prox = NULL;
        sprintf(jogo->timeB.jogadores[i].nome, "B_Jogador%d", i+1);
        jogo->timeB.jogadores[i].acertos = 0;
        jogo->timeB.jogadores[i].eliminacoes = 0;
        jogo->timeB.jogadores[i].emCampo = 1;
        jogo->timeB.jogadores[i].prox = NULL;
    }
    jogo->pontuacaoA = 0;
    jogo->pontuacaoB = 0;
    jogo->tempo = 0;
    jogo->timeA.filaEspera.inicio = NULL;
    jogo->timeA.filaEspera.fim = NULL;
    jogo->timeB.filaEspera.inicio = NULL;
    jogo->timeB.filaEspera.fim = NULL;
}

void jogarRodada(Jogo *jogo) {
    printf("Executando rodada...\n");
}

void eliminarJogador(Time *time, int idxJogador) {
    printf("Eliminando jogador %s\n", time->jogadores[idxJogador].nome);
}

void resgatarJogador(Time *time) {
    printf("Resgatando jogador da fila de espera\n");
}

void exibirRanking(Time *time) {
    printf("Ranking do time %s:\n", time->nome);
}

void salvarPartida(Jogo *jogo) {
    printf("Salvando partida...\n");
}
