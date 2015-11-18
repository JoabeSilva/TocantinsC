bool ignorar_linha = false;
/**
* Função para retirar comentários
* @param string input
* @param int line
* @return string
*/
std::string strip_comments(std::string& input, int line)
{
	size_t pos1 = input.find("//"); // encontra a posição inicial de um comentário simples
	size_t pos2 = input.find("/*"); // encontra a posição inicial de um comentário multi-linhas
	size_t pos3 = input.find("*/"); // encontra a posição final de um comentário multi-linhas

	if(pos1 != std::string::npos && ignorar_linha == false) // verifica se encontrou o inicio de um comentário simples
	{
        Token *token  = new Token(); // cria um novo token
        token->linha  = line; // seta a linha
        token->coluna = pos1; // seta a coluna
        token->valor  = "NAO TOKEN"; // seta o valor
        token->classe = 10; // seta a classe
        tokens.push_back(token); // insere o token na lista de token
		return input.replace(pos1, input.size(), input.size()-pos1, ' '); // remove comentário
	}
	else if(pos2 != std::string::npos && ignorar_linha == false) // verifica se encontrou o inicio de um comentário multi-linhas
	{
        Token *token  = new Token(); // cria um novo token
        token->linha  = line; // seta a linha
        token->coluna = pos2; // seta a coluna
        token->valor  = "NAO TOKEN"; // seta o valor
        token->classe = 10; // seta a classe
        tokens.push_back(token); // insere o token na lista de token
		ignorar_linha = true; // seta flag para ignorar linha
		return input.replace(pos2, input.size(), input.size()-pos2, ' '); // remove comentário
	}
	else if(pos3 != std::string::npos) // verifica se encontrou o final de um comentário multi-linhas
	{
		ignorar_linha = false; // seta flag para não ignorar linha
		return input.replace(0, pos3+2, pos3+2, ' '); // remove comentário
	}
	else if(ignorar_linha == true) // verifica se a linha é para ser ignorada
		return input.replace(1, input.size()-1, input.size()-1, ' '); // remove comentário
	else
		return input; // retorna linha
}

/**
* Função para percorrer arquivo
* @return void
*/
void retiraComentarios()
{
    string line = "", newCode = "";
    int lineNumber = 0;

	for (int i = 0; i < size_code; i++)
	{
        if (source_code[i] == '\n') // verifica se econtrou o final de uma linha
        {
            newCode += strip_comments(line, lineNumber++); // elimina comentário
            line = "";
        }
	    line += source_code[i];
	}
	newCode += strip_comments(line, lineNumber++);
	source_code = newCode;
}

/**
* Função para tratar a estrutura for retornando a árvore desta estrutura
* @return NoArvore
*/
NoArvore* trataFor()
{
	vector<string> vetorFor = {"FEPAR", "PNTVIR", "PNTVIR", "ABPAR"}; // vetor de inicializações dos elementos obrigatórios em um for
	NoArvore *t, *parentesis; // instancia os nós da árvore, t é nó principal, parentesis é o nó auxiliar
	t = novoNo(*leitor); // inicialiaza o nó inicial da árvore for

	while(true) // inicializa o loop que percorre o for
	{
		leitor++; // itera para o próximo elemento
		if(vetorFor.back() == (*leitor)->valor) // verifica se o último elemento da 'pilha' é igual ao atual elemento
		{
			if(vetorFor.size() == 4 && (*leitor)->valor == "ABPAR") // verifica se a 'pilha' possui 4 elementro e se o elemento atual é um abre parentesis
				parentesis = novoNo(*leitor); // inicialiaza o nó inicial do conteúdo dos parentesis
			vetorFor.pop_back(); // remove o último elemento da 'pilha'
			leitor++; // itera para o próximo elemento
			if(vetorFor.empty()) // verifica se há elementos na 'pilha'
				break; // quebra o loop se a pilha estiver vazia

			if(vetorFor.back() != (*leitor)->valor) // verifica se o último elemento é diferente do valor atual
			{
				if(vetorFor.size() == 3)
				{
					if((*leitor)->classe == CLS_TP)
					{
						status();
						parentesis->filhos.push_back(trataDeclaracaoVariavel()); // adiciona um filho à sub-árvore parentesis utilizando a função trataChamadaVariavel
						status();
						leitor--;
					}
					else if((*leitor)->valor == "TD")
					{
						parentesis->filhos.push_back(trataChamadaVariavel()); // adiciona um filho à sub-árvore parentesis utilizando a função trataChamadaVariavel
						leitor--;
						parentesis->filhos.push_back(trataExpressao()); // adiciona um filho à sub-árvore parentesis utilizando a função trataOperacao
					}
					else
						parentesis->filhos.push_back(trataExpressao()); // adiciona um filho à sub-árvore parentesis utilizando a função trataOperacao

				}
				else
					parentesis->filhos.push_back(trataExpressao()); // adiciona um filho à sub-árvore parentesis utilizando a função trataExpressao

			}
			else
				parentesis->filhos.push_back(novoNo(*leitor)); // adiciona um filho à sub-árvore parentesis utilizando o nó atual
				
			leitor--; // itera para o elemento anterior
		}
		else
		{
			cout << "Erro fatal (trataFor): Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << "."; // imprime um erro
			exit(0); // sai do programa
		}
	}

	t->filhos.push_back(parentesis); // adiciona um filho à árvore 't' utilizando a sub-árvore parentesis

	if((*leitor)->valor == "ABCHAV") // verifica se o elemento atual é igual a abre parentesis
		t->filhos.push_back(trataBloco()); // adiciona um filho à árvore 't' utilizando a função trataBloco
	else
		t->filhos.push_back(trataOperacao()); // adiciona um filho à árvore 't' utilizando a função trataOperacao

	return t; // retorna a árvore 't'
}

