#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iterator>
#include "utils.h"

struct Mochila {
	float capacidadeMaxima;
	std::vector<Item> itens;
};

using Solucao = std::vector<int>;

class HeuristicaMochilas {
	Instancia inst;
	std::unordered_set<int> itensDisponiveis;
	std::unordered_set<int> itensPresentes;
	std::vector<float> pesosAtuais;

public:
	HeuristicaMochilas(const Instancia& instancia) : inst(instancia) {

	}

	float adicionarItem(Solucao &sol, int item, int mochila) {
		float valor = inst.itens[item].valor + obterBonificacoes(sol, item, 0);
		float peso = inst.itens[item].peso;
		pesosAtuais[mochila] += peso;
		sol[item] = mochila;
		itensPresentes.insert(item);
		itensDisponiveis.erase(itensDisponiveis.find(item));
		return valor;
	}

	float removerItem(Solucao &sol, int item, int mochila) {
		float valor = inst.itens[item].valor + obterBonificacoes(sol, item, 0);
		float peso = inst.itens[item].peso;
		pesosAtuais[mochila] -= peso;
		sol[item] = -1;
		itensDisponiveis.insert(item);
		itensPresentes.erase(itensPresentes.find(item));
		return valor;
	}

	float obterBonificacoes(const Solucao &sol, int item, int partida = 0) {
		float bonificacao = 0;
		for (int i = partida; i < sol.size(); i++) {
			if(sol[i] >= 0)
				bonificacao += inst.bonificacoes[item][i];
		}
		return bonificacao;
	}

	bool verificarAceitacao(float delta, float temperatura) {
		float prob = std::exp(delta) / temperatura;
		return (std::rand() / static_cast<float>(RAND_MAX)) < prob;
	}

	// Criar uma classe
	float tentarAdicionar(const Solucao& solucao, int& itemAdicionado, int& mochilaDestino) {
		for (int item : itensDisponiveis) {
			for (int mochila = 0; mochila < inst.capacidades.size(); mochila++) {
				// Se tornar inadmissivel, rejeitar
				if (pesosAtuais[mochila] + inst.itens[item].peso > inst.capacidades[mochila]) {
					continue;
				}

				float melhora = inst.itens[item].valor;
				melhora += obterBonificacoes(solucao, item);

				// As penalidades superam o valor do item
				if (melhora <= 0) {
					continue;
				}

				itemAdicionado = item;
				mochilaDestino = mochila;
				return melhora;
			}
		}

		return 0;
	}

	bool tentarTrocar(Solucao& sol, float &melhora) {
		if (itensPresentes.empty() || itensDisponiveis.empty()) {
			// ninguém pra remover ou pra pôr
			return false;
		}

		std::vector<int> itensPresentesVec = embaralhar(itensPresentes);

		while (!itensPresentesVec.empty()) {
			int itemRemovido = itensPresentesVec.back();
			itensPresentesVec.pop_back();

			int mochilaEsvaziada = sol[itemRemovido];
			float valorRemovido = inst.itens[itemRemovido].valor + obterBonificacoes(sol, itemRemovido, 0);
			sol[itemRemovido] = -1;

			for (auto it = itensDisponiveis.begin(); it != itensDisponiveis.end(); ++it) {
				int itemAdicionado = *it;

				for (int mochila = 0; mochila < inst.capacidades.size(); mochila++) {
					sol[itemAdicionado] = mochila;
					float valorAdicionado = inst.itens[itemAdicionado].valor + obterBonificacoes(sol, itemAdicionado, 0);
			
					//std::cout << "\nTirando " << itemRemovido << " da " << mochilaEsvaziada << " e colocando " << itemAdicionado << " na " << mochila << "\n";
					//imprimirSequencia(sol);
					int peso = pesosAtuais[mochila] + inst.itens[itemAdicionado].peso;
					melhora = valorAdicionado - valorRemovido;
					std::cout << "Melhora = " << melhora << "\n";
					if (melhora > 0 && peso <= inst.capacidades[mochila]) {
						pesosAtuais[mochila] = peso;
						pesosAtuais[mochilaEsvaziada] -= inst.itens[itemRemovido].peso;

						itensPresentes.insert(itemAdicionado);
						itensDisponiveis.erase(itemRemovido);

						return true;
					} else {
						std::cout << "Novo peso da mochila " << mochila << ": " << peso << " de " << inst.capacidades[mochila] << "\n";
					}

					sol[itemAdicionado] = -1;
				}
			}

			// ctrl+z
			sol[itemRemovido] = mochilaEsvaziada;
		}
	
		return false;
	}

