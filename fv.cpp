#include <sstream>
#include <iostream>
#include <list>
#include <time.h>
#include <fstream>

//#define IBPP_WINDOWS = 1
//#define IBPP_GCC = 1

//#include "ibpp-2-0-5-1-bin-bcc/ibpp.h"

using namespace std;
list<Funcao*>::iterator itFunc;
list<Variavel*>::iterator itVar;
Funcao* tempF;
Variavel* tempV;
NoArvore* noDF;
bool achouFuncao = false;
bool varGlobal = true;
int cfi = 0;

string stringalizar(int inteiro)
{
    ostringstream resultado;
    resultado << inteiro;
    return resultado.str();
}

string divDataHora(string dataHoraDesorg)
{
    string data, hora, ano;

    for ( int i = 0; i < dataHoraDesorg.length(); i++)
    {
        if(i >= 0 && i <= 9)
            data += dataHoraDesorg[i];
        if(i >= 11 && i <= 18)
            hora += dataHoraDesorg[i];
        if (i >= 20 && i <= 23)
            ano += dataHoraDesorg[i];
    }

    return data+" "+ano+" / "+hora;

}

void saveError()
{

    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    string dataHora = asctime(timeinfo);
    string dados = '\n'+(*leitor)->lexema+'\t'+(*leitor)->valor+'\t'+stringalizar((*leitor)->linha)+'\t'+stringalizar((*leitor)->coluna)+'\t'+divDataHora(dataHora);
    ofstream arquivoSaida;
    arquivoSaida.open("ERROS.txt", ios_base::app);
    if(arquivoSaida.is_open() && arquivoSaida.good())
    {
        arquivoSaida << dados << endl;
        //arquivoSaida.close();
    }
}

void erro()
{
    saveError();
    cout<<"Errinho: Expressao nao esperada \""<<(*leitor)->valor<<"\" na linha "<<(*leitor)->linha<<", coluna "<<(*leitor)->coluna<<", Arquivo: "<<path<<endl;
    exit(0);
}

void getFuncao()
{
    itFunc=funcoes.begin();
    int i = 0;
    do
    {
        if((*leitor)->lexema==(*itFunc)->nome)
        {
            (*leitor)->valor = "FUNCAO";
            (*leitor)->classe = CLS_ID;
            noDF->token=(*leitor);
            noDF->tipoRetorno=(*itFunc)->tipoRetorno;
            achouFuncao=true;
            cfi++;
            if(cfi>=1)
            {
                saveError();
                cout<<"ERRO: Funcao \""<<(*leitor)->lexema<<"\" repitida na linha "<<(*leitor)->linha<<", coluna "<<(*leitor)->coluna<<"."<<endl;
                exit(0);
            }
        }
        if((*itFunc)!=funcoes.back())
            itFunc++;
        i++;
    }
    while(i != (int)funcoes.size());
}
NoArvore* trataParametrosDF()
{
    NoArvore* noFilho = new NoArvore();
    string asterisco = "";
    string tipoAsterisco = "";
    if((*leitor)!=tokens.back())
    {
        nextToken();
    }
    else
    {
        erro();
    }
    if((*leitor)->valor=="MULTPL")
    {
        asterisco+= (*leitor)->lexema;
        prevToken();
        tipoAsterisco = (*leitor)->lexema + asterisco;
    }
    else
    {
        prevToken();
        tipoAsterisco = (*leitor)->lexema;
        //nextToken();
    }
    if((*leitor)->classe==CLS_TP && tipoAsterisco==tipos.find(tipoAsterisco)->first)
    {
        noFilho->tipoRetorno=tipos.find(tipoAsterisco)->second;
        noFilho->token=(*leitor);
        tempF->tiposParametros.push_back(tipos.find(tipoAsterisco)->second);
        nextToken();
        if((*leitor)->valor=="MULTPL" && (*leitor)!=tokens.back())
        {
            nextToken();
        }

        if((*leitor)->valor=="TD" && (*leitor)!=tokens.back())
        {
            NoArvore* noNeto =new NoArvore();
            tempV = new Variavel();
            (*leitor)->valor = "VARIAV";
            (*leitor)->classe = CLS_ID;
            tempV->nome=(*leitor)->lexema;
            tempV->tipo=noFilho->tipoRetorno;
            tempV->constante=false;
            tempV->global=false;
            variaveis.push_back(tempV);
            tempF->tiposParametros.push_back(tempV->tipo);
            noNeto->token = (*leitor);
            noNeto->tipoRetorno = noFilho->tipoRetorno;
            noFilho->filhos.push_back(noNeto);
            (*leitor)->valor = "VARIAV";
            (*leitor)->classe = CLS_ID;
            nextToken();
            if((*leitor)->valor=="ABRCOL"  && (*leitor)!=tokens.back())
            {
                nextToken();
                if((*leitor)->valor=="FECCOL"  && (*leitor)!=tokens.back())
                {
                    nextToken();
                    if((*leitor)->valor=="VIRGUL"  && (*leitor)!=tokens.back())
                        nextToken();
                }
                else
                {
                    erro();
                }
            }
            else if((*leitor)->valor=="VIRGUL"  && (*leitor)!=tokens.back())
            {
                nextToken();
            }
        }
        else if((*leitor)->valor=="VIRGUL")
        {
            nextToken();
        }
    }
    else
    {
        erro();
    }
    return noFilho;
}

