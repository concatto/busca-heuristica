#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>

struct Item {
	float peso;
	float valor;
	Item(float peso, float valor) : peso (peso), valor (valor) {}
};

struct Mochila {
	float capacidadeMaxima;
	std::vector<Item> itens;
};

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

int main(int argc, char** argv) {
	std::vector<Mochila> mochilas;
	std::vector<Item> itens;

	int matriz[mochilas.size()][itens.size()];

	std::vector<float> fatias = fatiarString<float>("10,2,3", [&](const std::string& s) { return std::atof(s.c_str()); });
	for (float f : fatias) {
		std::cout << f << "\n";
	}

	std::cout << "Hello, world\n";
	return 0;
}