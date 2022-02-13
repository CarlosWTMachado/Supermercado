#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	int PDVsTamanho;
	int *PDVs;
	int *PDVsStatus;
	int **PDVsTemposAtendimento; // 0 - livre; 1 - ocupado
	int medida_de_agilidade;
	int tempos_limites[3];
} Setup_sistema;

typedef struct {
	int tipo;
	double tempo;
	int tMax;
	int tTotal;
} Desistencia;

typedef struct {
	char tipo;
	/*
		C - chegada cliente
		S - suspensao PDV
		R - retorno suspensao
	*/
	double tempo;
	int qtdeItens;
	int tipoCliente;
	int tempoPagamento;
	int PDV;
	int duracaoSuspensao;
} Evento;

typedef struct Ag{
	double id;
	Evento evento;
	struct Ag *nextAgenda;
} Agenda;

typedef struct {
	int tamanho;
	Agenda *agenda;
} Lista;

Lista criar_lista(){
	Lista lista;
	lista.tamanho = 0;
	lista.agenda = NULL;
	return lista;
}

void inserir_lista(Lista *lista, Agenda agenda){
	lista->tamanho++;
	Agenda *atual = lista->agenda;
	Agenda *nova = (Agenda*) malloc(sizeof(Agenda));
	nova->evento = agenda.evento;
	nova->id = agenda.id;
	nova->nextAgenda = agenda.nextAgenda;
	if(atual == NULL){
		lista->agenda = nova;
	} else {
		while(atual->nextAgenda != NULL){
			if(atual->nextAgenda->id > nova->id) break;
			else atual = atual->nextAgenda;
		}
		if(atual->nextAgenda == NULL){
			atual->nextAgenda = nova;
		} else{
			nova->nextAgenda = atual->nextAgenda;
			atual->nextAgenda = nova;
		}
	}
}

Agenda remover_lista(Lista *lista){
	Agenda *remover = lista->agenda;
	lista->agenda = lista->agenda->nextAgenda; 
	lista->tamanho--;
	return *remover;
}

void imprimir_lista(Lista *lista){
	Agenda *atual = lista->agenda;
	printf("Lista: %d \n", lista->tamanho);
	for(int i = 0; (i < lista->tamanho && atual != NULL); i++){
		printf("agenda: %c - %f\n", atual->evento.tipo, atual->id);
		atual = atual->nextAgenda;
	}
	printf("-+-+-+-+-+-");
}

Agenda criar_agenda(Evento evento){
	Agenda agenda;
	agenda.id = evento.tempo;
	agenda.evento = evento;
	agenda.nextAgenda = NULL;
	return agenda;
}

Evento criar_evento(char *linha){
	Evento evento;
	char c = linha[0];
	int count = 0;
	evento.tipo = c;
	evento.tempo = 0.0;
	evento.qtdeItens = 0;
	evento.tipoCliente = 0;
	evento.tempoPagamento = 0;
	evento.PDV = 0;
	evento.duracaoSuspensao = 0;
	if(c == 'C'){
		//Tipo Tempo qtdeItens tipoCliente tempoPagamento(ms)
		for(int i = 1; c != '\n'; i++){
			c = linha[i];
			if(c != ' '){
				if(count == 1){
					char *tempo = (char *) malloc(sizeof(char));
					char *ptr;
					for(int a = i; c != ' '; i++){
						tempo = (char *) realloc(tempo, ((i-a) + 1) * sizeof(char));
						tempo[i-a] = c;
						c = linha[i+1];
					}
					evento.tempo = strtod(tempo, &ptr);
					i--;
				} else if(count == 2){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.qtdeItens = atoi(str);
					i--;
				} else if(count == 3){
					evento.tipoCliente = linha[i] -'0';
				} //ta dando erro
				else if(count == 4){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != '\n'; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.tempoPagamento = atoi(str);
					i--;
				}
			}else{
				count++;
			}
		}
	}else if(c == 'S'){
		//Tipo tempo PDV duração_da_suspensão(minutos)
		for(int i = 1; c != '\n'; i++){
			c = linha[i];
			if(c != ' '){
				if(count == 1){
					char *tempo = (char *) malloc(sizeof(char));
					char *ptr;
					for(int a = i; c != ' '; i++){
						tempo = (char *) realloc(tempo, ((i-a) + 1) * sizeof(char));
						tempo[i-a] = c;
						c = linha[i+1];
					}
					evento.tempo = strtod(tempo, &ptr);
					i--;
				} else if(count == 2){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.PDV = atoi(str);
					i--;
				} else if(count == 3){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != '\n'; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.duracaoSuspensao = atoi(str);
					i--;
				}
			}else{
				count++;
			}
		}
	}
	return evento;
}

