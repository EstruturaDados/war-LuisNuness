#include <stdio.h>   // Para entrada e saida
#include <stdlib.h>  // Para alocacao dinamica (malloc/free), rand/srand
#include <string.h>  // Para manipulacao de strings (strcpy, strcmp)
#include <time.h>    // Para inicializar a semente de aleatoriedade (srand)

// --- 1. DEFINICOES GLOBAIS E STRUCTS ---

// Constantes para cores dos jogadores
#define JOGADOR_A "VERMELHO"
#define JOGADOR_B "AZUL"
#define MAX_MISSAO_LEN 100 // Tamanho maximo da string de missao

// Struct Territorio (Requisito)
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Struct Jogador (Nova)
typedef struct {
    char cor[10];
    char *missao; // Ponteiro para a missao alocada dinamicamente (Requisito)
} Jogador;

// Vetor de strings de missoes (Requisito: ao menos 5)
// TUDO SEM ACENTO AGORA
char *MISSOES[] = {
    "MISSAO 0: Conquistar no minimo 3 territorios seguidos. (Nao implementada a contagem sequencial)",
    "MISSAO 1: Ter um total de pelo menos 15 tropas no mapa.",
    "MISSAO 2: Possuir 3 territorios com 5 ou mais tropas cada.",
    "MISSAO 3: Eliminar todas as tropas da cor AZUL (Se voce for VERMELHO).",
    "MISSAO 4: Eliminar todas as tropas da cor VERMELHO (Se voce for AZUL)."
};
const int TOTAL_MISSOES = sizeof(MISSOES) / sizeof(MISSOES[0]);


// --- PROTOTIPOS DE FUNCOES (Modularizacao) ---
void limpar_buffer();
Territorio* alocar_territorios(int *tamanho);
void cadastrar_territorios(Territorio* mapa, int tamanho);
void exibir_territorios(Territorio* mapa, int tamanho);
void atacar(Territorio* atacante, Territorio* defensor);
void liberar_memoria(Territorio* mapa, Jogador *jogadores, int num_jogadores);
int escolher_territorio(Territorio* mapa, int tamanho, const char *tipo, int *indice_selecionado);

// Funcoes de Missao
void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
void exibirMissao(char* missao);
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* cor_jogador);

// --- FUNCAO PRINCIPAL (MAIN) ---
int main() {
    srand(time(NULL));
    
    int num_territorios = 0;
    
    // Alocacao Dinamica: Territorios
    Territorio* mapa_territorios = alocar_territorios(&num_territorios);

    if (mapa_territorios == NULL) {
        printf("Erro na alocacao de memoria. O programa sera encerrado.\n");
        return 1;
    }

    // --- Configuracao dos Jogadores ---
    Jogador jogadores[2];
    strcpy(jogadores[0].cor, JOGADOR_A); 
    strcpy(jogadores[1].cor, JOGADOR_B); 
    const int NUM_JOGADORES = 2;

    // Alocacao Dinamica: Missao para cada jogador
    for (int i = 0; i < NUM_JOGADORES; i++) {
        jogadores[i].missao = (char*)malloc(MAX_MISSAO_LEN * sizeof(char));
        if (jogadores[i].missao == NULL) {
            printf("Erro ao alocar missao para o Jogador %s.\n", jogadores[i].cor);
            liberar_memoria(mapa_territorios, jogadores, i);
            return 1;
        }
    }

    // Atribuicao e Exibicao da Missao
    for (int i = 0; i < NUM_JOGADORES; i++) {
        atribuirMissao(jogadores[i].missao, MISSOES, TOTAL_MISSOES);
        // MENSAGEM CORRIGIDA SEM ACENTO
        printf("\n=> MISSAO SORTEADA PARA O JOGADOR %s:\n", jogadores[i].cor); 
        exibirMissao(jogadores[i].missao);
    }
    printf("--------------------------------------------\n");
    
    // Cadastro inicial dos territorios
    cadastrar_territorios(mapa_territorios, num_territorios);

    printf("\n--- INICIO DA FASE DE ATAQUES ---\n");
    printf("Os ataques vao ocorrer entre os territorios cadastrados.\n\n");

    int indice_atacante = -1, indice_defensor = -1;
    char continuar_ataque = 's';
    int jogador_atual = 0; // 0 para Jogador A (VERMELHO), 1 para Jogador B (AZUL)

    // Loop principal de ataque
    while ((continuar_ataque == 's' || continuar_ataque == 'S')) {
        
        printf("\n--- TURNO DO JOGADOR %s ---\n", jogadores[jogador_atual].cor);
        exibir_territorios(mapa_territorios, num_territorios);

        // 1. Escolha do Territorio Atacante
        printf("\nESCOLHA O TERRITORIO ATACANTE:\n");
        if (!escolher_territorio(mapa_territorios, num_territorios, "atacante", &indice_atacante)) {
             limpar_buffer();
             continue; 
        }
        limpar_buffer();

        // 2. Escolha do Territorio Defensor
        printf("\nESCOLHA O TERRITORIO DEFENSOR:\n");
        if (!escolher_territorio(mapa_territorios, num_territorios, "defensor", &indice_defensor)) {
             limpar_buffer();
             continue; 
        }
        limpar_buffer();
        
        // Validacoes (mensagens sem acento)
        if (indice_atacante == indice_defensor) {
            printf("\nERRO: Nao e possivel atacar o proprio territorio.\n");
        } 
        else if (strcmp(mapa_territorios[indice_atacante].cor, mapa_territorios[indice_defensor].cor) == 0) {
            printf("\nERRO: Nao e possivel atacar um territorio de mesma cor (dono).\n");
        } 
        else if (strcmp(mapa_territorios[indice_atacante].cor, jogadores[jogador_atual].cor) != 0) {
            printf("\nERRO: Voce so pode atacar com seus territorios (%s).\n", jogadores[jogador_atual].cor);
        }
        else {
            // Executa o ataque
            atacar(&mapa_territorios[indice_atacante], &mapa_territorios[indice_defensor]);
            
            // Exibicao pos-ataque
            printf("\n--- DADOS ATUALIZADOS APOS O ATAQUE ---\n");
            exibir_territorios(mapa_territorios, num_territorios);

            // 3. Verificacao da Missao
            if (verificarMissao(jogadores[jogador_atual].missao, mapa_territorios, num_territorios, jogadores[jogador_atual].cor)) {
                 // MENSAGENS DE VITORIA SEM ACENTO
                 printf("\n*** CONDICAO DE VITORIA ATINGIDA! ***\n");
                 printf("O Jogador %s VENCEU O JOGO com a missao:\n", jogadores[jogador_atual].cor);
                 exibirMissao(jogadores[jogador_atual].missao);
                 continuar_ataque = 'n'; // Encerra o jogo
            } else {
                 jogador_atual = (jogador_atual + 1) % NUM_JOGADORES; 
            }
        }

        if (continuar_ataque != 'n' && continuar_ataque != 'N') {
            printf("\nDeseja realizar mais ataques (Continuar Turnos)? (S/N): ");
            scanf(" %c", &continuar_ataque);
            limpar_buffer();
        }
    }
    
    // Libera toda a memoria alocada dinamicamente
    liberar_memoria(mapa_territorios, jogadores, NUM_JOGADORES);

    printf("\nFim do programa. Memoria liberada com sucesso.\n");
    return 0;
}


