#include <algorithm>
#include <conio.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stack>
#include <windows.h>
#include <map>

#define TOK_NUM 0
#define TOK_OP 1
#define TOK_PONT 2

#define CLS_PR 0 // Palavra Reservada
#define CLS_PT 1 // Pontuação
#define CLS_OP 2 // Operador
#define CLS_VL 3 // Valor Literal
#define CLS_TP 4 // Tipo
#define CLS_ID 5 // Identificardor (Funções e Variáveis)

using namespace std;

struct Token {
    string lexema; // !
    string valor;  // EXCLA
    int linha;     // 55
    int coluna;    // 10
    int classe;    // CLS_PR
} typedef Token;

struct PadraoToken
{
    int info;
    int classe;
    string palavra;
    string token;
} typedef PadraoToken;

void tokenizaStrings();
void retiraComentarios();
void tokenizaNumeros();
void tokenizarOperadores();
void tokenizaPalavraReservada();
void tokenizaPontuacao();
void sortTokens();

// Inicio Analisador Sintatico
struct NoArvore {
    Token * token;
    vector<struct NoArvore *> filhos;
    int tipoRetorno;
} typedef NoArvore;

struct Funcao {
    string nome;
    vector<int> tiposParametros;
    int tipoRetorno;
} typedef Funcao;

struct Variavel {
    string nome;
    int tipo;
    bool constante;
    bool global;
} typedef Variavel;

map<string, int> tipos;
NoArvore * raiz;
list<Funcao*> funcoes;
list<Variavel*> variaveis;
list<Token*>::iterator leitor;
map<int, list<PadraoToken*>> tabelaHashDicionario;
int tamanhoHash = 0;
bool flagDefineFuncao = false;
int retornoFuncaoEmDefinicao = 0;
list<string> pilhaIncluidos;

// Joabe
NoArvore* trataInclude();
NoArvore* trataIf();
NoArvore* trataSwitch();
NoArvore* trataBloco();
NoArvore* trataOperacao();
Token* lerToken(string);
Token* nextToken();
Token* prevToken();
void imprimeArvore(NoArvore* raiz, string outputPath, string indent);
void status();
void imprimeNoArvore(NoArvore *);

// Rafael
NoArvore* trataFor();
NoArvore* trataExpressao();
bool eOperador(string C);
bool eOperando(string C);
void verificarExpressaoBemFormada();
int pegarPesoDoOperador(string op);
NoArvore* novoNo(Token* token);

// Marcos
NoArvore* trataDo();
NoArvore* trataWhile();

// Varley
NoArvore* trataDeclaracaoVariavel();
NoArvore* trataDeclaracaoFuncao();
NoArvore* trataChamadaFuncao();
NoArvore* trataChamadaVariavel();
NoArvore* trataDeclaracaoConstante();
void saveError();
// Fim Analisador Sintatico

list<Token*> tokens;    // lista de tokens de acordo com as ocorrencias
string source_code;      // string contendo o codigo fonte do arquivo
string path;            // caminho do arquivo de entrada
string output;          // caminho do arquivo de saida
int size_code = 0;      // tamanho da string source_code

#include "jhuly_lohrrana.cpp"
#include "joabe.cpp"
#include "marcos.cpp"
#include "varley.cpp"
#include "rafael.cpp"
#include "fv.cpp"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
      cout << "Informe o arquivo a ser analisado:" << endl;
      cin >> path;

    }
    else
    {
        path = argv[1];
    }
    output = "";
    if (path.find(';') != string::npos)
    {
        output = path.substr(path.find(';')+1);
        path   = path.substr(0, path.find(';'));
    }
    //cout << tipos.find(1)->second;

    // Carrega o dicionario
    gera();
    gera_tipos();

    // Faz a leitura do arquivo
    if (!readFile(path))
        return 0;
    size_code = source_code.size();

    // Tokeniza strings literais e substitui por espaços
    tokenizaStrings();

    // Retira os comentarios e os substitui por espaços e/ou quebras de linha com o objetivo de manter as linhas e colunas para as proximas leituras
    retiraComentarios();
    size_code = source_code.size();

    // Tokeniza os numeros
    tokenizaNumeros();

    // Tokeniza os operadores
    tokenizarOperadores();

    // Tokeniza as palavras reservadas
    tokenizaPalavraReservada();

    // Tokeniza pontuacao
    tokenizaPontuacao();

    // Reordena a lista de tokens
    sortTokens();

    // Imprime os tokens
    //printTokens(output);
    //return 0;

    // Analise sintatica
    raiz = new NoArvore();
    raiz->token       = NULL;
    raiz->tipoRetorno = tipos.find("void")->second;
    leitor            = tokens.begin();
    pilhaIncluidos.push_back(path);

    // Posiciona o leitor no primeiro token valido, ignorando comentarios
    while ((*leitor)->valor == "NAO TOKEN")
        leitor++;

    do
    {
        // Ao encontrar um cerquilha invoca o tratamento de include
        if ((*leitor)->valor == "CERQUI")
        {
            nextToken();
            if ((*leitor)->valor == "INCLUD")
            {
                prevToken();
                raiz->filhos.push_back(trataInclude());
            }
            else if ((*leitor)->valor == "DEFINE")
            {
                prevToken();
                raiz->filhos.push_back(trataDeclaracaoConstante());
            }
            else
            {
                cout << "Erro fatal: Diretiva nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
                exit(0);
            }
        }
        // Ao encontrar um tipo, pode ser uma declaracao de variavel ou funcao
        else if ((*leitor)->classe == CLS_TP)
        {
            nextToken();
            // Pula o token de ponteiro
            if ((*leitor)->valor == "MULTPL")
                nextToken();
            // Pula o token de identificador
            lerToken("TD");
            // Caso encontre um abre parenteses invoca o tratamento de declaracoes de funcoes
            if ((*leitor)->valor == "ABPAR")
            {
                prevToken();
                prevToken();
                if ((*leitor)->valor == "MULTPL")
                    prevToken();

                raiz->filhos.push_back(trataDeclaracaoFuncao());
            }
            // Caso contrario invoca o tratamento de declaracao de variaveis
            else
            {
                prevToken();
                prevToken();
                if ((*leitor)->valor == "MULTPL")
                    prevToken();

                raiz->filhos.push_back(trataDeclaracaoVariavel());
            }
        }
        // Qualquer outra coisa é inesperada
        else
        {
            cout << "Erro fatal: Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            exit(0);
        }
    }
    while (leitor != tokens.end());

    // Impressão simploria da arvore gerada
    imprimeArvore(raiz, output);

    return 0;
}