void PDVsInstalados(Setup_sistema *setup, char *linha){
	setup->PDVs = (int *) malloc(sizeof(int));
	setup->PDVsStatus = (int *) malloc(sizeof(int));
	setup->PDVsTemposAtendimento = (int **) malloc(sizeof(int*));
	setup->PDVs[0] = linha[0] - '0';
	setup->PDVsStatus[0] = 0;
	setup->PDVsTemposAtendimento[0] = (int *) malloc(sizeof(int));
	setup->PDVsTemposAtendimento[0][0] = 0;
	setup->PDVsTamanho = 1;
	char c = linha[1];
	int j = 1;
	for(int i = 1; c != '\n'; i++){
		if(c != ' '){
			setup->PDVs = (int *) realloc(setup->PDVs, (j+1) * sizeof(int));
			setup->PDVs[j] = c - '0';
			setup->PDVsStatus = (int *) realloc(setup->PDVsStatus, (j+1) * sizeof(int));
			setup->PDVsStatus[j] = 0;
			setup->PDVsTemposAtendimento = (int **) realloc(setup->PDVsTemposAtendimento, (j+1) * sizeof(int*));
			setup->PDVsTemposAtendimento[j] = (int *) malloc(sizeof(int));
			setup->PDVsTemposAtendimento[j][0] = 0;
			printf("/*/* %d %d */*/", j, setup->PDVsTemposAtendimento[j][0]);
			setup->PDVsTamanho++;
			j++;
		}
		c = linha[i+1];
	}
}

void PDVsNovos(Setup_sistema *setup, char *linha){
	char c = linha[0];
	for(int i = 0; c != '\n'; i++){
		if(c != ' '){
			setup->PDVs = (int*) realloc(setup->PDVs, ((setup->PDVsTamanho+1) * sizeof(int)));
			setup->PDVs[setup->PDVsTamanho] = c - '0';
			setup->PDVsStatus = (int*) realloc(setup->PDVsStatus, ((setup->PDVsTamanho+1) * sizeof(int)));
			setup->PDVsStatus[setup->PDVsTamanho] = 0;
			setup->PDVsTemposAtendimento = (int **) realloc(setup->PDVsTemposAtendimento, ((setup->PDVsTamanho+1) * sizeof(int*)));
			setup->PDVsTemposAtendimento[setup->PDVsTamanho] = (int *) malloc(sizeof(int));
			setup->PDVsTemposAtendimento[setup->PDVsTamanho][0] = 0;
			setup->PDVsTamanho++;
		}
		c = linha[i+1];
	}
}

void medida_de_agilidade_sistema(Setup_sistema *setup, char *linha){
	setup->medida_de_agilidade = 0;
	for(int i = 0; i < (strlen(linha) - 1); i++){
		setup->medida_de_agilidade = (int) setup->medida_de_agilidade + ((linha[i] - '0') * pow(10, strlen(linha) - 2 - i));
	}
}

void tempos_limites_sistema(Setup_sistema *setup, char *linha){
	int flag = 0;
	int count = 0;
	setup->tempos_limites[flag] = 0;
	for(int i = 0; linha[i] != '\n'; i++){
		if(linha[i] == ' '){
			flag++;
			setup->tempos_limites[flag] = 0;
			count = 0;
		} else{
			setup->tempos_limites[flag] = setup->tempos_limites[flag] * pow(10, count);
			setup->tempos_limites[flag] = setup->tempos_limites[flag] + (linha[i] - '0');
			count++;
		}
	}
}

typedef struct {
	int tipo;
	double tempo;
	int tMax;
	int tTotal;
} Desistencia;

Desistencia criar_desistencia(int tipo, double tempo, int tMax, int tTotal){
	Desistencia desistencia;
	desistencia.tipo = tipo;
	desistencia.tempo = tempo;
	desistencia.tMax = tMax;
	desistencia.tTotal = tTotal;
	return desistencia;
}