void removeVarLocais()
{
    list<Variavel*> tempVar;
    int j = 0, contador = 0;
    itVar = variaveis.begin();
    while(j != (int)variaveis.size())
    {
        if((*itVar)->global == true)
        {
            tempVar.push_back((*itVar));
            contador++;
        }
        else
        {
            //cout<<"Nao copia local => "<<(*itVar)->nome<<endl;
        }
        j++;
        itVar++;
    }
    variaveis.clear();
    if(contador>0)
    {
        j = 0;
        itVar = tempVar.begin();
        while(j != (int)tempVar.size())
        {
            variaveis.push_back(*itVar);
            j++;
            itVar++;
        }
    }
}

NoArvore* trataDeclaracaoFuncao()
{
    noDF = new NoArvore();
    tempF = new Funcao();
    if((*leitor)->lexema==tipos.find((*leitor)->lexema)->first)
    {
        noDF->tipoRetorno = (tipos.find((*leitor)->lexema)->second);//insere retorno na árvore temp
        tempF->tipoRetorno = (tipos.find((*leitor)->lexema)->second);
        retornoFuncaoEmDefinicao = (tipos.find((*leitor)->lexema)->second);
        (*leitor)->valor = "FUNCAO";
        (*leitor)->classe = CLS_ID;
        nextToken();
    }
    else
    {
        erro();
    }
    if(!funcoes.empty())
    {
        getFuncao();
        if(!achouFuncao)
        {
            noDF->token=(*leitor);
            tempF->nome=(*leitor)->lexema;
        }
    }
    else
    {
        noDF->token=(*leitor);
        tempF->nome=(*leitor)->lexema;
    }

    if((*leitor)!=tokens.back())
    {
        nextToken();
    }
    else
    {
        erro();
    }
    if((*leitor)->valor=="ABPAR" && (*leitor)!=tokens.back())
    {
        nextToken();
    }
    else
    {
        erro();
    }

    while((*leitor)->valor!="FEPAR")
    {
        noDF->filhos.push_back(trataParametrosDF());
    }
    if((*leitor)!=tokens.back())
    {
        nextToken();
    }
    else
    {
        erro();
    }
    funcoes.push_back(tempF);
    tempF->tiposParametros.clear();
    varGlobal = false;
    if((*leitor)->valor=="ABCHAV" && (*leitor)!=tokens.back())
    {
        flagDefineFuncao = true;
        NoArvore* chave = new NoArvore();
        chave->token = lerToken("ABCHAV");
        chave->tipoRetorno = tipos.find("void")->second;
        while ((*leitor)->valor != "FECHAV")
        {
            chave->filhos.push_back(trataOperacao());
        }
        flagDefineFuncao = false;
        nextToken();
        noDF->filhos.push_back(chave);
    }
    else if((*leitor)->valor=="PNTVIR")
    {
        if((*leitor)!=tokens.back())
        {
            nextToken();
        }
    }
    else
    {
        erro();
    }
    removeVarLocais();
    return noDF;
}

