using namespace std;

struct retorno{
string p;
int l;
int c;
};

typedef struct retorno Retorno;

void tokenizarOperadores()
{

    int a =-1;

    bool prime=true, bn=false;
    int linha = 1, coluna=0,espaco=0;;
    string palavra;
    Retorno retorno;
    vector <string> vetor = busca_classe(2);
    vector <string> tmp;
    tmp.clear();
    // string source_code="ai<=oi>>4==\n *= = /";
    Token *toke;
    //

    //if (caminho == NULL)
    //     printf("Erro, nao foi possivel abrir o arquivo\n");
    //else
    do
    {
        a++;
        //cout<<vetor[a]<<endl;
        if (!(tmp.empty())) prime=false;
        if (source_code[a]=='\n')
        {
            linha++;
            coluna=0;
            bn=true;
        }
        else if (source_code[a]==' ')
        {
            coluna++;
            espaco++;

        }

        else
        {

            if ((!prime) and (bn==true))
            {
                palavra=palavra;
            }
            if ((!prime)and(bn==false))
                palavra=palavra+source_code[a];
            else
                palavra=source_code[a];



            //   cout<<palavra<<""<<endl;
            coluna++;
            if (prime)
                for(int j=0; j<((int)vetor.size()); j++)
                {


                    if(vetor[j][0]==source_code[a])
                        tmp.push_back(vetor[j]);
                    if(vetor[j]==palavra){
                        retorno.p=vetor[j];
                        retorno.c=coluna;
                        retorno.l=linha;

                    }

                }


            else
            {
                bool marca=false;
                //  cout<<"T "<<retorno<<endl;
                for(int i=0; i<((int)tmp.size()); i++)

                    if (tmp[i]==palavra)
                    {
                        toke=new Token();
                        toke->classe=2;
                        toke->linha=retorno.l;
                        toke->coluna=retorno.c;
                        toke->valor=busca(tmp[i],2);
                        toke->lexema=tmp[i];
                        tokens.push_back(toke);


                        /*   cout<<"op: "<<tmp[i]<<endl;
                           cout<<"cl: "<<coluna-espaco<<endl;
                           cout<<"ln: "<<linha<<endl;
                           cout<<endl;*/marca=true;
                    }

                if (marca==false)
                {
                    toke=new Token();
                    toke->classe=2;
                    toke->linha=retorno.l;
                    toke->coluna=retorno.c;
                    toke->valor=busca(retorno.p,2);
                    toke->lexema=retorno.p;
                    tokens.push_back(toke);

                    /*
                    cout<<"op: "<<retorno<<endl;
                    cout<<"cl: "<<coluna-1-espaco<<endl;
                    cout<<"ln: "<<linha<<endl;
                    cout<<endl;*/a--;
                    coluna--;
                }
                tmp.clear();
                prime=true;
            }
            // for(int i=0; i<tmp.size(); i++)
            //  cout<<"- "<<tmp[i]<<endl;

            //   if ((k==1) and (q==0))

           // espaco=0;
            bn=false;

        }//if (source_code[a]==' ') break;

    }
    while (source_code[a]!= '\0');

    // string tmp;
// for (int i=0; i<retorno.size(); i++)
    // {
    // tmp=retorno[i].operador;
    // retorno[i].operador=busca(lista,tmp,2);
    // cout<<retorno[i].operador<<endl;

    //}


}

/*
int main()
{
    TokenizarOperadores();
    return 0;
}
*/