int tempo_atendimento(Setup_sistema *setup, Evento evento, int tempoFila, Desistencia *desistencias, int *desistenciasTam){
	int atendimento = (setup->medida_de_agilidade * setup->PDVs[evento.PDV-1] * evento.qtdeItens) + 
	(setup->medida_de_agilidade * setup->PDVs[evento.PDV-1] * evento.qtdeItens);
	if(evento.tipoCliente == 1){
		setup->PDVsTemposAtendimento[evento.PDV-1][0]++;
		setup->PDVsTemposAtendimento[evento.PDV-1] = realloc(setup->PDVsTemposAtendimento[evento.PDV-1], (setup->PDVsTemposAtendimento[evento.PDV-1][0] + 1) * sizeof(int));
		setup->PDVsTemposAtendimento[evento.PDV-1][setup->PDVsTemposAtendimento[evento.PDV-1][0]] = atendimento + tempoFila;
		return 1;
	}else if(evento.tipoCliente == 2){ //tipo 2 espera, no maximo, X minutos no Expresso (Fila + atendimento), e depois abandona as compras.
		if(atendimento + tempoFila > (setup->tempos_limites[0] * 60000)){
			*desistenciasTam++;
			desistencias = (Desistencia*) realloc(desistencias, (*desistenciasTam) * sizeof(Desistencia));
			desistencias[*desistenciasTam-1] = criar_desistencia(evento.tipoCliente, evento.tempo, (setup->tempos_limites[0] * 60000), atendimento + tempoFila);
			return 0;
		}else{
			setup->PDVsTemposAtendimento[evento.PDV-1][0]++;
			setup->PDVsTemposAtendimento[evento.PDV-1] = realloc(setup->PDVsTemposAtendimento[evento.PDV-1], (setup->PDVsTemposAtendimento[evento.PDV-1][0] + 1) * sizeof(int));
			setup->PDVsTemposAtendimento[evento.PDV-1][setup->PDVsTemposAtendimento[evento.PDV-1][0]] = atendimento + tempoFila;
			return 1;
		}
	}else if(evento.tipoCliente == 3){ //tipo 3 vai esperar, no maximo, Y minutos na FILA, e, no maximo, Z minutos no atendimento, e depois abandona a compra
		if(tempoFila <= (setup->tempos_limites[1] * 60000) && atendimento <= (setup->tempos_limites[2] * 60000)){
			setup->PDVsTemposAtendimento[evento.PDV-1][0]++;
			setup->PDVsTemposAtendimento[evento.PDV-1] = realloc(setup->PDVsTemposAtendimento[evento.PDV-1], (setup->PDVsTemposAtendimento[evento.PDV-1][0] + 1) * sizeof(int));
			setup->PDVsTemposAtendimento[evento.PDV-1][setup->PDVsTemposAtendimento[evento.PDV-1][0]] = atendimento + tempoFila;
			return 1;
		}else return 0;
	}
	return 0;
}

int findDestroy(Lista *lista, int pdv, char tipo){
	Agenda *atual = lista->agenda;
	for(int i = 0; i < lista->tamanho-1;i++){
		if(atual->nextAgenda->evento.PDV == pdv && atual->nextAgenda->evento.tipo == tipo){
			atual->nextAgenda = atual->nextAgenda->nextAgenda;
			return 1;
		}
	}
	return 0;
}

int main(int argc, char const argv[]) {
	Setup_sistema setup;
	FILE *arq;
	Lista lista = criar_lista();
	char Linha[100];
	char *result;
	int i;
	Desistencia *desistencias = (Desistencia*) malloc(sizeof(Desistencia));
	int desistenciasTam = 0;
	arq = fopen("ent01.in", "rt");

	if (arq != NULL) {
		i = 1;
		while (!feof(arq)) {
			result = fgets(Linha, 50, arq);
			if (result){
				if(i == 1){
					PDVsInstalados(&setup, Linha);
				} else if(i == 2){
					PDVsNovos(&setup, Linha);
				} else if(i == 3){
					medida_de_agilidade_sistema(&setup, Linha);
				} else if(i == 4){
					tempos_limites_sistema(&setup, Linha);
				}else{
					char character = Linha[0];
					if(character != 'F'){
						Evento evento = criar_evento(Linha);
						Agenda agenda = criar_agenda(evento);
						inserir_lista(&lista, agenda);
					}
				}
			}
			i++;
		}
	fclose(arq);
	}
	for(int i = 0; i < lista.tamanho; i++){
		if(lista.agenda->evento.tipo == 'C'){
			int j;
			for(j = 0; setup.PDVsStatus[j] != 0; j++);
			if(j == setup.PDVsTamanho){

			}else{
				setup.PDVsStatus[j] = 1;
				lista.agenda->evento.PDV = j+1;
				Evento evento;
				evento.tipo = 'R';
				evento.tempo = lista.agenda->evento.tempo /* + o tempo que demorara */;
				evento.PDV = j+1;
				Agenda agenda = criar_agenda(evento);
				if(lista.agenda->evento.tipoCliente == 1){
					tempo_atendimento(&setup, lista.agenda->evento, 0, &desistencias, &desistenciasTam);
				}
				inserir_lista(&lista, agenda);
				i--;
			}
		} else if(lista.agenda->evento.tipo == 'R'){
			setup.PDVsStatus[lista.agenda->evento.PDV - 1] = 0;
		} else if(lista.agenda->evento.tipo == 'S'){
			if(setup.PDVsStatus[lista.agenda->evento.PDV - 1] == 1) findDestroy(&lista, lista.agenda->evento.PDV, 'R');
			setup.PDVsStatus[lista.agenda->evento.PDV - 1] = 1;
			Evento evento;
			evento.tipo = 'R';
			evento.tempo = lista.agenda->evento.tempo + (lista.agenda->evento.duracaoSuspensao * 60);
			evento.PDV = lista.agenda->evento.PDV;
			Agenda agenda = criar_agenda(evento);
			inserir_lista(&lista, agenda);
			i--;
		}
		remover_lista(&lista);
		imprimir_lista(&lista);
	}
	return 0;
} 