/**
* Função para tratar expressão
* @return NoArvore
*/
NoArvore* trataExpressao()
{
	NoArvore *t, *t1, *t2; // instancia os nós da árvore, t é nó principal, t1 e t2 são o nós auxiliares
    stack<string> verificarAbreFecha; // instancia pilha de string para verificar parentesis
	stack<Token*> pilhaToken; // instancia pilha de tokens auxiliando na transforção de infixa para posfixa
	stack<NoArvore*> pilhaArvore; // instancia pilha de tokens auxiliando na transforção de infixa para posfixa
	list<Token*> posfixa; // instancia uma lista de tokens posfixo
	string auxiliar;
	int i = 0;

	while((*leitor)->valor != "PNTVIR" && (*leitor)->valor != "VIRGUL") // inicializa o loop que percorre a expressão até encontrar uma vírgula ou ponto e virgula
	{

		// verifa se a variável foi declarada
		if((*leitor)->valor == "TD")
		{
			leitor++; // itera para o próximo elemento
			auxiliar = (*leitor)->valor; // pega o valor atual do vetor
			leitor--; // itera para o elemento anterior
			if(auxiliar != "ABPAR")
			{
				// itera as variaveis, buscando as mesma
			    for (list<Variavel*>::iterator it = variaveis.begin(); it != variaveis.end(); ++it)
			    {
			        if((*leitor)->lexema == (*it)->nome) // verifica se a variavel já foi declarada
			        	++i; // se foi declarada incrementa
			    }
			    if(i == 0) // se valor for igual a zero, quer dizer que a variável não foi declarada
			    {
			        cout << "Erro fatal: Variavel '" << (*leitor)->lexema << "' nao declarada na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
			        exit(0);
			    }
			    else
			    	i = 0; // seta para zero afim de verificar no próximo loop
			}
		}

		if(eOperando((*leitor)->valor)) // verificar se é um operando
			posfixa.push_back(*leitor); // adiciona o elemento na pilha posfixa
		if((*leitor)->valor == "ABPAR") // verificar se é abre parentesis
		{
			verificarAbreFecha.push((*leitor)->valor); // insere o elemento atual na pilha em forma de string
			pilhaToken.push(*leitor); // insere o elemento atual na pilha em forma de token
		}
		if(!eOperando((*leitor)->valor)) // verifica se é um operador
		{
			// inicializa o loop até a pilha token ficar fazia ou encontrar um parentesis
			while(!pilhaToken.empty() && pilhaToken.top()->valor != "ABPAR" && pegarPesoDoOperador(pilhaToken.top()->valor) >= pegarPesoDoOperador((*leitor)->valor))
			{
				posfixa.push_back(pilhaToken.top()); // adiciona o elemento na pilha posfixa
				pilhaToken.pop(); // retira o último elemento da pilha
			}
			pilhaToken.push(*leitor); // insere o elemento atual na pilha em forma de token
		}
		if((*leitor)->valor == "FEPAR") // verificar se é fecha parentesis
		{
			// inicializa o loop até a pilha token ficar fazia ou encontrar um parentesis
			while(!pilhaToken.empty() && pilhaToken.top()->valor != "ABPAR")
			{
				posfixa.push_back(pilhaToken.top()); // adiciona o elemento na pilha posfixa
				pilhaToken.pop(); // retira o último elemento da pilha
			}
			if(!pilhaToken.empty()) // verifica se a pilha não está vazia
			{
				verificarAbreFecha.pop(); // retira o último elemento da pilha de verificação de parentesis
				pilhaToken.pop(); // retira o último elemento da pilha
			}
		}
		leitor++; // itera para o próximo elemento
		if((*leitor)->valor == "FEPAR" && verificarAbreFecha.empty()) // verifica se o elemento atual é fecha parentesis e confere se a pilha de verificação está vazia
			break; // saí do loop
	}

	// inicializa o loop até a pilha token ficar fazia
	while(!pilhaToken.empty())
	{
		posfixa.push_back(pilhaToken.top()); // retira o último elemento da pilha
		pilhaToken.pop(); // retira o último elemento da pilha
	}

	// inicializa o iterador da lista posfixa transformaça em árvore
	for(list<Token*>::iterator it = posfixa.begin(); it != posfixa.end(); ++it)
	{
		if(eOperando((*it)->valor)) // verifica se é um operando
		{
			t = novoNo(*it); // cria o nó
			pilhaArvore.push(t); // insere o nó na pilha de nós
		}
		if(eOperador((*it)->valor)) // verifica se é um operando
		{
			t = novoNo(*it); // cria o nó

			t1 = pilhaArvore.top(); // retorno o nó topo da pilha
			pilhaArvore.pop(); // retira o último elemento da pilha
			t2 = pilhaArvore.top(); // retorno o nó topo da pilha
			pilhaArvore.pop(); // retira o último elemento da pilha

			t->filhos.push_back(t1); // adiciona um filho à árvore 't' utilizando 't1'
			t->filhos.push_back(t2); // adiciona um filho à árvore 't' utilizando 't2'

			pilhaArvore.push(t); // insere o nó na pilha de nós
		}
	}

	if(!pilhaArvore.empty()) // verifica se a pilha de nós não está vazia
	{
		t = pilhaArvore.top(); // retorno o nó topo da pilha
		pilhaArvore.pop(); // retira o último elemento da pilha

		// inicializa o loop até a pilha de nós ficar fazia
		while(!pilhaArvore.empty())
		{
			t1 = pilhaArvore.top(); // retorno o nó topo da pilha
			pilhaArvore.pop(); // retira o último elemento da pilha
			t->filhos.push_back(t1); // adiciona um filho à árvore 't' utilizando 't1'
		}
	}
	return t; // retorna a árvore 't'
}