NoArvore* trataChamadaFuncao()
{
    NoArvore* noCF = new NoArvore();
    if((*leitor)->valor=="TD")
    {
        itFunc=funcoes.begin();
        int i=0;
        //int p;
        bool existeFuncao=false;
        while(i != (int)funcoes.size())
        {
            if((*leitor)->lexema==(*itFunc)->nome)
            {
                existeFuncao = true;
                (*leitor)->valor = "FUNCAO";
                (*leitor)->classe = CLS_ID;
                noCF->token =(*leitor);
                noCF->tipoRetorno = (*itFunc)->tipoRetorno;
                //while(p!=funcoes->tiposParametros.size()){
                //noRaiz->filhos.push_back(funcoes->tipoParametro);
                //funcoes->tiposParametros++;
                //p++;
                //}
                //break;
            }
            itFunc++;
            i++;
        }
        if(!existeFuncao)
        {
            saveError();
            cout<<"ERRO: FUNCAO \""<<(*leitor)->lexema<<"\" NAO EXISTE!"<<endl;
            cout<<"Linha: "<<(*leitor)->linha<<" Coluna: "<<(*leitor)->coluna<<endl;
            exit(0);
        }
    }
    else
    {
        erro();
    }
    nextToken();
    if((*leitor)->valor=="ABPAR")
    {
        nextToken();
        while((*leitor)->valor!="FEPAR")
        {
            noCF->filhos.push_back(trataExpressao());
            if((*leitor)->valor=="VIRGUL")
            {
                nextToken();
            }
        }
        nextToken();
        if((*leitor)->valor=="PNTVIR")
        {
            nextToken();
        }
    }
    else
    {
        erro;
    }
    return noCF;
}
string tipoAsterisco = "";
bool temTipo = false;
NoArvore* trataDeclaracaoVariavel()
{
    NoArvore* noDV = new NoArvore();
    bool temAsterisco = false;
    //cout<<"Leitor: "<<(*leitor)->lexema<<endl;
    if(!temTipo)
    {
        if((*leitor)->classe == CLS_TP)
        {
            noDV->token = (*leitor);
            noDV->tipoRetorno = tipos.find("void")->second;
            nextToken();
            if((*leitor)->valor == "MULTPL")
            {
                prevToken();
                tipoAsterisco = (*leitor)->lexema + "*";
                nextToken();
                nextToken();
            }
            else
            {
                if((*leitor)->valor!="TD")
                {
                    erro();
                }
                prevToken();
                tipoAsterisco = (*leitor)->lexema;
                nextToken();
            }
        }
        else
        {
            erro();
        }
        temTipo = true;
    }
    //cout<<"Leitor: "<<(*leitor)->lexema<<endl;
    while(1)
    {
        if((*leitor)->valor == "TD" && temTipo)
        {
            for (list<Variavel*>::iterator it = variaveis.begin(); it != variaveis.end(); ++it)
            {
                if ((*leitor)->lexema == (*it)->nome)
                {
                    saveError();
                    cout << "Erro fatal: Variavel '" << (*leitor)->lexema << "' ja declarada na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
                    exit(0);
                }
            }
            NoArvore* noFilho = new NoArvore();
            (*leitor)->valor = "VARIAV";
            (*leitor)->classe = CLS_ID;
            noFilho->token = (*leitor);
            //cout<<tipoAsterisco<<endl;
            noFilho->tipoRetorno = (tipos.find(tipoAsterisco)->second);
            //cout<<noRaiz->tipoRetorno<<endl;
            tempV = new Variavel();
            tempV->nome = (*leitor)->lexema;
            tempV->tipo = (tipos.find(tipoAsterisco)->second);
            tempV->global = !flagDefineFuncao;
            tempV->constante = false;
            nextToken();
            //cout<<"Leitor: "<<(*leitor)->lexema<<endl;
            if((*leitor)->valor == "ABRCOL")
            {
                nextToken();
                if((*leitor)->valor == "NUMERO")
                {
                    string numero = (*leitor)->lexema;
                    size_t p = numero.find(".");
                    if(p == string::npos)
                    {
                        NoArvore* noNeto = new NoArvore();
                        noNeto->token = (*leitor);
                        noNeto->tipoRetorno = tipos.find("int")->second;
                        noFilho->filhos.push_back(noNeto);
                    }
                    else
                    {
                        saveError();
                        cout<<"Os vetores so podem ser do tipo INTEIRO"<<endl;
                        cout<<"Linha: "<<(*leitor)->linha<<", Arquivo: "<<path<<"."<<endl;
                        exit(0);
                    }
                    nextToken();
                    if((*leitor)->valor == "FECCOL")
                    {
                        string txt = tipoAsterisco;
                        size_t pos = txt.find("*");
                        if(pos == string::npos)
                        {
                            noFilho->tipoRetorno = (tipos.find(tipoAsterisco+"*")->second);
                            tempV->tipo = (tipos.find(tipoAsterisco+"*")->second);
                        }
                        else
                        {
                            saveError();
                            cout<<"Este nao implementa ponteiro de ponteiro!"<<endl;
                            cout<<"Linha: "<<(*leitor)->linha<<", Coluna: "<<(*leitor)->coluna<<", Arquivo: "<<path<<"."<<endl;
                            exit(0);
                        }
                        nextToken();
                    }
                    else
                    {
                        erro();
                    }
                }
                else
                {
                    erro();
                }
            }
            variaveis.push_back(tempV);
            if((*leitor)->valor != "ATRIB"){
                noDV->filhos.push_back(noFilho);
            }
            if((*leitor)->valor == "ATRIB")
            {
                prevToken();
                noDV->filhos.push_back(trataExpressao());
                if((*leitor)->valor == "VIRGUL")
                {
                    nextToken();
                }
                else if((*leitor)->valor == "PNTVIR")
                {
                    tipoAsterisco = "";
                    temTipo = false;
                    nextToken();
                    break;
                }
                else
                {
                    erro();
                }
            }
            else if((*leitor)->valor == "VIRGUL")
            {
                nextToken();
            }
            else if((*leitor)->valor == "PNTVIR")
            {
                tipoAsterisco = "";
                temTipo = false;
                nextToken();
                break;
            }
            else
            {
                erro();
            }
        }
        else
        {
            erro();
        }
    }

    return noDV;
}
/*
        for (list<Variavel*>::iterator it = variaveis.begin(); it != variaveis.end(); ++it)
        {
            if (tokenVariavel->lexema == (*it)->nome)
            {
                cout << "Erro fatal: Variavel '" << tokenVariavel->lexema << "' ja declarada na linha " << raiz->token->linha << ", coluna " << raiz->token->coluna << ", arquivo " << path << "." << endl;
                exit(0);
            }
        }
*/

