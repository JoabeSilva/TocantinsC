using namespace std;

void tokenizaPalavraReservada()
{
    string str = source_code;
    string aux="",aux3="";
    Token* token;
    int linha = 1, coluna = 1;

    for (int i=0; i<((int)str.length()); i++)
    {
        if (((str[i]>=65 && str[i]<=90) or (str[i]>=97 && str[i]<=122) or (str[i] == 95)) or ((str[i]>=48 && str[i]<=57) && aux.size()!=0))
        {
            aux += str[i];
            aux3 = busca(aux, CLS_PR);
            if (strcmp(aux3.c_str(), "TD") != 0)
            {
                // Adapta��o para diferenciar "classe" e "class"
                if ((aux == "class") and (str[i+1] == 'e'))
                {
                    i++;
                    aux+=str[i];
                }

                token = new Token();
                token->linha  = linha;
                token->coluna = coluna - aux.size() + 1;
                token->lexema = aux;
                token->valor  = aux3;
                token->classe = CLS_PR;
                tokens.push_back(token);
                aux="";
            }
            else if (tipos.find(aux) != tipos.end())
            {
                token = new Token();
                token->linha  = linha;
                token->coluna = coluna - aux.size() + 1;
                token->lexema = aux;
                token->valor  = "TPPRIM";
                token->classe = CLS_TP;
                tokens.push_back(token);
                aux="";
            }
        }
        else if (aux != "0" and aux !="1" and aux != "2" and aux != "3" and aux != "4" and aux != "5" and aux !="6" and aux != "7" and aux !="8" and aux !="9")
        {
            if (aux != "" && aux != "cpp")
            {
                token = new Token();
                token->linha  = linha;
                token->coluna = coluna - aux.size();
                token->lexema = aux;
                token->valor  = "TD";
                token->classe = 10;
                tokens.push_back(token);
            }
            aux = "";
        }
        else
        {
            aux = "";
        }

        // Incrementa a posi��o da leitura
        if (str[i] == '\n')
        {
            coluna = 1;
            linha++;
        }
        else
        {
            coluna++;
        }
    }
}

void tokenizaPontuacao()
{
    string str = source_code;
    string aux="", aux2="", aux3="";
    char next;
    Token* token;
    int linha = 1, coluna = 1;

    for (int i=0; i < ((int)str.length()); i++)
    {
        if (not((str[i]>=65 && str[i]<=90) or (str[i]>=97 && str[i]<=122) or (str[i]>=48 && str[i]<=57) or isspace(str[i]) or str[i] == 95))
        {
            aux+=str[i];
            next = ' ';
            if (i+1 < ((int)str.length()))
                next = str[i+1];
            aux2 = busca(aux, CLS_PT);
            aux3 = busca(aux, CLS_OP);

            // � pontua��o reconhecida
            if ( aux2 != "TD" )
            {
                token  = new Token();
                token->linha  = linha;
                token->coluna = coluna;
                token->lexema = aux;
                token->valor  = aux2;
                token->classe = CLS_PT;
                tokens.push_back(token);

                aux  = "";
                aux2 = "";
                aux3 = "";
            }
            // N�o � pontua��o, mas tamb�m n�o � operador
            //else if (busca(lista, aux, CLS_OP) == "td" && isspace(str[i+1]) != 0)
            else if (aux3 == "TD" && ((next>=65 && next<=90) or (next>=97 && next<=122) or (next>=48 && next<=57) or isspace(next) or next == 95))
            {
                token  = new Token();
                token->linha  = linha;
                token->coluna = coluna;
                token->lexema = aux;
                token->valor  = "TD";
                token->classe = 10;
                tokens.push_back(token);

                aux  = "";
                aux2 = "";
                aux3 = "";
            }
            else if (aux3 != "TD")
            {
                aux  = "";
                aux2 = "";
                aux3 = "";
            }
        }

        // Incrementa a posi��o da leitura
        if (str[i] == '\n')
        {
            coluna = 1;
            linha++;
        }
        else
        {
            coluna++;
        }
    }
}

NoArvore* trataDo()
{
    /*
        Esta fun��o � respons�vel por percorrer o arquivo de entrada procurando a chamada do la�o de repeti��o "DO".
    */
    NoArvore* raiz = new NoArvore(); //cria uma nova raiz
    raiz->token = lerToken("DO"); // Verifica se existe a express�o "DO" no dicion�rio, se existir, armazena na raiz->token
    raiz->tipoRetorno = tipos.find("void")->second; // Define o tipo de retorno da fun��o;
    raiz->filhos.push_back(trataBloco()); // Pega todo o conte�do que est� dentro das chaves do "DO"
   // raiz->filhos.push_back(trataWhile()); //Chama a fun��o trataWhile.
    return raiz; // Retorna a estrutura da raiz formada.
}

NoArvore* trataWhile()
{
    /*
        Esta fun��o � respons�vel por percorrer o arquivo de entrada procurando a chamada da condi��o de parada "WHILE".
    */

    stack<string> verificarAbreFecha;//cria uma pilha de string
    NoArvore *t;
    int i = 0;
    t = novoNo(*leitor);

    while(true)
    {
        leitor++; //avan�a uma posi��o no ponteiro que percorre o arquivo.
        i++;
        if((*leitor)->valor == "ABPAR") //Verifica se o ponteiro de leitura est� em um "(" abre par�ntese
            verificarAbreFecha.push((*leitor)->valor); //Insere o abre par�ntese na pilha
        if((*leitor)->valor == "FEPAR")//Verifica se o ponteiro de leitura est� em um ")" fecha par�ntese
        {
            if(!verificarAbreFecha.empty() && //Se a pilha n�o estiver vazia
                verificarAbreFecha.top() == "ABPAR") //e encontrar um "(" abre par�ntese na pilha
            {
                verificarAbreFecha.pop(); //Retira o �ltimo elemento da pilha
            }
            else
            {
                cout << "Erro fatal: Expressao mal formada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ".";
                exit(0);
            }
        }
        if(verificarAbreFecha.empty()) // Verifica se a pilha est� vazia
                    break;


    }

    while(i != 2)
    {
        leitor--;
        i--;
    }
    t->filhos.push_back(trataExpressao()); // Armazenar as express�es de parada do while na arvore;

    lerToken("FEPAR"); //Verifica se existe a express�o "(" fecha par�ntese no dicion�rio, se existir, armazena na raiz->token
     // Verifica se existe a express�o "DO" no dicion�rio, se existir, armazena na raiz->token

    if((*leitor)->valor == "ABCHAV") //Se o caractere for um "{" abre chave
        t->filhos.push_back(trataBloco());//Armazena o conte�do que est� dentro das chaves na �rvore.
    else if((*leitor)->valor == "PNTVIR"){//Se o caractere atual � um ponto e v�rgula ";"
        lerToken("PNTVIR");
        //leitor++;
    }else
        t->filhos.push_back(trataOperacao());// Armazenar as express�es de parada do while na arvore;

    return t;//Retorna a �rvore;

}