/**
* Função para tratar expressão
* @param Token token
* @return NoArvore
*/
NoArvore* novoNo(Token* token)
{
	NoArvore* no = new NoArvore();
	no->token = token;
	return no;
}

/**
* Função para verificar se a string é operador
* @param string C
* @return bool
*/
bool eOperador(string C)
{
	return (C == "PERCEN" || C == "CERQUI" || C == "DIVISA" || C == "ATRIB" || C == "SETA" || C == "MULTPL" || C == "BARINV" || C == "SOMA" || C == "SUBTRA" || C == "MENOR" || C == "MEIGUA" || C == "MAIOR" || C == "MAIGUA" || C == "IGUAL" || C == "NIGUAL" || C == "MULTIG" || C == "DIVIGU" || C == "PERIGU" || C == "SOMIGU" || C == "SUBIGU" || C == "MEMEIG" || C == "MAMAIG" || C == "COMIGU" || C == "POTIGU" || C == "BARIGU" || C == "TIL" || C == "MENMEN" || C == "MAIMAI" || C == "COMERC" || C == "POTENC" || C == "BARVER" || C == "COMCOM" || C == "DBARRA" || C == "INTERR");
}

/**
* Função para verificar se a string é operando
* @param string C
* @return bool
*/
bool eOperando(string C)
{
	return (C == "NUMERO" || C == "TD" || C == "VARIAV" || C == "INCREM" || C == "DECREM" || C == "STRLIT" || C == "TRUE" || C == "FALSE" || C == "FALSE" || C == "ESCLAM");
}

/**
* Função para verificar o peso da string
* @param string op
* @return int
*/
int pegarPesoDoOperador(string op)
{
	int peso;
	if(op == "SOMA" || op == "SUBTRA")
		peso = 1;
	else if(op == "MULTPL" || op == "DIVISA")
		peso = 2;
	else if(op == "POTENC")
		peso = 3;
	else
		peso = -1;

	return peso;
}