// --- FUNCOES MODULARIZADAS DO NIVEL MESTRE/AVENTUREIRO ---

// Funcao padrao e segura para limpar o buffer de entrada
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

// Funcao para atribuir missao por sorteio
void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int indice_sorteado = rand() % totalMissoes;
    strcpy(destino, missoes[indice_sorteado]);
}

// Funcao para exibir missao
void exibirMissao(char* missao) {
    printf("   \"%s\"\n", missao);
}

// Funcao para verificar a missao (Logica de verificacao simples)
int verificarMissao(char* missao, Territorio* mapa, int tamanho, const char* cor_jogador) {
    int tropas_totais_jogador = 0;
    int territorios_com_5_tropas = 0;
    int tropas_inimigas_restantes = 0;
    
    for (int i = 0; i < tamanho; i++) {
        Territorio* t = (mapa + i); 
        if (strcmp(t->cor, cor_jogador) == 0) {
            tropas_totais_jogador += t->tropas;
            if (t->tropas >= 5) {
                territorios_com_5_tropas++;
            }
        } else {
            tropas_inimigas_restantes += t->tropas;
        }
    }

    // Logica de verificacao (baseada nas strings SEM ACENTO):
    if (strstr(missao, "MISSAO 1")) { // Ter 15 tropas
        return tropas_totais_jogador >= 15;
    }
    if (strstr(missao, "MISSAO 2")) { // 3 territorios com 5 tropas
        return territorios_com_5_tropas >= 3;
    }
    
    // Condicao de eliminacao de cor (MISSAO 3 e 4)
    if ((strcmp(cor_jogador, JOGADOR_A) == 0 && strstr(missao, "AZUL")) || 
        (strcmp(cor_jogador, JOGADOR_B) == 0 && strstr(missao, "VERMELHO"))) {
        
        return tropas_inimigas_restantes == 0;
    }

    return 0; 
}