	float aplicarBusca(Solucao& solucao) {
		float qualidade = 0;
		bool admissivel = true;
		pesosAtuais = std::vector<float>(inst.capacidades.size(), 0);
		itensDisponiveis.clear();
		
		for (int i = 0; i < solucao.size(); i++) {
			int mochila = solucao[i];
			if (mochila >= 0) {
				itensPresentes.insert(i);

				// Incremento na qualidade e nas capacidades
				qualidade += inst.itens[i].valor;
				pesosAtuais[mochila] += inst.itens[i].peso;

				// Verificação de admissibilidade
				if (pesosAtuais[mochila] > inst.capacidades[mochila] && admissivel) {
					admissivel = false;
				}
			
				// Cálculo das bonificações
				qualidade += obterBonificacoes(solucao, i, i+1);
			} else {
				itensDisponiveis.insert(i);
			}
		}

		while (!admissivel) {
			// Seleciona mochila estourada
			int mochila = -1, itemRemovido;
			for (int i = 0; i < itensPresentes.size(); i++) {
				auto it = itensPresentes.begin();
				std::advance(it, i);
				itemRemovido = *it;

				int candidata = solucao[itemRemovido];
				if(pesosAtuais[mochila] > inst.capacidades[mochila]) {
					mochila = candidata;
					break;
				}
			}
			if(mochila == -1) {
				admissivel = true;
			} else {
				// Atualiza capacidade e valor
				qualidade -= inst.itens[itemRemovido].valor;
				qualidade -= obterBonificacoes(solucao, itemRemovido);
				pesosAtuais[mochila] -= inst.itens[itemRemovido].peso;
				// Remove item
				solucao[itemRemovido] = -1;
				itensPresentes.erase(itensPresentes.find(itemRemovido));
				itensDisponiveis.insert(itemRemovido);
			}			
		}
		
		int itemAdicionado;
		int mochilaDestino;
		float melhora;
		while ((melhora = tentarAdicionar(solucao, itemAdicionado, mochilaDestino)) > 0) {
			solucao[itemAdicionado] = mochilaDestino;
			itensDisponiveis.erase(itemAdicionado);
			itensPresentes.insert(itemAdicionado);
			qualidade += melhora;
			pesosAtuais[mochilaDestino] += inst.itens[itemAdicionado].peso;
			//imprimirSequencia(solucao);
			std::cout << "Pesos atuais" << std::endl;
			//imprimirSequencia(pesosAtuais);
			std::cout << "Qualidade : " << qualidade << std::endl;
			std::cout << std::endl;
		}

		while(tentarTrocar(solucao, melhora)) {
			 qualidade += melhora;

			 while ((melhora = tentarAdicionar(solucao, itemAdicionado, mochilaDestino)) > 0) {
				solucao[itemAdicionado] = mochilaDestino;
				itensDisponiveis.erase(itemAdicionado);
				itensPresentes.insert(itemAdicionado);
				qualidade += melhora;
				pesosAtuais[mochilaDestino] += inst.itens[itemAdicionado].peso;
				//imprimirSequencia(solucao);
				std::cout << "Pesos atuais" << std::endl;
				//imprimirSequencia(pesosAtuais);
				std::cout << "Qualidade : " << qualidade << std::endl;
				std::cout << std::endl;
			}
		}
		return qualidade;
	}

	float tempera(Solucao &solucao, float temperaturaInicial, float alpha) {
		float qualidade = 0;
		pesosAtuais = std::vector<float>(inst.capacidades.size(), 0);
		itensDisponiveis.clear();
		std::unordered_set<int> estouradas;
		for (int i = 0; i < solucao.size(); i++) {
			int mochila = solucao[i];
			if (mochila >= 0) {
				itensPresentes.insert(i);

				// Incremento na qualidade e nas capacidades
				qualidade += inst.itens[i].valor;
				pesosAtuais[mochila] += inst.itens[i].peso;

				// Verificação de admissibilidade
				if (pesosAtuais[mochila] > inst.capacidades[mochila]) {
					estouradas.insert(mochila);
				}
			
				// Cálculo das bonificações
				qualidade += obterBonificacoes(solucao, i, i+1);
			} else {
				itensDisponiveis.insert(i);
			}
		}
	
		float temperatura = temperaturaInicial;
		while (temperatura >= 1) {
			// Se a solução é inadmissível
			if (!estouradas.empty()) {
				std::cout << "inadmissivel\n";
				// Remover um item aleatoriamente
				int indiceSorteado = std::rand() % itensPresentes.size();
				auto it = itensPresentes.begin();
				std::advance(it, indiceSorteado);
				int item = *it;
				int mochila = solucao[item];
		
				float delta = inst.capacidades[mochila] - pesosAtuais[mochila];
		
				// True se a mochila estiver estourada
				bool melhora = estouradas.find(mochila) != estouradas.end();
		
				// Se melhora ou se a têmpera disser pra aceitar
				if (melhora || verificarAceitacao(delta, temperatura)) {
					qualidade -= removerItem(solucao, item, mochila);
		
					if (pesosAtuais[mochila] < inst.capacidades[mochila]) {
						estouradas.erase(mochila);
					}
				}
			} else {
				std::cout << "admissivel\n";
				// Selecionar um item aleatório
				int indiceSorteado = std::rand() % itensDisponiveis.size();
				auto it = itensDisponiveis.begin();
				std::advance(it, indiceSorteado);
				int item = *it;
		
				// Selecionar uma mochila aleatória para inserir o item
				int mochila = std::rand() % pesosAtuais.size();
		
				std::cout << "Tentando adicionar " << item << " na mochila " << mochila << "\n";
				// Se não vai estourar
				if (pesosAtuais[mochila] + inst.itens[item].peso < inst.capacidades[mochila]) {
					float delta = adicionarItem(solucao, item, mochila);
					if (delta < 0) {
						std::cout << "Piorou\n";
						// Piorou!
						if (!verificarAceitacao(delta, temperatura)) {
							// Rejeitou, desfazer a adição
							std::cout << "Rejeitou. Desfazendo\n";
							removerItem(solucao, item, mochila);
							delta = 0;
						} else {
							std::cout << "Aceitou mesmo assim.\n";
						}
					} else {
						std::cout << "Melhorou!\n";
					}
					qualidade += delta;		
				} else {
					std::cout << "Rejeitado porque estourou.\n";
				}
			}	
			temperatura *= alpha;
		}
		return qualidade;
	}
};

int main(int argc, char** argv) {
	std::srand(std::time(nullptr));

	Instancia inst("instances/mchls50_4_7.lia");
	imprimirSequencia(inst.capacidades);
	HeuristicaMochilas heuristicaMochilas(inst);
	Solucao sol(inst.itens.size(), -1);
	double qualidade = heuristicaMochilas.tempera(sol, 100, 0.99);
	std::cout << "Qualidade obtida: " <<  qualidade << std::endl;
	imprimirSequencia(sol);
	return 0;
}