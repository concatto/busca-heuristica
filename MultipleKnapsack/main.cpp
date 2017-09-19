#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iterator>
#include "utils.h"

struct Mochila {
	float capacidadeMaxima;
	std::vector<Item> itens;
};

using Solucao = std::vector<int>;

bool tentarAdicionar(const Instancia& inst, const Solucao& solucao, std::vector<int> itensDisponiveis, std::vector<float> pesosAtuais, int& itemAdicionado, int& mochilaDestino) {
	for (int item : itensDisponiveis) {
		for (int mochila = 0; mochila < inst.capacidades.size(); mochila++) {
			// Se tornar inadmissivel, rejeitar
			if (pesosAtuais[mochila] + inst.itens[item].peso > inst.capacidades[mochila]) {
				continue;
			}

			float bonificacoesAtuais = 0;
			for (int i = 0; i < solucao.size(); i++) {
				if (solucao[i] >= 0) {
					bonificacoesAtuais += inst.bonificacoes[item][i];
				}
			}

			// As penalidades superam o valor do item
			if (bonificacoesAtuais + inst.itens[item].valor <= 0) {
				continue;
			}

			itemAdicionado = item;
			mochilaDestino = mochila;
			return true;
		}
	}

	return false;
}

void aplicarBusca(const Instancia& inst, const Solucao& solucao) {
	float qualidade = 0;
	bool admissivel = true;
	std::vector<float> pesosAtuais(inst.capacidades.size(), 0);
	std::unordered_set<int> itensDisponiveis;

	for (int i = 0; i < solucao.size(); i++) {
		int mochila = solucao[i];

		if (mochila >= 0) {
			// Incremento na qualidade e nas capacidades
			qualidade += inst.itens[i].valor;
			pesosAtuais[mochila] += inst.itens[i].peso;

			// Verificação de admissibilidade
			if (pesosAtuais[mochila] > inst.capacidades[mochila] && admissivel) {
				admissivel = false;
			}

			// Cálculo das bonificações
			for (int j = i + 1; j < solucao.size(); j++) {
				if (solucao[j] >= 0) {
					qualidade += inst.bonificacoes[i][j];
				}
			}
		} else {
			itensDisponiveis.insert(i);
		}
	}

	while (true) {
		if (admissivel) {

		}
	}	
}


int main(int argc, char** argv) {
	Instancia inst("in.txt");
	imprimirSequencia(inst.capacidades);

	return 0;
}