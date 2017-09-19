#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <functional>
#include <utility>
#include <cmath>

struct Item {
    double valor;
    double peso;
    Item(double valor, double peso) : valor(valor), peso(peso) {

    }
};

using Solucao = std::vector<int>;
using Objetivo = std::function<double(const Solucao& sol)>;

Solucao gerarSolucaoAleatoria(const std::vector<Item> &itens) {
    Solucao solucao;
    for(int i = 0; i < itens.size(); i++)
        solucao.push_back(std::rand()%2);
    return solucao;
}

double avaliarSolucao(const Solucao &sol, const std::vector<Item> &itens, double capacidade) {
    double sumPeso = 0;
    double sumValor = 0;
    for(int i = 0; i < itens.size(); i++) {
        if(sol[i] == 0)
            continue;
        sumPeso += itens[i].peso;
        sumValor += itens[i].valor;
    }
    if(sumPeso > capacidade)
        return capacidade - sumPeso;

    return sumValor;
}

Solucao buscarMelhorVizinho(const Solucao &sol, Objetivo funcaoObjetivo) {
    Solucao melhor = sol;
    bool admissivel = funcaoObjetivo(sol) >= 0;

    for (int i = 0; i < sol.size(); i++) {
        Solucao vizinha = sol;

        if (admissivel && sol[i] == 0) {
            //Se estiver abaixo da capacidade, tentar incluir
            vizinha[i] = 1;
        } else if (!admissivel && sol[i] == 1) {
            //Se estiver acima da capacidade, tentar remover
            vizinha[i] = 0;
        }

        if (funcaoObjetivo(vizinha) >= funcaoObjetivo(melhor)) {
            melhor = vizinha;
        }
    }
    return melhor;
}

Solucao melhorMelhoraMochila(double capacidadeMaxima, const std::vector<Item> &itens) {
    //Definiçao da funçao objetivo utilizando lambda
    Objetivo objetivo = [&](const Solucao &sol) {
        return avaliarSolucao(sol, itens, capacidadeMaxima);
    };

    Solucao atual = gerarSolucaoAleatoria(itens);
    double qualidadeAtual = objetivo(atual);

    while (true) {
        atual = buscarMelhorVizinho(atual, objetivo);
        double q = objetivo(atual);

        if (q == qualidadeAtual) {
            //Nao conseguiu melhorar
            return atual;
        } else {
            //Achou uma soluçao melhor
            qualidadeAtual = q;
        }
    }
}

Solucao forcaBruta(double capacidadeMaxima, const std::vector<Item> &itens) {
    Solucao melhor(itens.size(), 0);
    double qualidade = 0;
    long int combinacoes = std::pow(2, itens.size());
    for(int i = 0; i < combinacoes; i++) {
        Solucao vizinha;
        vizinha.resize(itens.size());
        for(int j = 0; j < itens.size(); j++) {
            vizinha[j] = (i >> j) & 1;
        }
        double q = avaliarSolucao(vizinha, itens, capacidadeMaxima);
        if(q > qualidade) {
            melhor = vizinha;
            qualidade = q;
        }
    }
    return melhor;
}

int main()
{
    std::srand(std::time(nullptr));
    double capacidadeMaxima = 180.5;
    std::vector<Item> itens{
        {29, 23},
        {18, 12},
        {5, 25},
        {65, 46},
        {20, 42},
        {15, 15},
        {23, 43},
        {68, 32},
        {35, 25},
        {45, 36},
        {120, 72},
        {25, 25},
        {58, 32},
        {15, 5},
        {75, 46},
        {20, 22},
        {105, 55}
    };
    Solucao melhor = forcaBruta(capacidadeMaxima, itens);

    std::cout << "Melhor solucao: \t";
    std::cout << "[ ";
    double somaPeso = 0;
    for(int i = 0; i < melhor.size(); i++) {
        std::cout << melhor[i] << " ";
        if(melhor[i] == 1)
            somaPeso += itens[i].peso;
    }
    std::cout << "]\n";
    std::cout << "Avaliacao: R$" << avaliarSolucao(melhor, itens, capacidadeMaxima) << "\n";
    std::cout << "Peso: " << somaPeso << "kg \n";

    Solucao melhorMelhora = melhorMelhoraMochila(capacidadeMaxima, itens);
    int tentativas = 1;
    while(avaliarSolucao(melhor, itens, capacidadeMaxima) != avaliarSolucao(melhorMelhora, itens, capacidadeMaxima)) {
        melhorMelhora = melhorMelhoraMochila(capacidadeMaxima, itens);
        tentativas++;
    }
    std::cout << "Tentativas: " << tentativas << "\n";
}
