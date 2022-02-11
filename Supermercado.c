#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	int PDVsTamanho;
	int *PDVs;
	int PDV_instalado_tamanho;
    int *PDV_instalado;
	int novos_PDVs_tamanho;
	int *novos_PDVs;
	int medida_de_agilidade;
	int tempos_limites[3];
} Setup_sistema;

typedef struct {
	char tipo;
	double tempo;
	int qtdeItens;
	int tipoCliente;
	int tempoPagamento;
	int PDV;
	int duracaoSuspensao;
} Evento;

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
					printf("%c - %f", evento.tipo, evento.tempo);
					i--;
				} else if(count == 2){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.qtdeItens = atoi(str);
					printf(" %d", evento.qtdeItens);
					i--;
				} else if(count == 3){
					evento.tipoCliente = linha[i] -'0';
					printf(" %d", evento.tipoCliente);
				} //ta dando erro
				else if(count == 4){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.tempoPagamento = atoi(str);
					printf(" %d", evento.tempoPagamento);
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
					printf("%c - %f", evento.tipo, evento.tempo);
					i--;
				} else if(count == 2){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.PDV = atoi(str);
					printf(" %d", evento.PDV);
					i--;
				} else if(count == 3){
					char *str = (char *) malloc(sizeof(char));
					for(int a = i; c != ' '; i++){
						str = (char *) realloc(str, ((i-a) + 1) * sizeof(char));
						str[i-a] = c;
						c = linha[i+1];
					}
					evento.duracaoSuspensao = atoi(str);
					printf(" %d", evento.duracaoSuspensao);
					i--;
				}
			}else{
				count++;
			}
		}
	}
	printf("\n--------------\n");
	return evento;
}

void PDVsInstaldos(Setup_sistema *setup, char *linha){
	setup->PDVs = (int *) malloc(sizeof(int));
	setup->PDVs[0] = linha[0] - '0';
	setup->PDVsTamanho = 1;
	char c = linha[1];
	for(int i = 1; c != '\n'; i++){
		if(c != ' '){
			setup->PDVs = (int *) realloc(setup->PDVs, (i + 1) * sizeof(int));
			setup->PDVs[i] = c - '0';
			setup->PDVsTamanho++;
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

// Pontos de Venda (PDVs)
// atende clientes segundo uma polı́tica Primeiro a Chegar é o Primeiro a ser Atendido (fila)
// Os dados disponibilizados são estes:
// O número de PDVs, atualmente instalados;
// O fator de agilidade (FA) de cada operador(a) de caixa, variado entre [1,10]. Um FA igual a 1 indica máxima agilidade;

// TAD AGENDA

// TAD evento

int main(int argc, char const argv[]) {
	Setup_sistema setup;
	FILE *arq;
	Evento *evento = (Evento*) malloc(sizeof(Evento));
	int tamEvento = 0;
	char Linha[100];
	char *result;
	int i;

	arq = fopen("ent01.in", "rt");

	if (arq != NULL) {
		i = 1;
		while (!feof(arq)) {
			result = fgets(Linha, 50, arq);
			if (result){
				if(i == 1){
					PDVsInstaldos(&setup, Linha);
				} else if(i == 2){
					PDVsNovos(&setup, Linha);
					printf("%s-> %d\n", Linha, setup.PDVs[setup.PDVsTamanho - 1]);
				} else if(i == 3){
					medida_de_agilidade_sistema(&setup, Linha);
				} else if(i == 4){
					tempos_limites_sistema(&setup, Linha);
				}else{
					char character = Linha[0];
					if(character != 'F'){
						evento = (Evento*) realloc(evento, (tamEvento + 1) * sizeof(Evento));
						tamEvento++;
						evento[tamEvento-1] = criar_evento(Linha);
					}
				}
			}
			i++;
		}

	fclose(arq);
	}
/*
	// code
	agenda = criarAgenda();
	novo = lerDados(ent01.in);

	evento = criarEvento(novo->tipo, novo->tempo, novo);
	agendar(agenda, evento);

	while (!vaziaAgenda(agenda)) {
	evento = proximoevento(agenda);
	relogio = evento->tempo;
	if (evento->tipo == 'C'){
	// acões relacionada ao evento chegada de cliente
	// inserir na fila
	cliente = evento->carga;
	inserirNaFila(clientes, cliente);
	if (tamanhoFila(clientes) == 1)&&(CAIXALIVRE(PDVs)){
	evento = criarEvento('I', relogio, NULL);
	agendar(agenda, evento);
	}
	}else if(evento->tipo == 'S'){
	// acões relacionadas ao evento suspensao de atendimento
	pdv = evento->carga;
	pdv->status = 'S';
	// Precisa completar as ações de suspensão de ATENDIMENTO
	// Agendar quando o caixa retoma o atendimento

	}else if(evento->tipo == 'I'){
	// descobrir o PDV ocioso (RR)
	pdv=proximoPDV(PDV);
	if (pdv!=NULL){
	// retirar cliente;
	cliente = desenfileirar(clientes)

	///agendar o final do atendimento
	tempo = relogio + passarCompras + pagamento;
	evento=criarEvento('F', tempo, pdv);
	agendar(agenda, evento);
	}
	}else if(evento->tipo == 'F'){ // final de atendimento
	// evento de final de atendimento
	/// cliente = desenfileirar(clientes)
	pdv = evento->carga;
	pdv->status = 'L';
	if (!vazia(clientes)){
	evento = criarEvento('I', relogio, NULL);
	agendar(agenda, evento);
	}
	}
	// code 
	}
*/
	return 0;
} 