// Funcao de Alocacao Dinamica (calloc/malloc)
Territorio* alocar_territorios(int *tamanho) {
    int n;
    // MENSAGEM CORRIGIDA SEM ACENTO
    printf("--- Configuracao Inicial ---\n");
    printf("Quantos territorios serao cadastrados? ");
    if (scanf("%d", &n) != 1) { 
        limpar_buffer();
        printf("Entrada invalida. Usando 4 territorios.\n");
        n = 4;
    }
    limpar_buffer();

    if (n <= 1) { 
        printf("Numero de territorios invalido. Minimo e 2. Usando 2.\n");
        n = 2; 
    }

    Territorio* mapa = (Territorio*)calloc(n, sizeof(Territorio));
    
    if (mapa == NULL) {
        *tamanho = 0;
        return NULL;
    }
    
    *tamanho = n; 
    // MENSAGEM CORRIGIDA SEM ACENTO
    printf("Memoria alocada para %d territorios.\n\n", n);
    return mapa;
}

// Funcao de Cadastro
void cadastrar_territorios(Territorio* mapa, int tamanho) {
    // MENSAGEM CORRIGIDA SEM ACENTO
    printf("--- Cadastro dos Territorios ---\n");
    for (int i = 0; i < tamanho; i++) {
        Territorio* t = mapa + i; 

        printf("-> Territorio %d de %d:\n", i + 1, tamanho);

        printf("   Nome (max. 29): ");
        scanf("%29s", t->nome); 

        printf("   Cor do exercito (%s ou %s): ", JOGADOR_A, JOGADOR_B);
        scanf("%9s", t->cor);

        printf("   Quantidade de tropas: ");
        scanf("%d", &(t->tropas));
        
        if (t->tropas <= 0) {
            t->tropas = 1; 
            printf("   *Tropas ajustadas para 1 (minimo).\n");
        }

        limpar_buffer(); 
        printf("\n");
    }
    printf("Cadastro finalizado.\n");
}

// Funcao de Exibicao
void exibir_territorios(Territorio* mapa, int tamanho) {
    printf("--- LISTA DE TERRITORIOS ---\n");
    for (int i = 0; i < tamanho; i++) {
        Territorio* t = (mapa + i); 
        printf("   [%d] %s (Cor: %s, Tropas: %d)\n", i, t->nome, t->cor, t->tropas);
    }
    printf("----------------------------\n");
}

// Funcao Auxiliar para Escolha e validacao
int escolher_territorio(Territorio* mapa, int tamanho, const char *tipo, int *indice_selecionado) {
    int indice;
    printf("   Digite o INDICE do territorio %s (0 a %d): ", tipo, tamanho - 1);
    
    if (scanf("%d", &indice) != 1) {
        printf("   Entrada invalida. Digite um numero.\n");
        return 0;
    }

    if (indice < 0 || indice >= tamanho) {
        printf("   Indice %s invalido.\n", tipo);
        return 0;
    }
    
    if (strcmp(tipo, "atacante") == 0 && mapa[indice].tropas < 2) {
        printf("   Territorio atacante deve ter pelo menos 2 tropas para atacar.\n");
        return 0;
    }
    
    *indice_selecionado = indice;
    return 1;
}

// Funcao Principal de Ataque 
void atacar(Territorio* atacante, Territorio* defensor) {
    int dado_ataque = (rand() % 6) + 1;
    int dado_defesa = (rand() % 6) + 1;

    printf("\n=== SIMULACAO DE ATAQUE ===\n");
    printf("Territorio Atacante: %s (Tropas: %d)\n", atacante->nome, atacante->tropas);
    printf("Territorio Defensor: %s (Tropas: %d)\n", defensor->nome, defensor->tropas);
    printf("Rolagem: Atacante tirou %d vs Defensor tirou %d\n", dado_ataque, dado_defesa);

    if (dado_ataque > dado_defesa) {
        // ATAQUE VENCEU
        printf("VITORIA DO ATACANTE! (%s conquista %s)\n", atacante->nome, defensor->nome);
        
        // Transfere a cor do atacante e metade das tropas
        strcpy(defensor->cor, atacante->cor); 
        int tropas_transferidas = atacante->tropas / 2;
        
        defensor->tropas = tropas_transferidas;
        atacante->tropas -= tropas_transferidas;
        
        printf("   >> Tropas transferidas: %d. Novo dono: %s.\n", tropas_transferidas, defensor->cor);

    } else { 
        // DEFEZA RESISTIU
        printf("DEFEZA RESISTIU! (%s manteve a posse)\n", defensor->nome);
        
        // Atacante perde uma tropa
        if (atacante->tropas > 1) {
            atacante->tropas--;
            printf("   >> O atacante perdeu 1 tropa. Tropas restantes: %d.\n", atacante->tropas);
        } else {
             printf("   >> O atacante nao tinha tropas suficientes para perder.\n");
        }
    }
}

// Funcao de Liberacao de Memoria (free)
void liberar_memoria(Territorio* mapa, Jogador *jogadores, int num_jogadores) {
    if (mapa != NULL) {
        free(mapa);
    }
    
    for (int i = 0; i < num_jogadores; i++) {
        if (jogadores[i].missao != NULL) {
            free(jogadores[i].missao);
        }
    }
    printf("\nMemoria dinamica liberada com sucesso.\n");
}