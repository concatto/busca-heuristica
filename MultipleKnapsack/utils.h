#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>


template <class Container>
void imprimirSequencia(const Container& sequencia, std::ostream& out = std::cout) {
	out << "[";
	for (auto it = std::begin(sequencia); it != std::end(sequencia); ++it) {
		if (it != std::begin(sequencia)) {
			out << ", ";
		}
		out << (*it);
	}
	out << "]\n";
}

float toFloat(const std::string& s) {
	return std::atof(s.c_str());
}

template <class T>
std::vector<T> fatiarString(const std::string& s, std::function<T(const std::string&)> conversor) {
	std::vector<T> resultado;

	int inicio = 0;
	do {
		int k = s.find(",", inicio);
		if (k != std::string::npos) {
			resultado.push_back(conversor(s.substr(inicio, k)));
			inicio = k + 1;
		} else {
			resultado.push_back(conversor(s.substr(inicio)));
			inicio = -1;
		}
	} while (inicio != -1);

	return resultado;
}

template <class Container>
std::vector<typename Container::value_type> embaralhar(const Container& container) {
	std::vector<typename Container::value_type> vec(container.begin(), container.end());
	std::random_shuffle(vec.begin(), vec.end());
	return vec;
}

std::vector<float> obterValoresArquivo(std::ifstream &entrada) {
	std::string linha;
	std::getline(entrada, linha);
	return fatiarString<float>(linha, toFloat);
}

struct Item {
	float peso;
	float valor;
	Item(float peso, float valor) : peso (peso), valor (valor) {}
};

using FloatMatrix = std::vector<std::vector<float>>;

struct Instancia {
	std::vector<Item> itens;
	std::vector<float> capacidades;
	FloatMatrix bonificacoes;

	Instancia(const std::string& arquivo) {
		std::ifstream entrada(arquivo);
		obterValoresArquivo(entrada); // ignorar primeira linha
		std::vector<float> valores = obterValoresArquivo(entrada);
		std::vector<float> pesos = obterValoresArquivo(entrada);

		capacidades = obterValoresArquivo(entrada);

		for (int i = 0; i < pesos.size(); i++) {
			itens.push_back({ pesos[i], valores[i] });
		}

		bonificacoes = FloatMatrix(valores.size(), std::vector<float>(valores.size(), 0));

		std::string linha;
		while (std::getline(entrada, linha)) {
			std::vector<float> bonus = fatiarString<float>(linha, toFloat);
			float a = bonus[0] - 1;
			float b = bonus[1] - 1;
			bonificacoes[a][b] = bonus[2];
			bonificacoes[b][a] = bonus[2];
		}
	}
};

#endif