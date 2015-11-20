using namespace std;

// String para identificar números
char* numbers = (char*) "0123456789";
// Sequência para identificar letras maiúsculas e minúsculas e o caracter _
char* letters = (char*) "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";

/**
 * Entende-se pelo neologismo "tokenizar" como identificar um determinado padrão em uma
 * string, categorizá-lo criando uma instância de Token*, e adicioná-lo à lista global tokens.
 */

/**
 * Converte int em string
 *
 * @param  int     x  Int a ser convertido
 * @return string     String criada
 */
string to_string(int x)
{
    std::stringstream ss;
    ss << x;
    return ss.str();
}

/**
 * Converte char em string
 *
 * @param  int     x  Char a ser convertido
 * @return string     String criada
 */
string to_string(char c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

/**
 * Preenche uma string até alcançar um tamanho requerido. Caso a string já possua tamanho
 * maior ou igual ao requisitado nada é realizado e ela mesma é retornada.
 *
 * @param  string  s    String a ser preenchida
 * @param  int     pad  Tamanho mínimo que a string apresentará
 * @param  char    c    Caracter a ser utilizado como preenchimento, espaço por padrão
 * @param  int     lr   Define se o preenchimento ocorre à direita ou esquerda da string,
 *                      esquerda por padrão
 * @return string       String preenchida
 */
string strpad(string s, int pad, char c = ' ', int lr = 0)
{
    string out = "";

    if (lr == 0)
    {
        for (int i = 0; i < pad - ((int) s.size()); i++)
            out += c;
    }
    out += s;
    if (lr > 0)
    {
        for (int i = 0; i < pad - ((int)s.size()); i++)
            out += c;
    }

    return out;
}

/**
 * Retorna um nome uma constante de classe
 *
 * @param  int     k  Constante a ser nomeada
 * @return string     Nome da classe
 */
string className(int k)
{
    string s = "CLASSE DESCONHECIDA";

    if (k == CLS_PR)
        s = string("PALAVRA RESERVADA");
    else if (k == CLS_PT)
        s = string("PONTUACAO");
    else if (k == CLS_OP)
        s = string("OPERADOR");
    else if (k == CLS_VL)
        s = string("VALOR LITERAL");
    else if (k == CLS_TP)
        s = string("TIPO PRIMITIVO");
    else if (k == CLS_ID)
        s = string("IDENTIFICADOR");

    return s;
}

/**
 * Efetua a leitura do arquivo de código-fonte, carregando todo o seu conteúdo
 *
 * @param  string  path  Caminho do arquivo de código-fonte
 * @return bool          Verdadeiro se o procedimento tiver dado certo
 */
bool readFile(string path)
{
    FILE* file;
    string extension;
    string data;

    if (path.empty())
    {
        cout << "Informe o arquivo a ser analisado:" << endl;
        cin >> path;
    }
    // Proibe a utilizacao do .. para caminhos de arquivo
    if (path.find("..") != string::npos)
    {
        cout << "Arquivo invalido. Nao pode conter '..'.";
        return false;
    }
    if (path.find_last_of(".") == string::npos)
    {
        cout << "Tipo de arquivo invalido.";
        return false;
    }
    extension = path.substr(path.find_last_of(".")+1);
    if (extension != "c" && extension != "cpp" && extension != "h" && extension != "hpp")
    {
        cout << "Tipo de arquivo invalido. ";
        return false;
    }
    if (extension == "cpp" || extension == "hpp")
    {
        cout << "Este compilador nao suporta arquivos C++. ";
        return false;
    }
    file = fopen(path.c_str(), "r");
    if (file == NULL)
    {
        cout << "Erro ao abrir arquivo \""+path+"\"" << endl;
        return false;
    }
    char c = fgetc(file);
    unsigned x;
    while (c != EOF)
    {
        x = c;
        if (x > 128)
        {
            while (x > 128 && !feof(file))
            {
                c = fgetc(file);
                x = c;
            }
            data += '?';
        }
        if (c != EOF)
            data += c;
        c = fgetc(file);
    }
    source_code = data;
    /*
    size_code = strlen(data.c_str());
    source_code = new char[size_code];
    strcpy(source_code, data.c_str());
*/
    return true;
}

/**
 * Tokeniza todos os números do código-fonte carregado
 *
 * @return void
 */
void tokenizaNumeros()
{
    char c;
    int i = 0;
    int line = 1;
    int column = 1;
    for (i = 0; i < size_code; i++)
    {
        c = source_code[i];

        // Encontrou um número não precedido de letra ou underline
        if (strchr(numbers, c) != NULL && (i == 0 || strchr(letters, source_code[i-1]) == NULL))
        {
            Token *token  = new Token();
            token->linha  = line;
            token->coluna = column;
            token->valor  = "NUMERO";
            token->classe = CLS_VL;
            while (i < size_code && strchr(numbers, c) != NULL)
            {
                token->lexema += c;
                i++;
                column++;
                c = source_code[i];
            }
            if (i < size_code-1 && c == '.' && strchr(numbers, source_code[i+1]) != NULL)
            {
                token->lexema += c;
                i++;
                column++;
                c = source_code[i];
                while (i < size_code && strchr(numbers, c) != NULL)
                {
                    token->lexema += c;
                    i++;
                    column++;
                    c = source_code[i];
                }
            }
            tokens.push_back(token);
        }

        // Incrementa a posição da leitura
        if (c == '\n')
        {
            column = 1;
            line++;
        }
        else
        {
            column++;
        }
    }
}

/**
 * Tokeniza todas as strings do código-fonte carregado, desconsiderando comentários.
 * Após a tokenização as strings são substituidas por espaços em branco com a mesma
 * quatidade de caracteres. Nenhuma string deve iniciar em uma linha e terminar em
 * outra
 *
 * @return void
 */
void tokenizaStrings()
{
    bool onString = false, onLineComent = false, onMLineComent = false;
    string s = "", before = "", c2 = "";
    Token* token;
    char c;
    int linha = 1, coluna = 1, i;

    for (i = 0; i < size_code; i++)
    {
        c = source_code[i];
        if (i > 1)
        {
            c2  = source_code[i-1];
            c2 += c;
        }

        if (onString)
            s += c;

        // Abre comentario de linha
        if (c2 == "//" && !onString && !onLineComent && !onMLineComent)
        {
            onLineComent = true;
        }
        // Abre comentario multi-linha
        else if (c2 == "/*" && !onString && !onLineComent && !onMLineComent)
        {
            onMLineComent = true;
        }
        // Abre string
        else if ((c == '"' || c == '\'') && !onString && !onLineComent && !onMLineComent)
        {
            s += c;
            onString = true;
            before = string(source_code).substr(0, i);
        }
        // Fecha string
        else if ((c == '"' || c == '\'') && onString)
        {
            token = new Token();
            token->linha  = linha;
            token->coluna = coluna - s.size() + 1;
            token->lexema = s;
            token->valor  = "STRLIT";
            token->classe = CLS_VL;
            tokens.push_back(token);

            before += strpad("", s.size());
            before += string(source_code).substr(i+1);
            source_code = before;
            onString = false;
            s = "";
        }
        else if (c2 == "*/" && onMLineComent)
        {
            onMLineComent = false;
        }
        else if (c == '\n' && onLineComent)
        {
            onLineComent = false;
        }
        else if (c == '\n' && onString)
        {
            cout << "Erro Fatal: String literal nao finalizada na linha " << linha << " e coluna " << coluna - s.size() << ".";
            saveError();
            exit(0);
        }

        // Incrementa a posição da leitura
        if (c == '\n')
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

/**
 * Imprime a lista de tudo que foi tokenizado a partir do arquivo de código-fonte
 * carregado. Caso seja informado uma string não vazia como caminho, todo o conteúdo
 * é impresso no arquivo informado ao invés da saída padrão.
 *
 * @param  string  outputPath  Path do possível arquivo de sáida
 * @return void
 */
void printTokens(string outputPath)
{
    std::list<Token*>::iterator it;
    int maiorLinha = 5;
    int maiorColuna = 6;
    int maiorClasse = 6;
    int maiorValor = 5;
    int maiorLexema = 6;
    stringstream resultado;
    ofstream file;


    for (it = tokens.begin(); it != tokens.end(); ++it)
    {
        if (maiorLinha < ((int) to_string((*it)->linha).size()))
            maiorLinha = (int) to_string((*it)->linha).size();
        if (maiorColuna < ((int) to_string((*it)->coluna).size()))
            maiorColuna = (int) to_string((*it)->coluna).size();
        if (maiorClasse < ((int) className((*it)->classe).size()))
            maiorClasse = (int) className((*it)->classe).size();
        if (maiorValor < ((int) (*it)->valor.size()))
            maiorValor = (int) (*it)->valor.size();
        if (maiorLexema < ((int) (*it)->lexema.size()))
            maiorLexema = (int) (*it)->lexema.size();
    }
    resultado << strpad("Linha", maiorLinha, ' ', 1) << " | ";
    resultado << strpad("Coluna", maiorColuna, ' ', 1) << " | ";
    resultado << strpad("Classe", maiorClasse, ' ', 1) << " | ";
    resultado << strpad("Valor", maiorValor, ' ', 1) << " | ";
    resultado << "Lexema" << endl;
    resultado << strpad("", maiorLinha, '-', 1) << "-+-";
    resultado << strpad("", maiorColuna, '-', 1) << "-+-";
    resultado << strpad("", maiorClasse, '-', 1) << "-+-";
    resultado << strpad("", maiorValor, '-', 1) << "-+-";
    resultado << strpad("", maiorLexema, '-', 1) << endl;
    for (it = tokens.begin(); it != tokens.end(); ++it)
    {
        //resultado << "(" << (*it)->linha << "," << (*it)->coluna << ")" << " " << (*it)->valor << " " << (*it)->lexema << endl;
        resultado << strpad(to_string((*it)->linha), maiorLinha, ' ', 1) << " | ";
        resultado << strpad(to_string((*it)->coluna), maiorColuna, ' ', 1) << " | ";
        resultado << strpad(className((*it)->classe), maiorClasse, ' ', 1) << " | ";
        resultado << strpad((*it)->valor , maiorValor, ' ', 1) << " | ";
        resultado << (*it)->lexema << endl;
    }

    if (outputPath.empty())
    {
        cout << resultado.str();
    }
    else
    {
        file.open(outputPath, ios_base::out|ios_base::trunc);
        file << resultado.str();
        file.close();
        system(outputPath.c_str());
    }
}

/**
 * Compara a posição de linha e coluna de dois tokens, determinando que deve ser
 * organizado primeiro em uma ordenação.
 *
 * @param  Token*  a  Primeiro token
 * @param  Token*  b  Segundo token
 * @return bool       Verdadeiro caso a deva ficar antes de b
 */
bool comparaTokens(Token* a, Token* b)
{
    bool aFirst;

    if (a->linha < b->linha)
        aFirst = true;
    else if (a->linha > b->linha)
        aFirst = false;
    // mesma linha
    else if (a->coluna < b->coluna)
        aFirst = true;
    else if (a->coluna > b->coluna)
        aFirst = false;
    // mesma linha e coluna
    else
        aFirst = true;

    return aFirst;
}

/**
 * Ordena a lista de tokens de acordo com a posição de linha e coluna de cada um.
 *
 * @return void
 */
void sortTokens()
{
    tokens.sort(comparaTokens);
}

/**
 * Tokeniza todos os operadores do código-fonte carregado
 *
 * @return void
 */
void tokenizarOperadores_()
{
    Token* token;
    char c1, c2;
    string c1c2 = "", valorC1c2 = "", valorC1 = "";
    int linha = 1, coluna = 1;

    for (int i = 0; i < size_code-1; i++)
    {
        c1 = source_code[i];
        c2 = source_code[i+1];

        // Nenhum operador possui letras, números ou espaços
        if (strchr(letters, c1) == NULL && strchr(numbers, c1) == NULL && !isspace(c1))
        {
            c1c2  = c1;
            c1c2 += c2;
            valorC1   = busca(to_string(c1), CLS_OP);
            valorC1c2 = busca(c1c2, CLS_OP);
            if (strchr(letters, c2) == NULL && strchr(numbers, c2) == NULL && !isspace(c2) && valorC1c2 != "TD")
            {
                token = new Token();
                token->linha  = linha;
                token->coluna = coluna;
                token->lexema = c1c2;
                token->valor  = valorC1c2;
                token->classe = CLS_OP;
                tokens.push_back(token);

                i++;
                c1 = source_code[i];
                coluna++;
            }
            else if (valorC1 != "TD")
            {
                token = new Token();
                token->linha  = linha;
                token->coluna = coluna;
                token->lexema = c1;
                token->valor  = valorC1;
                token->classe = CLS_OP;
                tokens.push_back(token);
            }
        }

        // Incrementa a posição da leitura
        if (c1 == '\n')
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

/**
 * Efetua a leitura de um token já esperado na lista e posiciona o leitor no próximo token
 * relevante. Caso o leitor aponte para um token cujo valor de token é diferente do esperado,
 * é impresso um erro e a execução para.
 *
 * @param  string  valorEsperado  Valor de token que espera-se que o leitor esteja apontando
 * @return Token*                 Token que foi lido
 */
Token* lerToken(string valorEsperado)
{
    Token* t;

    // Verifica se o valor apontado pelo leitor é igual ao valor esperado
    if ((*leitor)->valor != valorEsperado)
    {
        cout << "Erro fatal: Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
        saveError();
        exit(0);
    }
    t = (*leitor);
    // Passa para o próximo token relevante
    nextToken();

    // Retorna o token lido
    return t;
}

/**
 * Incrementa o leitor para o próximo token relevante, ignorando comentários e prevenindo o fim de arquivo não esperado
 *
 * @return  Token*  O token no qual o leitor para
 */
Token* nextToken()
{
    Token* t;
    leitor++;
    // Incrementa o leitor até o próximo token relevante
    while (leitor != tokens.end() && (*leitor)->valor == "NAO TOKEN")
        leitor++;

    // Caso o leitor chegue ao fim do arquivo dentro da declaração de uma função é impresso um erro e a execução para
    if (leitor == tokens.end() && flagDefineFuncao)
    {
        cout << "Erro fatal: Fim do arquivo " << path << " nao esperado." << endl;
        saveError();
        exit(0);
    }

    // Caso o leitor já aponte para o último token, é o incremento não é realizado e o leitor fica apontando para o
    // último token válido, ainda que não seja relevante
    if (leitor == tokens.end())
    {
        leitor--;
        t = (*leitor);
        leitor++;
    }
    else
    {
        t = (*leitor);
    }

    return t;
}

/**
 * Decrementa o leitor para o token relevante anterior, ignorando comentários e prevenindo o início de arquivo não esperado
 *
 * @return  Token*  O token no qual o leitor para
 */
Token* prevToken()
{
    // Decrementa o leitor até o token relevante anterior
    if (leitor != tokens.begin())
    {
        leitor--;
        while ((*leitor)->valor == "NAO TOKEN" && leitor != tokens.begin())
            leitor--;
    }
    // Caso ocorra a tentativa de decrementar para um token anterior ao primeiro, é impresso um erro e a execução para
    else
    {
        cout << "Erro interno: Tentativa de acessar um token inexistente do arquivo " << path << " nao esperado." << endl;
        saveError();
        exit(0);
    }

    return (*leitor);
}

/**
 * Monta uma árvore para a estrutura de include, carregando o arquivo incluido e adicionando a
 * árvore montada a partir do mesmo como filha da estrutura de include.
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataInclude()
{
    NoArvore* raiz;
    string fonte;
    list<Token*> tokensTemp;
    int distanciaLeitor;
    string source_codeTemp;
    int size_codeTemp;
    string pathTemp;

    raiz = new NoArvore();
    raiz->tipoRetorno = tipos.find("void")->second;

    lerToken("CERQUI");
    raiz->token = (*leitor);
    lerToken("INCLUD");
    // Caso encontre um "<" é feito o include de uma biblioteca padrão
    if ((*leitor)->valor == "MENOR")
    {
        nextToken();
        lerToken("TD");
        prevToken();
        fonte = (*leitor)->lexema;
        if (!gera_funcoes(fonte+".h"))
        {
            cout << "Erro fatal: Biblioteca nao encontrada \"" << (*leitor)->lexema << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        nextToken();
        lerToken("MAIOR");
    }
    // Caso contrário é feito o include de um arquivo em um subdiretório
    else if ((*leitor)->valor == "STRLIT")
    {
        if ((*leitor)->lexema[0] == '\'')
        {
            cout << "Erro fatal: A string de inclusao \"" << (*leitor)->lexema << "\" deveria ser delimitada por aspas duplas na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        fonte = (*leitor)->lexema;
        fonte = fonte.substr(1, fonte.size()-2);
        // Trata inclusões recursivas
        if (find(pilhaIncluidos.begin(), pilhaIncluidos.end(), fonte) != pilhaIncluidos.end())
        {
            cout << "Erro fatal: O arquivo \"" << fonte << "\" provoca inclusao recursiva na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }

        // Salva o status do leitor em uma variável temporária para possibilitar restaurá-lo depois
        tokensTemp      = tokens;
        distanciaLeitor = distance(tokens.begin(), leitor);
        source_codeTemp = source_code;
        size_codeTemp   = size_code;
        pathTemp        = path;
        // Reinicia as variáveis
        tokens = list<Token*>();
        source_code = "";
        size_code   = 0;
        path        = fonte;
        pilhaIncluidos.push_back(path);

        // Faz a leitura do arquivo
        if (!readFile(fonte))
        {
            cout << "Erro fatal: Arquivo " << (*leitor)->lexema << " invalido na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        size_code = source_code.size();

        // Tokeniza o arquivo
        tokenizaStrings();
        retiraComentarios();
        size_code = source_code.size();
        tokenizaPontuacao();
        tokenizaNumeros();
        tokenizarOperadores();
        tokenizaPalavraReservada();
        sortTokens();

        // Monta a árvore do arquivo
        leitor = tokens.begin();

        // Posiciona no primeiro token valido
        while ((*leitor)->valor == "NAO TOKEN")
            leitor++;

        // Executa o mesmo algoritmo definido na função principal
        do
        {
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
            else if ((*leitor)->classe == CLS_TP)
            {
                nextToken();
                if ((*leitor)->valor == "MULTPL")
                    nextToken();
                lerToken("TD");
                if ((*leitor)->valor == "ABPAR")
                {
                    prevToken();
                    prevToken();
                    if ((*leitor)->valor == "MULTPL")
                        prevToken();

                    raiz->filhos.push_back(trataDeclaracaoFuncao());
                }
                else if ((*leitor)->valor == "ATRIB" || (*leitor)->valor == "PNTVIR")
                {
                    prevToken();
                    prevToken();
                    if ((*leitor)->valor == "MULTPL")
                        prevToken();

                    raiz->filhos.push_back(trataDeclaracaoVariavel());
                }
            }
            else
            {
                cout << "Erro fatal: Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
                saveError();
                exit(0);
            }
        }
        while (leitor != tokens.end());

        if (pilhaIncluidos.back() != path)
        {
            cout << "Erro interno: Pilha de arquivos incluidos inconsistente ao fim do arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }

        // Restaura as variáveis globais
        tokens = tokensTemp;
        leitor = tokens.begin();
        for (int i = 0; i < distanciaLeitor; i++)
            leitor++;
        source_code = source_codeTemp;
        size_code   = size_codeTemp;
        path        = pathTemp;
        pilhaIncluidos.pop_back();
        nextToken();
    }

    return raiz;
}

/**
 * Monta uma árvore para a estrutura de um bloco de código fonte iniciando em "{" e terminando em "}"
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataBloco()
{
    NoArvore* raiz = new NoArvore();
    raiz->token = lerToken("ABCHAV");
    raiz->tipoRetorno = tipos.find("void")->second;
    // Chama a trata operação enquanto não encontrar um "}"
    while ((*leitor)->valor != "FECHAV")
    {
        raiz->filhos.push_back(trataOperacao());
    }
    nextToken();

    return raiz;
}

/**
 * Faz os devidos tratamentos e verificações para invocar a função que trata uma operação dentro do código fonte
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataOperacao()
{
    string t1, t2;
    t1 = (*leitor)->valor;
    nextToken();
    t2 = (*leitor)->valor;
    prevToken();
    if (t1 == "TD" && t2 == "ABPAR")
    {
        return trataChamadaFuncao();
    }
    else if ((*leitor)->classe == CLS_TP)
    {
        return trataDeclaracaoVariavel();
    }
    else if (t1 == "TD")
    {
        NoArvore* expressao = trataExpressao();
        if  (
                expressao->token->classe != CLS_OP
                || (
                    expressao->token->valor != "ATRIB"
                    && expressao->token->valor != "SOMIGU"
                    && expressao->token->valor != "SUBIGU"
                    && expressao->token->valor != "DIVIGU"
                    && expressao->token->valor != "MULTIG"
                    && expressao->token->valor != "PERIGU"
                )
            )
        {
            cout << "Esperava-se uma atribuicao, na linha " << expressao->token->linha << ", coluna " << expressao->token->coluna << "." << endl;
            saveError();
            exit(0);
        }
        lerToken("PNTVIR");
        return expressao;
    }
    else if (t1 == "IF")
    {
        return trataIf();
    }
    else if (t1 == "DO")
    {
        return trataDo();
    }
    else if (t1 == "WHILE")
    {
        return trataWhile();
    }
    else if (t1 == "FOR")
    {
        return trataFor();
    }
    else if (t1 == "SWITCH")
    {
        return trataSwitch();
    }
    else if (t1 == "RETURN")
    {
        NoArvore* raiz = new NoArvore();
        NoArvore* expressao;
        raiz->token = lerToken("RETURN");
        raiz->tipoRetorno = tipos.find("void")->second;
        expressao = trataExpressao();

        if  (
                retornoFuncaoEmDefinicao != expressao->tipoRetorno
                &&
                (
                    (
                        retornoFuncaoEmDefinicao == tipos.find("double")->second
                        && expressao->tipoRetorno != tipos.find("int")->second
                        && expressao->tipoRetorno != tipos.find("float")->second
                    )
                    ||
                    (
                        retornoFuncaoEmDefinicao == tipos.find("float")->second
                        && expressao->tipoRetorno != tipos.find("int")->second
                    )
                )
            )
        {
            cout << "Erro fatal: Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        lerToken("PNTVIR");
        raiz->filhos.push_back(expressao);
        return raiz;
    }
    else if (t1 == "CONTIN")
    {
        NoArvore* raiz = new NoArvore();
        raiz->token = lerToken("CONTIN");
        raiz->tipoRetorno = tipos.find("void")->second;
        lerToken("PNTVIR");
        return raiz;
    }
    else if (t1 == "BREAK")
    {
        NoArvore* raiz = new NoArvore();
        raiz->token = lerToken("BREAK");
        raiz->tipoRetorno = tipos.find("void")->second;
        lerToken("PNTVIR");
        return raiz;
    }
    else
    {
        cout << "Erro fatal: Expressao nao esperada \"" << (*leitor)->valor << "\" na linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", arquivo " << path << "." << endl;
        saveError();
        exit(0);
    }

    return new NoArvore();
}

/**
 * Monta uma árvore para a estrutura de um "if" incluindo as estruturas de "else" conseguintes
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataIf()
{
    NoArvore* raiz = new NoArvore();
    raiz->token = lerToken("IF");
    raiz->tipoRetorno = tipos.find("void")->second;

    lerToken("ABPAR");
    raiz->filhos.push_back(trataExpressao());
    lerToken("FEPAR");

    if ((*leitor)->valor == "ABCHAV")
    {
        raiz->filhos.push_back(trataBloco());
    }
    else
    {
        raiz->filhos.push_back(trataOperacao());
    }
    if ((*leitor)->valor == "ELSE")
    {
        nextToken();
        if ((*leitor)->valor == "ABCHAV")
        {
            raiz->filhos.push_back(trataBloco());
        }
        else
        {
            raiz->filhos.push_back(trataOperacao());
        }
    }

    return raiz;
}

/**
 * Monta uma árvore para a estrutura do primeiro "case" de um switch, os "cases" conseguintes, e o possível
 * "default" encontrado, são adicionados como filhos até que o bloco de "switch" termine
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataCase()
{
    NoArvore* raiz = new NoArvore();
    NoArvore* constante;
    NoArvore* bloco;
    if ((*leitor)->valor == "CASE")
        raiz->token = lerToken("CASE");
    else
        raiz->token = lerToken("DEFAUL");
    raiz->tipoRetorno = tipos.find("void")->second;

    if (raiz->token->valor == "CASE")
    {
        if ((*leitor)->valor == "TD")
        {
            constante = trataChamadaVariavel();
            if (constante->token->valor != "CONSTA" || constante->tipoRetorno != tipos.find("int")->second)
            {
                cout << "Expressoes para CASEs devem ser inteiros constantes, na linha " << constante->token->linha << ", coluna " << constante->token->coluna << ", arquivo " << path << "." << endl;
                saveError();
                exit(0);
            }
        }
        else
        {
            constante = new NoArvore();
            constante->token = lerToken("NUMERO");
            if (constante->token->lexema.find(".") != string::npos)
            {
                cout << "Expressoes para CASEs devem ser inteiros constantes, na linha " << constante->token->linha << ", coluna " << constante->token->coluna << "." << endl;
                saveError();
                exit(0);
            }
            constante->tipoRetorno = tipos.find("int")->second;
        }
        raiz->filhos.push_back(constante);
    }

    bloco = new NoArvore();
    bloco->token = lerToken("DPONTO");
    bloco->tipoRetorno = tipos.find("void")->second;
    // Chama a trataOperacao ou a trataCase até encontrar a "}" que finaliza o switch
    while ((*leitor)->valor != "FECHAV")
    {
        if ((*leitor)->valor == "CASE" || (*leitor)->valor == "DEFAUL")
            bloco->filhos.push_back(trataCase());
        else
            bloco->filhos.push_back(trataOperacao());
    }
    raiz->filhos.push_back(bloco);

    return raiz;
}

/**
 * Monta uma árvore para a estrutura de um "switch"
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataSwitch()
{
    NoArvore* raiz = new NoArvore();
    NoArvore* parentese = new NoArvore();
    NoArvore* expressao;
    raiz->token = lerToken("SWITCH");
    raiz->tipoRetorno = tipos.find("void")->second;

    parentese->token = lerToken("ABPAR");
    parentese->tipoRetorno = tipos.find("void")->second;
    // expressao = trataExpressao();
    expressao = trataChamadaVariavel();
    // imprimeNoArvore(expressao);
    if (expressao->tipoRetorno != tipos.find("int")->second)
    {
        cout << "Expressoes switch devem retornar um valor inteiro, na linha " << expressao->token->linha << ", coluna " << expressao->token->coluna << "." << endl;
        saveError();
        exit(0);
    }
    parentese->filhos.push_back(expressao);
    raiz->filhos.push_back(parentese);
    lerToken("FEPAR");
    lerToken("ABCHAV");
    raiz->filhos.push_back(trataCase());
    lerToken("FECHAV");

    return raiz;
}

/**
 * Imprime os lexemas dos tokens de uma árvore em um arquivo fazendo o devido recuo para que os filhos
 * sejam identificados. Caso o caminho do arquivo esteja vazio a saída é direcionada para a saída
 * padrão, e a string padrão para indentação são dois espaços.
 *
 * @param  raiz    Raiz da árvore a ser impressa
 * @param  outputPath  Caminho do arquivo de saída
 * @param  indent  String utilizada para indentação
 */
void imprimeArvore(NoArvore* raiz, string outputPath = "", string indent = "  ")
{
    int i, recuo = -1;
    vector<int> indices;
    vector<NoArvore*> pilhaNos;
    stringstream resultado;
    ofstream file;

    pilhaNos.push_back(raiz);
    indices.push_back(0);
    while (pilhaNos.size() > 0)
    {
        // Imprime o lexema atual
        if (pilhaNos.back()->token != NULL)
        {
            for (i = 0; i < recuo; i++)
                resultado << indent;
            resultado << pilhaNos.back()->token->lexema;
        }
        resultado << endl;

        if (indices.back() < (int) pilhaNos.back()->filhos.size())
        {
            pilhaNos.push_back(pilhaNos.back()->filhos[indices.back()]);
            indices[indices.size()-1]++;
            indices.push_back(0);
            recuo++;
        }
        else
        {
            while (pilhaNos.size() > 0 && indices.back() == (int) pilhaNos.back()->filhos.size())
            {
                indices.pop_back();
                pilhaNos.pop_back();
                recuo--;
            }
            if (pilhaNos.size() > 0 && indices.back() < (int) pilhaNos.back()->filhos.size())
            {
                pilhaNos.push_back(pilhaNos.back()->filhos[indices.back()]);
                indices[indices.size()-1]++;
                indices.push_back(0);
                recuo++;
            }
        }
    }

    if (outputPath.empty())
    {
        cout << resultado.str();
    }
    else
    {
        file.open(outputPath, ios_base::out|ios_base::trunc);
        file << resultado.str();
        file.close();
        system(outputPath.c_str());
    }
}

/**
 * Imprime as informações do token para o qual o leitor está apontando
 */
void status()
{
    cout << "Linha " << (*leitor)->linha << ", coluna " << (*leitor)->coluna << ", token " << (*leitor)->valor << ", lexema " << (*leitor)->lexema << "." << endl;
}

/**
 * Imprime as informações de um nó de árvore
 * @param  NoArvore*  Nó a ser impresso
 */
void imprimeNoArvore(NoArvore* no)
{
    string tipoNome = "";
    map<string, int>::iterator it;

    for (it = tipos.begin(); it != tipos.end(); ++it)
    {
        if (it->second == no->tipoRetorno)
            tipoNome = it->first;
    }
    cout << "-------------------------------------" << endl;
    cout << "token.valor " << no->token->valor << endl;
    cout << "token.lexema " << no->token->lexema << endl;
    cout << "token.classe " << className(no->token->classe) << endl;
    cout << "linha-coluna (" << no->token->linha << ", " << no->token->coluna << ")" << endl;
    cout << "tipo " << tipoNome << " (" << no->tipoRetorno << ")" << endl;
    cout << "num filhos " << no->filhos.size() << endl;
    cout << "-------------------------------------" << endl;
}


/**
 * Monta a estrutura de uma chamada à um identificador de variável
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataChamadaVariavel_()
{
    string nomeTipo = "";
    NoArvore* indiceArranjo;
    NoArvore* raiz = new NoArvore();
    raiz->token = lerToken("TD");

    // Busca a variavel no escopo disponivel
    for (list<Variavel*>::iterator it = variaveis.begin(); it != variaveis.end(); ++it)
    {
        if (raiz->token->lexema == (*it)->nome)
        {
            raiz->token->classe = CLS_ID;
            raiz->token->valor = "VARIAV";
            raiz->tipoRetorno = (*it)->tipo;
        }
    }
    // Erro para variaveis nao declaradas
    if (raiz->token->valor == "TD")
    {
        cout << "Erro fatal: Variavel '" << raiz->token->lexema << "' nao declarada na linha " << raiz->token->linha << ", coluna " << raiz->token->coluna << ", arquivo " << path << "." << endl;
        saveError();
        exit(0);
    }
    // Trata chamadas de arranjos
    if ((*leitor)->valor == "ABRCOL")
    {
        indiceArranjo = new NoArvore();
        indiceArranjo->token = lerToken("NUMERO");
        if (indiceArranjo->token->lexema.find(".") != string::npos)
        {
            cout << "Expressoes para indice de arranjos devem ser inteiros constantes, na linha " << indiceArranjo->token->linha << ", coluna " << indiceArranjo->token->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        indiceArranjo->tipoRetorno = tipos.find("int")->second;
        raiz->filhos.push_back(indiceArranjo);
        lerToken("FECCOL");

        // Trata o acesso a ponteiros
        for (map<string, int>::iterator itTipos = tipos.begin(); itTipos != tipos.end(); ++itTipos)
        {
            if (raiz->tipoRetorno == itTipos->second)
                nomeTipo = itTipos->first;
        }
        // Deve ser impossivel que não seja encontrado um tipo de ponteiros, mas por via das duvidas ele emite um erro interno em um caso assim
        if (nomeTipo == "")
        {
            cout << "Erro interno " << raiz->token->linha << ", coluna " << raiz->token->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
        if (nomeTipo.find("*") == 0)
        {
            raiz->tipoRetorno = tipos.find(nomeTipo.substr(1))->second;
        }
        else
        {
            cout << "Tentativa de acessar um ponteiro em uma variavel comum na linha " << indiceArranjo->token->linha << ", coluna " << indiceArranjo->token->coluna << ", arquivo " << path << "." << endl;
            saveError();
            exit(0);
        }
    }

    return raiz;
}

/**
 * Monta a estrutura da declaração de uma variável
 *
 * @return  NoArvore*  A estrutura de árvore que foi montada
 */
NoArvore* trataDeclaracaoVariavel_()
{
    string ponteiro = "";
    Variavel* variavel;
    Token* tokenVariavel;
    NoArvore* noVariavel;
    NoArvore* tamanhoArranjo = NULL;
    NoArvore* raiz = new NoArvore();
    raiz->token = lerToken("TPPRIM");
    raiz->tipoRetorno = tipos.find("void")->second;

    while (true)
    {
        if ((*leitor)->valor == "MULTPL")
            ponteiro = lerToken("MULTPL")->lexema;

        tokenVariavel = lerToken("TD");
        // Verifica se a variavel ja foi declarada
        for (list<Variavel*>::iterator it = variaveis.begin(); it != variaveis.end(); ++it)
        {
            if (tokenVariavel->lexema == (*it)->nome)
            {
                cout << "Erro fatal: Variavel '" << tokenVariavel->lexema << "' ja declarada na linha " << raiz->token->linha << ", coluna " << raiz->token->coluna << ", arquivo " << path << "." << endl;
                saveError();
                exit(0);
            }
        }
        // Trata declaração de arranjos
        if ((*leitor)->valor == "ABRCOL")
        {
            ponteiro = "*";
            nextToken();
            tamanhoArranjo = new NoArvore();
            tamanhoArranjo->token = lerToken("NUMERO");
            if (tamanhoArranjo->token->lexema.find(".") != string::npos)
            {
                cout << "Expressoes para tamanho de arranjos devem ser inteiros constantes, na linha " << tamanhoArranjo->token->linha << ", coluna " << tamanhoArranjo->token->coluna << ", arquivo " << path << "." << endl;
                saveError();
                exit(0);
            }
            tamanhoArranjo->tipoRetorno = tipos.find("int")->second;
            lerToken("FECCOL");
        }
        // Cria e adiciona a variavel
        variavel = new Variavel();
        variavel->nome = tokenVariavel->lexema;
        variavel->constante = false;
        variavel->global = !flagDefineFuncao;
        variavel->tipo = tipos.find(ponteiro+raiz->token->lexema)->second;
        variaveis.push_back(variavel);
        if ((*leitor)->valor == "ATRIB")
        {
            if (tamanhoArranjo != NULL)
            {
                prevToken();
                prevToken();
                prevToken();
            }
            prevToken();
            raiz->filhos.push_back(trataExpressao());
        }
        else
        {
            // Corrige as informacoes do token da variavel
            tokenVariavel->classe = CLS_ID;
            tokenVariavel->valor = "VARIAV";
            // Cria o no da arvore que aponta para o token do identificador e adiciona-o aos filhos
            noVariavel = new NoArvore();
            noVariavel->token = tokenVariavel;
            noVariavel->tipoRetorno = variavel->tipo;
            if (tamanhoArranjo != NULL)
                noVariavel->filhos.push_back(tamanhoArranjo);
            raiz->filhos.push_back(noVariavel);
        }
        // Verifica se ja e o fim da declaracao
        if ((*leitor)->valor == "PNTVIR")
        {
            lerToken("PNTVIR");
            break;
        }
        else
        {
            lerToken("VIRGUL");
        }
        ponteiro = "";
        tamanhoArranjo = NULL;
    }

    return raiz;
}