NoArvore* trataChamadaVariavel()
{
    NoArvore* noCV = new NoArvore();
    if((*leitor)->valor == "TD")
    {
        itVar = variaveis.begin();
        int i = 0;
        bool temVariavel = false;
        while(i != (int)variaveis.size())
        {
            if((*leitor)->lexema == (*itVar)->nome)
            {
                (*leitor)->valor = "VARIAV";
                (*leitor)->classe = CLS_ID;
                noCV->token = (*leitor);
                noCV->tipoRetorno = (*itVar)->tipo;
                temVariavel = true;
                break;
            }
            itVar++;
            i++;
        }
        if(!temVariavel)
        {
            saveError();
            cout<<"ERRO: variavel \""<<(*leitor)->lexema<<"\" nao declarada!"<<endl;
            cout<<"Linha: "<<(*leitor)->linha<<", Coluna: "<<(*leitor)->coluna<<", Arquivo: "<<path<<endl;
            exit(0);
        }
        nextToken();
        if((*leitor)->valor == "ABRCOL")
        {
            nextToken();
            if((*leitor)->valor == "NUMERO")
            {
                nextToken();
                if((*leitor)->valor == "FECCOL")
                {
                    nextToken();
                }
                else
                {
                    erro();
                }
            }
            else
            {
                erro();
            }
        }
        if((*leitor)->valor == "ATRIB")
        {
            prevToken();
            noCV->filhos.push_back(trataExpressao());
        }
        if((*leitor)->classe == CLS_OP && (*leitor)->valor != "ATRIB")
        {
            noCV->filhos.push_back(trataExpressao());
        }
    }
    else
    {
        erro();
    }
    return noCV;
}

NoArvore* trataDeclaracaoConstante()
{
    NoArvore* noRaiz = new NoArvore();
    if((*leitor)->valor == "CERQUI")
    {
        nextToken();
        if((*leitor)->valor == "DEFINE")
        {
            nextToken();
            if((*leitor)->valor == "TD")
            {
                tempV = new Variavel();
                tempV->nome = (*leitor)->lexema;
                tempV->tipo = (tipos.find("int")->second);
                tempV->global = true;
                tempV->constante = true;
                noRaiz->token = (*leitor);
                noRaiz->tipoRetorno = (tipos.find("int")->second);
                (*leitor)->valor = "CONSTA";
                (*leitor)->classe = CLS_ID;
                nextToken();
                if((*leitor)->valor == "NUMERO")
                {
                    tempV->tipo = (tipos.find("float")->second);
                    noRaiz->tipoRetorno = (tipos.find("float")->second);
                }
                else if((*leitor)->valor == "SRTLIT")
                {
                    tempV->tipo = (tipos.find("char*")->second);
                    noRaiz->tipoRetorno = (tipos.find("char*")->second);
                }
                variaveis.push_back(tempV);
            }
            else
            {
                erro();
            }
        }
        else
        {
            erro();
        }
    }
    else
    {
        erro();
    }
    return noRaiz;
}
