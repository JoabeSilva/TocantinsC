//EEEIIII!!!!!!!!!!!!!
// O código tá meio cachorro, nada elegante, mas é o que temos até acabar a FECOIMP
// Se não gostar do nome dos tokens pode entrar  mo arquivo e alterar os tokens
using namespace std;

void gera_tipos ()
{
    char ch;
    int i=0;
    string palavra="";
    FILE* arq = fopen("tipos.txt", "r");
    if(arq == NULL)
    {
        printf("Erro, nao foi possivel abrir o arquivo\n");
    }
    else
    {
        while( (ch=fgetc(arq))!= EOF )
        {
            if (ch=='\n')
            {
                tipos[palavra]=i;
                i++;
                palavra="";
            }
            else
            {
                palavra=palavra+ch;
            }
        }
    }
    fclose(arq);
}

bool gera_funcoes (string path)
{
    Funcao* funcao = new Funcao();
    string nome;
    string tipoParametro;
    string tipoRetorno;
    char ch;
    int valor=0;
    FILE* arq = fopen(path.c_str(), "r");
    if(arq == NULL)
    {
        return false;
    }
    else
    {
        while( (ch=fgetc(arq))!= EOF )
        {
            if (ch=='\n')
            {
                //cout<<funcao->nome<<endl;;
                funcoes.push_back(funcao);
                funcao = new Funcao();
                //cout<<funcao->tiposParametros.size()<<endl;
                //funcao->tiposParametros.clear();
                valor=0;
            }
            else if (ch==' ')
            {
                valor=1;
                funcao->tipoRetorno=tipos[tipoRetorno];
                tipoRetorno="";
            }
            else if (ch=='(')
            {
                valor=2;
                funcao->nome=nome;
                nome="";
            }
            else
            {
                if (valor==0)
                    tipoRetorno=tipoRetorno+ch;
                else if (valor==1)
                    nome=nome+ch;
                else if (valor==2)
                {
                    do
                    {
                        if (ch==',')
                        {
                            funcao->tiposParametros.push_back(tipos[tipoParametro]);
                            tipoParametro="";
                        }
                        else
                            tipoParametro=tipoParametro+ch;
                    }
                    while( (ch=fgetc(arq))!= ')');
                    funcao->tiposParametros.push_back(tipos[tipoParametro]);

                    tipoParametro="";
                    ch=fgetc(arq);
                }
            }
        }
    }
    fclose(arq);

    return true;
}

int melhorTamanhoHash(int populacao, int maiorTeste)
{
    int n, fator, i, j, colisoes, resultado;
    map <int,int> table;

    fator = populacao+2;
    resultado = 0;
    for (i = 1; i <= maiorTeste; i++)
    {
        // Inicializa o mapa de colisoes
        for (j = 0; j < i; j++)
        {
            table[j] = 0;
        }

        colisoes = 0;
        for (j = 0; j < populacao; j++)
        {
            n = rand();
            // Incrementa a quantidade de colisoes nesta posicao de hash
            table[n%i]++;
            // Armazena o hash que tiver a maior quantidade de colisoes
            if (table[n%i] > colisoes)
            {
                colisoes = table[n%i];
            }
        }

        if (colisoes+i < fator)
        {
            fator = colisoes+i;
            resultado = i;
        }
    }

    return resultado;
}

int hashString(string s)
{
    return (s.size() * s[0] + s[1]) % tamanhoHash;
}

// recebe uma palavra e retorna um token, no caso de não encontrar a palavra "td"
string busca (string v, int classe)
{
    list<PadraoToken*> lista = tabelaHashDicionario.find(hashString(v))->second;
    list<PadraoToken*>::iterator it;

    for (it = lista.begin(); it != lista.end(); ++it)
    {
        if ((*it)->palavra == v && (*it)->classe == classe)
            return (*it)->token;
    }

    return "TD";
}

vector<string> busca_classe (int classe)
{
    vector<string> palavras;
    list<PadraoToken*> lista;
    map<int, list<PadraoToken*>>::iterator itTabela;
    list<PadraoToken*>::iterator itLista;

    if (classe>2)
        return palavras;
    for (itTabela = tabelaHashDicionario.begin(); itTabela != tabelaHashDicionario.end(); ++itTabela)
    {
        for (itLista = itTabela->second.begin(); itLista != itTabela->second.end(); ++itLista)
        {
            if ((*itLista)->classe == classe)
                palavras.push_back((*itLista)->palavra);
        }
    }

    return palavras;
}

// função que lê o arquivo, concatena as palavras, ao achar uma tabulação, concatena os tokens, joga tudo numa lista
void gera ()
{
    int classe = 0;
    char url[] = "palavras.txt";
    char ch;
    int boole;
    int i = 0;
    int valor = 0;
    string palavra  = "";
    string palavra1 = "";
    string resultado;
    string resultado1;
    string toke = "";
    FILE *arq;

    arq = fopen(url, "r");
    if(arq == NULL)
    {
        printf("Erro, nao foi possivel abrir o arquivo\n");
    }
    else
    {
        tamanhoHash = 1;
        while ((ch = fgetc(arq)) != EOF)
        {
            if (ch == '\n')
                tamanhoHash++;
        }
        tamanhoHash = melhorTamanhoHash(tamanhoHash, tamanhoHash);
        fclose(arq);
        arq = fopen(url, "r");
        while ((ch = fgetc(arq)) != EOF)
        {
            if (ch == '\n')
            {
                valor = 0;
                if (boole == 1)
                {
                    classe++;
                }
                else
                {
                    palavra = palavra1;
                    PadraoToken *padrao = new PadraoToken();
                    padrao->info    = i;
                    padrao->palavra = palavra;
                    padrao->token   = toke;
                    padrao->classe  = classe;
                    tabelaHashDicionario[hashString(palavra)].push_back(padrao);
                    //lista=insere(lista,i, palavra, toke,classe);
                    palavra1 = "";
                    toke     = "";
                    i++;
                }
                boole = 1;
            }
            else if (ch=='\t')
            {
                valor = 1;
                boole = 2;
            }
            else
            {
                if (valor ==0)
                {
                    resultado = palavra1 + ch;
                    palavra1  = resultado;
                }
                else
                {
                    resultado1 = toke + ch;
                    toke       = resultado1;
                }
            }
        }
    }

    fclose(arq);
}

void imprimeHash()
{
    list<PadraoToken*> lista;
    map<int, list<PadraoToken*>>::iterator itTabela;
    list<PadraoToken*>::iterator itLista;

    cout << "Dicionario: " << endl;
    for (itTabela = tabelaHashDicionario.begin(); itTabela != tabelaHashDicionario.end(); ++itTabela)
    {
        for (itLista = itTabela->second.begin(); itLista != itTabela->second.end(); ++itLista)
        {
            cout << (*itLista)->palavra << ";";
        }
        cout << endl;
    }
    cout << "--------------------" << endl;
}
// comentei o main só pra usarem as funções, mas pode descomentar
/*int main()
{

    Lista* lista= new Lista;
    lista = inicializa();

    lista=gera(lista);

    string ch;
    ch=busca(lista,",",2);
    // se for encontrado retorna o próprio token, se não, retorna "td"
    if (ch!="td")
        cout<<"\n \n"<<"Encontrado! Token = "<<ch<<"\n \n";
    else
        cout<<"\n \n"<<"Não encontrado! Token desconhecido \n \n";
    vector<string> p =busca_classe(lista,1);
    for (int i=0; i<p.size();i++)
        cout<<p[i]<<"\n";
        return 0;
}
*/

